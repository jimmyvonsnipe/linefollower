#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h> 
#include <util/delay.h>    
#include <stdint.h>       
#include <stdbool.h>    
#include <stdio.h>      
#include <stdlib.h>
#include <math.h>     


#include "pins.h"

volatile bool overflowed = 0; 


void startUART() {
	UBRR1H = 0;
	UBRR1L = 103;
	
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); //8 bits, no parity, one stop bit
	UCSR1B = _BV(TXEN1) | _BV(RXEN1); //enable TX and RX
}
void startTimer() {
	TIMSK4 = _BV(TOIE4);
	TCCR4B =  _BV(CS43) | _BV(CS41) | _BV(CS40);
	
}

ISR(TIMER4_OVF_vect) {
	overflowed = true;
}

int main() {
	
	DDRD &= ~(_BV(B_BTN0) | _BV(B_BTN1));
	
	PORT_BTN |= _BV(B_BTN0) | _BV(B_BTN1);
	DDRC |= _BV(6) | _BV(7);
	DDRD |= _BV(7);
	
	while (true) {
		startUART();
		startTimer();
		sei();
		while (true) {
			if (overflowed) {
				overflowed = false;
				while (!(UCSR1A & _BV(UDRE1)));
				UDR1 = 'A'; 
			}
		}
	}
	
	
	return 0;
	
}

