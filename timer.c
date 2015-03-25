#include "defs.h"

// Generates timer interrupt 122 times per second
void timerInit() {
	TCCR0B=(1<<CS02);
	TIMSK0 |=(1<<TOIE0);
}

// Timer system interrupt, pulses PORTC1
ISR(TIMER0_OVF_vect) {
	secondTimer--;
	if(!secondTimer) {
		if (!rcvrFlag)
			rcvrFlag = 1;
		secondTimer = INTS_PER_SEC;
	}
}