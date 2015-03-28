#include "defs.h"
#include <string.h>

void wireInit() {
	#define BAUD 115200
	#include <util/setbaud.h>
	UBRR0H=UBRRH_VALUE;
	UBRR0L=UBRRL_VALUE;  //set baud rate
	UCSR0B = (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0);  //enable receiver and transmitter
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);  // 8bit data format
	#if USE_2X
		UCSR0A |= (1 << U2X0);
	#else
		UCSR0A &= ~(1 << U2X0);
	#endif
}

void wireSend(char ch) {
		while (!( UCSR0A & (1<<UDRE0)));  // wait while register is free
		UDR0 = ch;
}

void wireSendString(char *str) {
	while(*str)
		wireSend(*str++);
}

void radioInit() {
	//#undef BAUD
	//#define BAUD 38400
	//#include <util/setbaud.h>
	UBRR1H=UBRRH_VALUE;
	UBRR1L=UBRRL_VALUE;  //set baud rate
	UCSR1B = (1<<TXEN1)|(1<<RXEN1)|(1<<RXCIE1);  //enable receiver and transmitter
	UCSR1C = (1<<UCSZ10)|(1<<UCSZ11);  // 8bit data format
	#if USE_2X
		UCSR1A |= (1 << U2X1);
	#else
		UCSR1A &= ~(1 << U2X1);
	#endif
}

void radioSend(char ch) {
		while (!( UCSR1A & (1<<UDRE1)));  // wait while register is free
		UDR1 = ch;
}

void radioSendString(char *str) {
	while(*str)
		radioSend(*str++);
}

int wireGetCmpString(volatile unsigned int *timer, char str[]) {
	char robo[100] = "";
	int counter = 0;

	while (*timer) {
		if (rxWireFlag) {
			rxWireFlag = 0;
			if (wireReceived == '\r' || wireReceived == '\n') {
				robo[counter] == 0x00;
				if(!strcmp(robo, str)) {
					return 1;
				}
				else {
					return -1;
				}
			}
			else robo[counter++] = wireReceived;
		}
	}
	return 0;
}

