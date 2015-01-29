/*********************************************
Title: ADC Library
Author: Jonathan Wang

Description:
	Basic functions for ADC use
*********************************************/
#include <avr/io.h> 
#include "adc.h"
#include "LCD.h"

void adcInit() 
{
	ADCSRA  = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);  // Configure ADC
}

int getADC(int chan) 
{
	ADMUX = chan;			// Use C0 for ADC reading
	ADCSRA  |= (1<<ADSC);  		// Start conversion
	while (!(ADCSRA & (1<<ADIF)));  // Wait for completion
	return ADCW;			// Store ADC result
}

int getAVG(int n)
{
	int i, sum;
	for (i = 0; i < n; i++)
		sum += getADC(0);

	return sum/n;
}

int avgADC(int avg, int x) 
{
	return (avg*(x-1) + getADC(0))/x;
}

// Prints the ADC value as a int value
void print_ADC(int val)
{
	int i = 0;
	while(i<4)
	{
		Command(0xCF-i);
		Print('0' + val%10);
		i++;
		val /= 10;
	}
}
