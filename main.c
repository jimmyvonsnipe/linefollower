#include "main.h"
// I was here

//const float POSITIONS[] = {POS_LIN1, POS_LIN2, POS_LIN3, POS_LIN4, POS_LIN5};
volatile bool calibrating = false;
const uint8_t MUXES[] = {MUX_LIN1, MUX_LIN2, MUX_LIN3, MUX_LIN4, MUX_LIN5};
volatile uint16_t readings[] = {0, 0, 0, 0, 0}; //10 bit output from the ADC
volatile uint32_t adjusted[] = {0, 0, 0, 0, 0}; //10 bit output from the ADC
volatile uint16_t maxes[] = {0, 0, 0, 0, 0}; //10 bit output from the ADC
volatile uint16_t mins[] = {1023, 1023, 1023, 1023, 1023}; //10 bit output from the ADC
const uint16_t centerValue = (ADC_NUMBER-1) * 500;
uint8_t curMux = 0;
uint16_t lastPos = (ADC_NUMBER-1)*500;
bool lostLine = false;

float iTerm = 0;
float lastError = 0;

int main() {
	setupPins();
	setupADC(); //start reading line sensor values
	setupPWM(); //setup PWM timers
	uint8_t val = 0;
	calibrating = true; 
	DDRD &= ~_BV(4); //jumper for straight ahead, set as input
	PORTD |= _BV(4); //pullup resistor
	
	//see if initial calibration is better than continuous calibration?
	// while (!(PINC & _BV(7))) {
		// PORTD |= _BV(5);
	// }
	// PORTD &= ~_BV(5);
	// calibrating = false;
	
	
	while (1) {
		
		// if (PINC & _BV(6)) {
			// PORT_MB1 |= _BV(B_MB1); // heats up
			// PORT_MA1 |= _BV(B_MA1); //heats up
		// } else {
			// PORT_MB1 &= ~_BV(B_MB1);
			// PORT_MA1 &= ~_BV(B_MA1);
		// }

		// if (PINC & _BV(7)) {
			// PORT_MB2 |= _BV(B_MB2); // M2 B2
			// PORT_MA2 |= _BV(B_MA2); //M1 A2
		// } else {
			// PORT_MA2 &= ~_BV(B_MA2);
			// PORT_MB2 &= ~_BV(B_MB2);
		// }
	
		if (PINC & _BV(6)) {
			val = val >= 255 ? 255 : val + 1;
		} 
		if (PINC & _BV(7)) {
			val = val <= 0 ? 0 : val - 1;
		}
		_delay_ms(10);
		
		//show the estimated line position on Top LEDs
		//if we lose the line, turn on the blue LED. 
		uint16_t pos = getCoL();
		if (pos < 1000) {
			if (lostLine) PORTD |= _BV(5);
			PORT_LED0 |= _BV(B_LED0);
			PORT_LED1 &= ~_BV(B_LED1);
			PORT_LED2 &= ~_BV(B_LED2);
			PORT_LED3 &= ~_BV(B_LED3);
		} else if (pos < 1500) {
			PORTD &= ~_BV(5);
			PORT_LED0 |= _BV(B_LED0);
			PORT_LED1 |= _BV(B_LED1);
			PORT_LED2 &= ~_BV(B_LED2);
			PORT_LED3 &= ~_BV(B_LED3);
		} else if (pos < 2500) {
			PORTD &= ~_BV(5);
			PORT_LED0 &= ~_BV(B_LED0);
			PORT_LED1 |= _BV(B_LED1);
			PORT_LED2 |= _BV(B_LED2);
			PORT_LED3 &= ~_BV(B_LED3);
		} else if (pos < 3000) {
			PORTD &= ~_BV(5);
			PORT_LED0 &= ~_BV(B_LED0);
			PORT_LED1 &= ~_BV(B_LED1);
			PORT_LED2 |= _BV(B_LED2);
			PORT_LED3 |= _BV(B_LED3);
		} else {
			if (lostLine) PORTD |= _BV(5);
			PORT_LED0 &= ~_BV(B_LED0);
			PORT_LED1 &= ~_BV(B_LED1);
			PORT_LED2 &= ~_BV(B_LED2);
			PORT_LED3 |= _BV(B_LED3);
		}
		
		// const float Kp = 2;
		// const float Ki = 0;
		// const float Kd = 0;
		const float Kp = 1.5;
		const float Ki = 0.05;
		const float Kd = 0.25;
		
		float error = (pos - 2000.0) / 2000.0;
		
		iTerm += error; 
		//stop integral windup if the bot gets stuck etc
		if (fabs(iTerm) > 1) { //turn this limiting into a function
			iTerm = iTerm < 0 ? -1 : 1;
		}
		
		float dTerm = lastError - error;
		lastError = error;
		
		float correction = (error * Kp) + (iTerm * Ki) + (dTerm * Kd);
		if (fabs(correction) > 1) {
			correction = correction < 0 ? -1 : 1;
		}	
		
		bool goLeft = correction < 0;
		correction = 1 - fabs(correction);
		uint16_t slowMotorVal = round(val * correction);
		slowMotorVal = slowMotorVal > val ? val : slowMotorVal;
		
		
		uint8_t leftMotorVal = goLeft ? slowMotorVal : val;
		uint8_t rightMotorVal = !goLeft ? slowMotorVal : val;
		
		if (PIND & _BV(4)) { //if jumper not connected
			setMotorOut(MB2, leftMotorVal);
			setMotorOut(MA2, rightMotorVal);
		} else {			
			setMotorOut(MB2, val);
			setMotorOut(MA2, val);
		}
		
	}
	
	//LCD for debuggin. 
	// lcdInit();
	// lcdDefaults();
	// lcdPutString(0,0,"Hello World");
	// lcdDraw();
	// while(1) {
		// char* str = "XXXX";
		// lcdClearBuffer();
		// for (int i = 0; i < 5; i++) {
			// uint16_t reading = readings[i];
			// itoa(reading, str, 10); //base 10 decimal
			// uint8_t row = i > 2 ? 1 : 0;
			// uint8_t col = i > 2 ? (i - 3) * 5 : i * 5;
			// lcdPutString(col, row, str);
		// }
		// uint16_t pos = getCoL();
		// itoa(pos, str, 10);
		// lcdPutString(10, 1, str);
		// if (pos < 1500)	lcdPutString(14, 1, "<-");
		// else if (pos > 2500) lcdPutString(14, 1, "->");
		// else lcdPutString(14, 1, "||");

		// lcdDraw();
		// _delay_ms(200);
	// }
	return 1;
}

