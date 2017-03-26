#ifndef PINS_H
#define PINS_H

//motor driver outputs
#define B_MOT1 5
#define PORT_MOT1 PORTB
#define B_MOT2 6
#define PORT_MOT2 PORTB
#define B_MOT4 7
#define PORT_MOT4 PORTB 
#define B_MOT3 0
#define PORT_MOT3 PORTD

#define MUX_LIN1 0b000000 //adc0
#define MUX_LIN2 0b000001 //adc1
#define MUX_LIN3 0b000100 //adc4
#define MUX_LIN4 0b000101 //adc5
#define MUX_LIN5 0b000110 //adc6
// #define MUX_LIN6 0b000111 //adc7
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