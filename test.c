/*
 * Author:  AVR Tutoruals
 * Website: www.AVR-Tutorials.com
 *
 * Written in AVR Studio 5
 * Compiler: AVR GNU C Compiler (GCC)
 */ 
 
#include <avr/io.h>
#include <avr/interrupt.h>
 
/*ADC Conversion Complete Interrupt Service Routine (ISR)*/
ISR(ADC_vect)
{
	PORTE |= 1 << 6;
	ADCSRA |= 1<<ADSC;		// Start Conversion
}
 
int main(void)
{
	DDRE = 1 << 6;
	DDRB = 1 << 0;
	
	//DDRD = 0xFF;			// Configure PortD as output
	//DDRA = 0x00;			// Configure PortA as input
					// PA0 is ADC0 input
 
	ADCSRA = 0x8F;			// Enable the ADC and its interrupt feature
					// and set the ACD clock pre-scalar to clk/128
	ADMUX = 0xE0;			// Select internal 2.56V as Vref, left justify 
					// data registers and select ADC0 as input channel 
 
	sei();				// Enable Global Interrupts
	ADCSRA |= 1<<ADSC;		// Start Conversion
 
	while(1) {
		if (ADCH > 128) PORTB = 1 << 0;
		else PORTB = 0;
	
	}	
} 