#include "lcd.h"

char* top;
char* bot;

void pulse_e() { //commit data on the parallel pins to the LCDs memory
	PORTB |= LCD_E;
	PORTB &= ~(LCD_E);
}

void writeDisplay(uint8_t data) {
	//set write mode 
	//set R/W pin LOW
	PORTB &= ~(LCD_RW);
	PORTB &= ~(LCD_E);
	//send MS 4 bits
	PORTD &= 0xE1;
	PORTD |= (data >> 3) & 0x1E;
	//confirm
	pulse_e();
	//send LS 4 bits;
	PORTD &= 0xE1;
	PORTD |= (data << 1) & 0x1E;
	//confirm
	pulse_e();
	_delay_ms(WRITE_DELAY); //delay is very large
}

void sendData(uint8_t data) {
	PORTB |= LCD_RS;
	writeDisplay(data);
	PORTB &= ~(LCD_RS);
}

void sendCommand(uint8_t data) {
	PORTB &= ~(LCD_RS);
	writeDisplay(data);
}

void sendStr(char* str) {
	while (*str != 0x00) {
		sendData(*str++);
	}
}

void lcdInit() {
	DDRB |= LCD_RS | LCD_E | LCD_RW;
	DDRD |= 0x1E;
	
	//enable 4 pin mode
	PORTD &= 0xE1;
	PORTD |= 0x04;
	pulse_e();
	_delay_ms(50);
	
	top = calloc(17, 1);
	bot = calloc(17, 1);
}

void lcdDefaults() {
	sendCommand(0b00001100);
	sendCommand(0b00101000);
	lcdClear();
}

void lcdClear() {
	sendCommand(0b00000001);
	memset(top, ' ', 16);
	memset(bot, ' ', 16);
}

void lcdDraw() {
	sendCommand(0b00000010);
	sendStr(top);
	sendCommand(0b11000000);
	sendStr(bot);
}

void lcdPutString(uint8_t c, uint8_t r, char* str) {
	if (!(c < 16 && r < 2)) return;
	char* row = r == 0 ? top : bot;
	
	row += c;
	while ((*str != 0x00) && (*row != 0x00)) {
		*(row++) = *(str++);
	}
}