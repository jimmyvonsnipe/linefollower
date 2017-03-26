#include "main.h"

const float POSITIONS[] = {POS_LIN1, POS_LIN2, POS_LIN3, POS_LIN4, POS_LIN5};
const uint8_t MUXES[] = {MUX_LIN1, MUX_LIN2, MUX_LIN3, MUX_LIN4, MUX_LIN5};
volatile uint16_t readings[] = {0, 0, 0, 0, 0}; //10 bit output from the ADC
uint8_t curMux = 0;
uint16_t minLine = 1023;
uint16_t maxLine = 0;



int main() {
	DDRF = 0;
	PORTF = 0;
	lcdInit();
	lcdDefaults();
	lcdPutString(0,0,"Hello World");
	lcdDraw();
	setupADC(); //start reading line sensor values
	while(1) {
		char* str = "XXXX";
		lcdClearBuffer();
		for (int i = 0; i < 5; i++) {
			uint16_t reading = readings[i];
			itoa(reading, str, 10); //base 10 decimal
			uint8_t row = i > 2 ? 1 : 0;
			uint8_t col = i > 2 ? (i - 3) * 5 : i * 5;
			lcdPutString(col, row, str);
		}
		float col = getCoL();
		uint16_t pos = round(col * 1023);
		itoa(pos, str, 10);
		lcdPutString(10, 1, str);
		if (col < 0.4)	lcdPutString(14, 1, "<-");
		else if (col > 0.6)	lcdPutString(14, 1, "->");
		else	lcdPutString(14, 1, "||");

		lcdDraw();
		_delay_ms(200);
	}
	return 1;
}

void setupADC() {
	ADMUX = _BV(REFS0);
	setMux(0);
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	sei();
	ADCSRA |= _BV(ADSC);
}

void setMux(int mux) {
	ADMUX = (ADMUX & 0b11100000) | (MUXES[mux] & 0b00011111);
	ADCSRB = (ADCSRB & ~(_BV(MUX5))) | (MUXES[mux] & _BV(MUX5));
}

ISR(ADC_vect) {
	uint16_t reading = ADC;
	readings[curMux] = reading;
	if (reading < minLine) minLine = reading;
	if (reading > maxLine) maxLine = reading;
	
	curMux = (curMux + 1) % ADC_NUMBER;
	setMux(curMux);
	ADCSRA |= _BV(ADSC); //start new conversion
}

float getCoL() {
	float massDist = 0;
	float mass = 0;
	float spreadMin = 1;
	float spreadMax = 0;
	uint16_t range = maxLine - minLine;
	
	for (uint8_t i = 0; i < ADC_NUMBER; i++) {
		float fltReading = (readings[i] - minLine) / ((float)range);
		fltReading = fltReading == 1 ? 0.01 : 1 - fltReading;
		//uint16_t reading = readings[i] < 1023 ? (1023 - readings[i]) : 1;
		//float fltReading =  reading / 1023.0;
		mass += fltReading;
		massDist += fltReading * POSITIONS[i];
	}
	return (massDist / mass);
}

void setupPins() {
	//outputs for motor driver
	DDRB |= _BV(B_MOT1) | _BV(B_MOT2) | _BV(B_MOT4); 
	DDRD |= _BV(B_MOT3);
	//output for board LED
	DDRD |= _BV(5);
}