#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/interrupt.h>   // interrupt vectors
#include <util/delay.h>      // delay functions

#include <stdint.h>          // C header for int types like uint8_t
#include <stdbool.h>    
#include <stdio.h>      
#include <math.h>     

#include "pins.h"
#include "lcd.h"

//const float POSITIONS[];
const uint8_t MUXES[];
uint8_t curMux;
volatile uint16_t readings[]; 

void setMotorOut(uint8_t motor, uint8_t val);
void setupPWM();

void setupADC();
void setMux(int mux);
// float getCoL();
uint16_t getCoL();
void setupPins();
int main();

#endif