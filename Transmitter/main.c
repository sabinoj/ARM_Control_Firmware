/*********************************************
Title: Main
Author: Jonathan Wang

Description:
	Getting familiar with Atmega8 again
	Get buttons to repsond to certain LEDs
*********************************************/
#define F_CPU 8000000UL

#include <avr/io.h>	// this contains all the IO port definitions
#include <util/delay.h>
#include <avr/interrupt.h>
#include "LED_Button.h"
#include "LCD.h"
#include "adc.h"

// Global Variables
int x = 10;		// Used for average ADC
int avg;		// Used for average ADC
char statement1[] = "Option 1     \0";
char statement2[] = "Option 2     \0";
char statement3[] = "None selected\0";
char statement4[] = "Avg ADC = \0";

int main()
{
	cli();
	// Pin set up
	DDRB  = 0xFF;	// set PORTB to output
	PORTB = 0x00;	// Pins in PORTB are off
	DDRD  = 0x0F;	// set PORTD to output
	PORTD = 0x00;	// Pins in PORTD are off
	//************************************************************
	// USART set up
	UCSRA |= (1<<U2X);			// Set U2X to 1
	UCSRB |= (1<<TXEN) | (1<<RXEN);		// Enable TX and RX
	UCSRC |= (1<<UCSZ0) | (1<<UCSZ1);	// Set to 8-bit words
	UBRRH = 0;				// Set Baud rate value (High byte)
	UBRRL = 8;				// Set Baud rate value (Low byte)
	//************************************************************
	// Interupt Set up
	TCCR0 |= (1<<CS02) | (1<<CS00);		// Prescaler set
	TIMSK |= (1<<TOIE0);			// Enable overflow interrupt of clock 0
	
	//************************************************************
	
	adcInit();
	InitLCD();		
	avg = getAVG(x);
	sei();

	while(1)	
	{
		// Cutecom stuff
		while(!(UCSRA & (1<<UDRE)));
		UDR = 0x63;

		avg = avgADC(avg, x);
	}
}

// Timer Interrupt
// Refresh LCD about 30 times a second
ISR(TIMER0_OVF_vect)
{
	Command(0xC0);
	Print_to_LCD(statement4);
	print_ADC(avg);
	PORTD = get_Button();
	Command(0x80);
	if (PORTD == 0x01)	Print_to_LCD(statement1);
	else if (PORTD == 0x02)	Print_to_LCD(statement2);
	else			Print_to_LCD(statement3);
}

// USART Interrupt
ISR(USART_RXC_vect)
{

}