void setMotorOut(uint8_t motor, uint8_t val) {
	switch (motor) {
		case MA1:
		break;
		case MA2:
		OCR1BL = val;
		break;
		case MB1:
		break;
		case MB2:
		OCR0A = val;
		break;
	}
}

void setupPWM() {
	OCR0A = 0; 
	TCCR0A = _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(CS01) | _BV(CS00);
	//enable timer 0 with fast PWM with a 1/64 prescaler
	//results in a pwm frequency of about 976Hz
	//play around see what the motors like
	
	OCR1B = 0;
	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10); 
	//clk/64 and only 8 bit fast PWM	
	//only 8 bits so we can use the same logic across both timers,
	//1024 bit precision is overkill with shit motors
}

void setupADC() {
	ADMUX = _BV(REFS0); //Internal Vcc reference with a cap at AREF
	setMux(curMux); //look at the first sensor
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	// enable ADC with interupts, cpu speed/128 is our ADC clock
	sei(); //enable interrupts globally
	ADCSRA |= _BV(ADSC); 
}

void setMux(int mux) { 
	//check datasheet for ADC Mux values. This changes the sensor that is being read. 
	ADMUX = (ADMUX & 0b11100000) | (MUXES[mux] & 0b00011111);
	ADCSRB = (ADCSRB & ~(_BV(MUX5))) | (MUXES[mux] & _BV(MUX5));
}


