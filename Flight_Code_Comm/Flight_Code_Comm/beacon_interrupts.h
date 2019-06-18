/*
 * beaconInterrupts.h
 *
 * Created: 26-05-2019 20:07:43
 *  Author: Rohan Singh
 */ 


#ifndef BEACON_INTERRUPTS_H_
#define BEACON_INTERRUPTS_H_
#include "avr_compiler.h"
#define DOT_TIME 3000
#define DASH_TIME 9000
#define WORD_TIME 12000
// The length of a dot is 1 time unit:DOT
// A dash is 3 time units:DASH
// The space between symbols (dots and dashes) of the same letter is 1 time unit:DOT
// The space between letters is 3 time units:DASH
// The space between words is 7 time units:DASH+WORD
#define BEACON_LENGTH 800 //Maximum number of HM Data operations required

void beaconInit();
void generateBeacon(char input[]);

#endif /* BEACON_INTERRUPTS_H_ */