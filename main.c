#define MAIN
#include "defs.h"
#include "adc.h"
#include "roboteq.h"
#include "serial.h"
#include "timer.h"
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>

// Function Prototypes
void getSensorData();
void txData(char *, int);
int rxData(char *, int);
void dataToTerminal();

// SYSINIT FUNCTION
// Sets port data directions and initializes systems
void sysInit() {
	DDRA = 0x15;		// A0,2,4 as output
	PORTA = 0xC0;		// enable A6-7's pull-up resistors
	DDRB = 0x00;		// B0-7 as input
	PORTB = 0xFF;		// Enable B's pull-up resistors
	DDRC = 0x0F;		// Set PortC 0-3 as output
	PORTC = 0xF0;		// Enable PortC's pull-up resistors
	DDRD  = 0x80;   	// D3 as output
	PORTD |= 0x80;  	// Turn on the power LED

	rcvrFlag = 0;
	configFlag = 0;
	rxRadioFlag = 0;
	rxWireFlag = 0;
	startDataFlag = 0;
	targetDevice = TERMINAL;
	twentyFiveMS_Timer = INTS_PER_25MS;
	secondTimer = INTS_PER_SECOND;
	roboteqResponseTime = 0;
	roboteqStatus = 0;
	roboteqErrCnt = 0;

	adcInit();
	wireInit();
	radioInit();
	timerInit();
	wireSendString("Starting\r\n");
	sei();
}


// MAIN FUNCTION
// Initializes hardware
// Runs main program loop for Transmitter or Receiver, chosen at compile
int main() {
	int counter = 0;
	char buffer[100];
	sysInit();

	PORTC |= POWER_LED;

	// TRANSMITTER MAIN PROGRAM LOOP
	#ifdef TRANSMITTER 
	while(1) {
		// TODO: This happens every time through the loop, too much!
		// Replace with timer trigger
		getSensorData(); // get 6 adc values and stick them in the buffer

		// Ready command from receiver?
		if (rcvrFlag == 1) { 
			txData(data, NUM_ADC_CHANS + NUM_DIGITAL_CHANS);  // transmit the data buffer
			rcvrFlag = 0;  // set the ready flag low
		}
		
		// received config command from terminal?
		if (configFlag == 1) {
			// command valid?
				// acknowledge the command
				// command for me?
					// execute the command
				// command for receiver?
					// transmit the command
			// command invalid?
				// transmit error message
			configFlag = 0;  // set the config flag low
		}

		//if (BUTTON_PRESSED)
		//	processButton();

		// CALIBRATION?
		// BATTERY LEVEL?
	}
	#endif

	// RECEIVER MAIN PROGRAM LOOP
	#ifdef RECEIVER
	
	while(1) {
		if (roboteqErrCnt > ROBOTEQ_ERROR_LIMIT) 
			roboteqStatus = 0;

		if (roboteqStatus == 0)
			while(roboteqInit() != 1);

		// Am I ready to receive data?
		if (rcvrFlag == 1) {
			if (targetDevice == TERMINAL) {
				wireSendString("Requesting data...\r\n");
			}
			else {
				sprintf(buffer, "Requesting data #%d...\r\n", counter++);
				wireSendString(buffer);
			}
			radioSend(RCVR_READY);
			rcvrFlag = 0;
		}

		// Start byte from transmitter?
		if (startDataFlag == 1) {
			// populate the buffer
			if (rxData(data, NUM_ADC_CHANS + NUM_DIGITAL_CHANS)) {
				if (targetDevice == TERMINAL) {
					dataToTerminal();
				}
				else if (targetDevice == ROBOTEQ) {
					dataToTerminal();
					dataToRoboteq();  // IMPLEMENT THIS!
				}
			}
			else {  // data bad?
				// report error to transmitter?
			}
			startDataFlag = 0;
		}

		// command byte from transmitter?
		if (configFlag == 1) {
			// command roboteq to stop
			// process the command
			// allow roboteq to resume
		}

		// Turn off the associate LED if timer expires
		if (!radioAssocTmr)
			PORTC &= ~RADIO_LED;
	}
	#endif

	return 0;
}


// GETSENSORDATA FUNCTION
// Collects 3 ADC readings, 2 digital readings, and populates the buffer
// Returns nothing
void getSensorData() {
	PORTA = 0xC1;  // Power to ADC channel 1
	data[0] = getADC(1)/4;

	PORTA = 0xC4;  // Power to ADC channel 3
	data[1] = getADC(3)/4;

	PORTA = 0xD0;  // Power to ADC channel 5
	data[2] = getADC(5)/4;
	PORTA = 0xC0;

	data[3] = (PINA & 0x40)?1:0;
	data[4] = (PINA & 0x80)?1:0;
}


// TXDATA FUNCTION
// Sends start byte, size unsigned chars, and a checksum of the added chars
// Returns nothing
// TODO: Switch to choose start byte
void txData(char *buffer, int size) {
	unsigned char checksum = 0;

	radioSend(START_BYTE);
	for (int i = 0; i < size; i++) {
		radioSend(buffer[i]);
		wireSend(buffer[i]);
		checksum += buffer[i]; 
	}
	radioSend(checksum);
}


// RXDATA FUNCTION
// Receives size chars and places them into a buffer
// Compares the sum of the chars against the checksum
// Returns 1 if checksum match, 0 otherwise
int rxData(char *buffer, int size) {
	unsigned char checksum = 0;
	rxRadioFlag = 0;

	for (int i = 0; i < size; i++) {
		while(!rxRadioFlag);
		buffer[i] = radioReceived;
		checksum += radioReceived;
		rxRadioFlag = 0;
	}

	while(!rxRadioFlag);
	rxRadioFlag = 0;
	if (checksum != radioReceived)
		return 0;
	else
		return 1;
}


// DATATOTERMINAL FUNCTION
// Prints the sensor data to terminal in a nice format
// Returns nothing
void dataToTerminal() {
	char buf[80];

	wireSendString("Got good data!\r\n");
	data[0] -= 60;
	data[0] = data[0]*100/91;			
	sprintf(buf, "ADC0    = %d%c\r\n", data[0], '%');
	wireSendString(buf);
	data[1] -= 60;
	data[1] = data[1]*100/91;
	sprintf(buf, "ADC1    = %d%c\r\n", data[1], '%');
	wireSendString(buf);
	data[2] = data[2]*100/255;
	sprintf(buf, "ADC2    = %d%c\r\n", data[2], '%');
	wireSendString(buf);
	sprintf(buf, "Switch0 = %s\r\n", (data[3])?"Open":"Closed");
	wireSendString(buf);
	sprintf(buf, "Switch1 = %s\r\n\n", (data[4])?"Open":"Closed");
	wireSendString(buf);
}