ISR(ADC_vect) {
	uint16_t reading = ADC;
	readings[curMux] = reading; //save our reading in the appropriate array bin
	if(calibrating) { //do the calibration here so we can use it in the main code
		if (reading < mins[curMux]) mins[curMux] = reading;
		if (reading > maxes[curMux]) maxes[curMux] = reading;
	}
	
	//focus on the next sensor, if at the last one, loop around
	curMux = (curMux + 1) % ADC_NUMBER;
	setMux(curMux);
	ADCSRA |= _BV(ADSC); //start new conversion
}

//wanted to reduce cycle count so getting rid of floats where practical
// float getCoL() {
	// float massDist = 0;
	// float mass = 0;
	// float spreadMin = 1;
	// float spreadMax = 0;
	// uint16_t range = maxLine - minLine;
	
	// for (uint8_t i = 0; i < ADC_NUMBER; i++) {
		// float fltReading = (readings[i] - minLine) / ((float)range);
		// fltReading = fltReading == 1 ? 0.01 : 1 - fltReading;
		// uint16_t reading = readings[i] < 1023 ? (1023 - readings[i]) : 1;
		// float fltReading =  reading / 1023.0;
		// mass += fltReading;
		// massDist += fltReading * POSITIONS[i];
	// }
	// return (massDist / mass);
// }

uint16_t getCoL() {
	uint32_t massDist = 0;
	uint32_t mass = 0;
	uint16_t spreadMin = 1000;
	uint16_t spreadMax = 0;
	
	//floats have been replaced with 1000 int precision
	//for each sensor
	for (uint8_t i = 0; i < ADC_NUMBER; i++) {
		
		//get the max and min expected values for each sensor (written in the interrupt)
		//the difference is the usable range of the sensor. 
		uint32_t range = maxes[i] - mins[i];
		if (range == 0) return centerValue; //divide by 0 avoidance
		
		//where in between the minimum and maximum sensor values is the latest reading?
		//express this as a proportion between 0 and 1000
		//ideally 1000 is the line and 0 is the black surface, ofc this is wishful thinking
		//the bot usually gets like 200 and 800 which is pretty good
		uint32_t adjReading = (((uint32_t)readings[i] - mins[i]) * 1000);
		adjReading = adjReading / (range);
		adjReading = adjReading == 0 ? 1 : adjReading;
		
		//calculating the minimum and maximum sensor values at this time for lost line situation
		if (adjReading < spreadMin) spreadMin = adjReading;
		if (adjReading > spreadMax) spreadMax = adjReading;

		adjusted[i] = adjReading; //not necessary array but useful for debugging
		
		//think of brightnesses as mass and each sensor as having a displacement
			//and you'll understand how i've abused a sorta-center-of-mass equation to find the line
		mass += adjReading;
		massDist += adjReading * (i * 1000);
	}
	
	//if the line was last left of center, then steer hard left if we lose the line, and vice versa
	uint16_t lastDir = lastPos < centerValue ? 0 : (ADC_NUMBER-1)*1000;
	// if there is very little variation in the data
		//and the overall field is relatively dark (doesn't trigger on cross intersections)
		//we've probably lost the line
	uint16_t valueSpread = spreadMax - spreadMin;
	const uint16_t LOST_THRESH = 300;
	const uint16_t FOUND_THRESH = 350;
	const uint16_t DARKNESS_THRESH = ((1000 * ADC_NUMBER) / 2); // in terms of sum of all readings
	
	if (valueSpread < LOST_THRESH && mass < DARKNESS_THRESH) { 
		lostLine = true;
	}  else if (valueSpread > FOUND_THRESH) { //some hysteresis to stop retarded twitching, commit to a direction. 
		lostLine = false;
	}
	
	if (lostLine) return lastDir;
	
	lastPos = (uint16_t)(massDist / mass);
	return lastPos;
}

void setupPins() {
	//outputs for motor driver
	DDRB |= _BV(B_MA1) | _BV(B_MA2) | _BV(B_MB2); 
	DDRD |= _BV(B_MB1);
	//outputs for board LEDs
	DDRE |= _BV(B_LED0);
	DDRB |= _BV(B_LED1) | _BV(B_LED2) | _BV(B_LED3);
	//output for onboard LED
	DDRD |= _BV(5);
	
}