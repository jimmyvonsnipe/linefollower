#ifndef LCD_H
#define LCD_H

#include <avr/io.h>          // register definitions
#include <stdlib.h>          // register definitions
#include <util/delay.h>      // delay functions
#include <string.h>      // delay functions


#define LCD_RS 1 << 0
#define LCD_RW 1 << 1
#define LCD_E 1 << 2

#define PORT_CTRL PORTB
#define PORT_DATA PORTD

#define WRITE_DELAY 4

void lcdInit();
void lcdDefaults();
void lcdClear();
void lcdDraw();

void lcdPutString(uint8_t c, uint8_t r, char* str);


void pulse_e();
void writeDisplay(uint8_t data);
void sendData(uint8_t data);
void sendCommand(uint8_t data);

void sendStr(char* str);




#endif