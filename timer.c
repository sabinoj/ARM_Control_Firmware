#include "defs.h"

// Generates timer interrupt 122 times per second
void timerInit() {
	TCCR0B=(1<<CS02);
	TIMSK0 |=(1<<TOIE0);
}