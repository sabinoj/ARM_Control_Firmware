/*********************************************
Title: LCD Library
Author: Jonathan Wang

Description:
	Basic functions the LCD uses
*********************************************/
#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"
/*****************************************/
// Send command to LCD
// Modified to send command to LCD instead of data
void Command(char a) 
{
	SPDR = a;		// Output command via SPI to SIPO
	while(!(SPSR & 0x80));  // Wait for SPI Flag
	a = SPDR;		// Equate a with SPIDR
	asm("NOP");		// Wait for 1 cycle
	PORTB &= ~(0x04);   	// Pulse RCK = $04
	asm("NOP");
	asm("NOP");
	PORTB |= 0x04;	        // Data Ready for LCD
	PORTB &= ~(0x01);	// RS = $01
	asm("NOP");
	asm("NOP");
	asm("NOP");
	PORTB |= 0x02;  	// Fire ENABLE = $02
	asm("NOP");
	asm("NOP");
	PORTB &= ~(0x02);  	// ENABLE off
	_delay_ms(2);
}
/*****************************************/
// Print character to LCD
void Print(char a) 
{
	SPDR = a;		// Output command via SPI to SIPO
	while(!(SPSR & 0x80)); 	// Wait for SPI Flag
	a = SPDR;		// Equate a with SPIDR
	asm("NOP");		// Wait for 1 cycle
	PORTB &= ~(0x04);   	// Pulse RCK = $04
	asm("NOP");
	asm("NOP");
	PORTB |= 0x04;	        // Data Ready for LCD
	PORTB |= 0x01;		// RS = $01
	asm("NOP");
	asm("NOP");
	asm("NOP");
	PORTB |= 0x02;		// Fire ENABLE = $02
	asm("NOP");
	asm("NOP");
	PORTB &= ~(0x02);   	// ENABLE off
	_delay_ms(2);
}
/*****************************************/
// Initalize the LCD
void InitLCD(void)
{
	SPCR = 0x50;
	_delay_ms(1);
	Command(0x30);
	_delay_ms(1);
	Command(0x30);
	_delay_ms(1);
	Command(0x30);
	_delay_ms(1);
	Command(0x38);
	_delay_ms(1);
	Command(0x0C);
	_delay_ms(1);
	Command(0x01);
	_delay_ms(2);
}


/*****************************************/
// Prints a string on the LCD
void Print_to_LCD(char word[])
{
	int i = 0;
	while (word[i])		Print(word[i++]);
}
