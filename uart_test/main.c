#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h> 
#include <util/delay.h>    
#include <stdint.h>       
#include <stdbool.h>    
#include <stdio.h>      
#include <stdlib.h>
#include <math.h>     

#define BAUDSEL 103 //9600baud

volatile uint8_t encoderState = 0;

volatile int16_t encoderB = 0;
volatile int16_t encoderA = 0;

volatile bool overflowed = 0; 

void startUART() {
	UBRR1H = 0;
	UBRR1L = 103;
	
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); //8 bits, no parity, one stop bit
	UCSR1B = _BV(TXEN1) | _BV(RXEN1); //enable TX and RX
}

void startEncoding() {
	PCMSK0 = _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT2) | _BV(PCINT3);
	PCICR = _BV(PCIE0);
}

void startTimer() {
	TIMSK4 = _BV(TOIE4);
	TCCR4B =  _BV(CS43) | _BV(CS41) | _BV(CS40);
	
}


ISR(PCINT0_vect) {
	uint8_t changeMask = encoderState ^ PINB;
	uint8_t dirMask = encoderState ^ (PINB >> 1);
	
	bool changeB = changeMask & (_BV(0) | _BV(1));
	bool changeA = changeMask & (_BV(2) | _BV(3));
	
	//If both values changed in the encoder, we've lost something. 
	bool invalidB = (changeMask & _BV(0)) & (changeMask >> 1);
	bool invalidA = (changeMask & _BV(2)) & (changeMask >> 1);
	
	
	if (changeB && !invalidB) {
		encoderCount += dirMask & _BV(0) ? 1 : -1; 
	}
	if (changeB && !invalidB) {
		encoderCount += dirMask & _BV(2) ? 1 : -1; 
	}
	
	encoderState = PINB;
}

ISR(TIMER4_OVF_vect) {
	overflowed = true;
}

int main() {
	
	startUART();
	startInt();
	startTimer();
	sei();
	while (true) {
		if (overflowed) {
			overflowed = false;
			while (!(UCSR1A & _BV(UDRE1)));
			UDR1 = encoderCount; 
		}
	}
	
	
	return 0;
	
}

