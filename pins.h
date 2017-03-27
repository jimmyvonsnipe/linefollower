#ifndef PINS_H
#define PINS_H

#define B_LED0 6
#define PORT_LED0 PORTE
#define B_LED1 0
#define PORT_LED1 PORTB
#define B_LED2 1
#define PORT_LED2 PORTB
#define B_LED3 2
#define PORT_LED3 PORTB


//motor driver outputs
#define B_MA1 5
#define PORT_MA1 PORTB
#define B_MA2 6
#define PORT_MA2 PORTB
#define B_MB2 7
#define PORT_MB2 PORTB 
#define B_MB1 0
#define PORT_MB1 PORTD

#define MUX_LIN1 0b000100 //adc4  PF4
#define MUX_LIN2 0b000101 //adc5  PF5
#define MUX_LIN3 0b000110 //adc6  PF6
#define MUX_LIN4 0b000111 //adc7  PF7
#define MUX_LIN5 0b100011 //adc11 PB4
// #define MUX_LIN6 
// #define MUX_LIN7 0b100000 //adc8
// #define MUX_LIN8 0b100001 //adc9

// #define POS_LIN1 0
// #define POS_LIN2 1.0/4.0
// #define POS_LIN3 2.0/4.0
// #define POS_LIN4 3.0/4.0
// #define POS_LIN5 1
#define POS_LIN1 0
#define POS_LIN2 1000
#define POS_LIN3 2000
#define POS_LIN4 3000
#define POS_LIN5 4000

#endif