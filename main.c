#define MAIN
#include "defs.h"
#include "adc.h"
#include "serial.h"
//#include "timer.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// Function Prototypes
void getData();
void processButton();
void txData();
void timerInit();
void dataToTerminal();
int rxData();
int roboteqInit();

// SYSINIT FUNCTION
// Sets port data directions and initializes systems
void sysInit() {
	DDRA = 0x15;		// A0,2,4 as output
	PORTA = 0xC0;		// enable A6-7's pull-up resistors
	DDRB = 0x00;		// B0-7 as input
	PORTB = 0xFF;		// Enable B's pull-up resistors
	DDRC = 0x0F;		// Set PortC 0-3 as output
	PORTC = 0xF1;		// Enable PortC's pull-up resistors
	DDRD  = 0x80;   	// D3 as output
	PORTD |= 0x80;  	// Turn on the power LED

	rcvrFlag = 0;
	configFlag = 0;
	rxRadioFlag = 0;
	rxWireFlag = 0;
	startDataFlag = 0;
	targetDevice = TERMINAL;
	tenMS_Timer = INTS_PER_10MS;
	secondTimer = INTS_PER_SECOND;

	adcInit();
	wireInit();
	radioInit();
	timerInit();
	wireSendString("Starting\r\n");
	sei();
}

// MAIN FUNCTION
int main() {
	sysInit();

	// TRANSMITTER MAIN PROGRAM LOOP
	#ifdef TRANSMITTER 
	while(1) {
		// TODO: This happens every time through the loop, too much!
		// Replace with timer trigger
		getData(); // get 6 adc values and stick them in the buffer

		// Ready command from receiver?
		if (rcvrFlag == 1) { 
			txData();  // transmit the data buffer
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
	PORTC |= 0x02;
	while(1) {
		// am I ready to receive data?
		if (rcvrFlag == 1) {
			if (targetDevice == TERMINAL) {
				wireSendString("Requesting data...\r\n");
			}
			radioSend(RCVR_READY);
			rcvrFlag = 0;
		}

		// start byte from transmitter?
		if (startDataFlag == 1) {
			// populate the buffer
			if (rxData()) {
				if (targetDevice == TERMINAL) {
					dataToTerminal();
				}
				else if (targetDevice == ROBOTEQ) {
					//dataToRoboteq();  // IMPLEMENT THIS!
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
	}
	#endif

	return 0;
}

// PROCESSBUTTON FUNCTION
// Does something when button is pressed, and debounces
void processButton() {	
	_delay_ms(10);
	PORTC ^= 0x04;
	rcvrFlag = 1;
	while(BUTTON_PRESSED);
	_delay_ms(10);
}

// GETDATA
// Gets 3 ADC readings, 2 digital readings, and populates the buffer
void getData() {
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
// Sends start byte, 6 unsigned chars, and a checksum of the added chars
void txData() {
	unsigned char checksum = 0;

	radioSend(START_BYTE);
	for (int i = 0; i < NUM_ADC_CHANS + NUM_DIGITAL_CHANS; i++) {
		radioSend(data[i]);
		wireSend(data[i]);
		checksum += data[i]; 
	}
	radioSend(checksum);
}

// RXDATA FUNCTION
// Receives 6 chars and places them into a buffer
// Compares the sum of the chars against the checksum
// Returns 1 if checksum match, 0 otherwise
int rxData() {
	unsigned char checksum = 0;
	rxRadioFlag = 0;

	for (int i = 0; i < NUM_ADC_CHANS + NUM_DIGITAL_CHANS; i++) {
		while(!rxRadioFlag);
		data[i] = received;
		checksum += received;
		rxRadioFlag = 0;
	}

	while(!rxRadioFlag);
	rxRadioFlag = 0;
	if (checksum != received)
		return 0;
	else
		return 1;
}

// Generates timer interrupt 122 times per second
void timerInit() {
	TCCR0B=(1<<CS02);
	TIMSK0 |=(1<<TOIE0);
}

// Timer system interrupt, flips flags
ISR(TIMER0_OVF_vect) {
	tenMS_Timer--;
	if(!tenMS_Timer) {  // Process 10ms timer events
		if (!rcvrFlag)
			rcvrFlag = 1;

		secondTimer--;
		if(!secondTimer) {  // Process 1s timer events
			secondTimer = INTS_PER_SECOND;

			if(!cal_time)
				cal_time--;
		}

		tenMS_Timer = INTS_PER_10MS;
	}
}

// Wire TTL Rx data interrupt
ISR(USART0_RX_vect) { 
    rxWireFlag = 1;
	received = UDR0;
	if (received == RCVR_READY) 
		rcvrFlag = 1;
	else if (received == START_BYTE)
		startDataFlag = 1;
	else if (received == CONFIG_CMD)
		configFlag = 1;
}

// Radio TTL Rx data interrupt
ISR(USART1_RX_vect) { 
    rxRadioFlag = 1;
	received = UDR1;
	if (received == RCVR_READY) 
		rcvrFlag = 1;
	else if (received == START_BYTE)
		startDataFlag = 1;
	else if (received == CONFIG_CMD)
		configFlag = 1;
}

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

int roboteqInit() {
	// query roboteq for version
		// start timer
		// if responded, move along
		return 0; // timer expired
	// set to non-verbose mode
		// start timer
		// check for ++
		// if ++, return 1
	return 0; // timer expired
}