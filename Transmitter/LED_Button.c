/*********************************************
Title: LED/Button Library
Author: Jonathan Wang

Description:
	Get buttons to repsond to certain LEDs
	Currently assuming that 3 buttons will be used
	Modes:
	- Calibrate
	-
	-
*********************************************/
#include <avr/io.h> 
#include "LED_Button.h"

// Polls for a button push from user
// Returns 0 if no buttons are pushed.
int get_Button()
{
	if (PIND & 0x20)	return 0x01;	
	else if (PIND & 0x40)	return 0x02;
	else			return 0x00;
}
