// uC definitions
#define F_CPU 8000000UL	 // set the CPU clock
#include <avr/io.h>

// Module Definition
#define TRANSMITTER
//#define RECEIVER

// Flags and comands
#define DELAY 10
#define BUTTON_PRESSED !(PINC & 0x10)  // Change to specific button
#define	START_BYTE 0xDB  // Sensor Data start command
#define RCVR_READY 0x4D  // Receiver ready command
#define CONFIG_CMD 0xB3  // Command for receiver to enter config mode
#define NUM_READINGS 3  // Number of ADC readings to be averaged
#define INTS_PER_10MS 12  // Interrupts per 10ms
#define INTS_PER_SECOND 100  // 10ms * 100 = 1s
#define NUM_ADC_CHANS 3  // Number of attached ADC sensors
#define NUM_DIGITAL_CHANS 2  // Number of attached digital sensors
#define DEBOUNCE_COUNT 100  // Counter for switch debouncing

// Shared Variables
#ifdef MAIN
	volatile unsigned char rxWireFlag;  // Data ready in wired USART RX buffer
	volatile unsigned char rxRadioFlag;  // Data ready in wireless USART RX buffer
	volatile unsigned char received;  // Data from USART RX
	volatile unsigned char rcvrFlag;  // Receiver is ready for sensor data
	volatile unsigned char configFlag;  // Received command to enter config mode
	volatile unsigned char startDataFlag;  // About to receive sensor data
	unsigned char data[NUM_ADC_CHANS + NUM_DIGITAL_CHANS];  // Buffer with sensor data
	unsigned char dataMin[NUM_ADC_CHANS];
	unsigned char dataMax[NUM_ADC_CHANS];
	unsigned int tenMS_Timer;
	unsigned int secondTimer;
	unsigned int cal_time;
#else
	extern volatile unsigned char rxWireFlag;  // Data ready in wired USART RX buffer
	extern volatile unsigned char rxRadioFlag;  // Data ready in wireless USART RX buffer
	extern volatile unsigned char received;  // Data from USART RX
	extern volatile unsigned char rcvrFlag;  // Receiver is ready for sensor data
	extern volatile unsigned char configFlag;  // Received command to enter config mode
	extern volatile unsigned char startDataFlag;  // About to receive sensor data
	extern unsigned char data[NUM_ADC_CHANS + NUM_DIGITAL_CHANS];
	extern unsigned int tenMS_Timer;
	extern unsigned int secondTimer;
	extern unsigned int cal_time;
#endif