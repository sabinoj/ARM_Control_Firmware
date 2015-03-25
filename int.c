#include <avr/io.h>
#include <avr/interrupt.h>

int intInit(void) {
    DDRD &= ~(1 << DDD2);     // Clear pin
    PORTD |= (1 << PORTD2);    // turn On the Pull-up
    
    MCUCR |= (1 << ISC10);    // set INT0 to trigger on falling edge
    GICR |= (1 << INT0);      // Turns on INT0
}



ISR (INT0_vect) {
    PORTD ^= 0x40;
}