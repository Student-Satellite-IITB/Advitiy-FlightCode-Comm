/*
 * beaconInterrupts.c
 *
 * Created: 26-05-2019 20:06:58
 *  Author: Rohan Singh
 */ 
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <string.h>
#include "pmic_driver.h"
#include "beacon_interrupts.h"

unsigned char morseOutput[BEACON_LENGTH]; //Array containing all the operations to be performed to get the given string as morseTable
uint8_t currentIndex=0; //The order of operation to be performed from morseOutput
uint8_t totalLength; //Size of the morseOutput array being used up
unsigned char morseTable[27][8]={{0,3,1,4,2,2,2,2},{1,3,0,3,0,3,0,4},{1,3,0,3,1,3,0,4},
{1,3,0,3,0,4,2,2},{0,4,2,2,2,2,2,2},{0,3,0,3,1,3,0,4},
{1,3,1,3,0,4,2,2},{0,3,0,3,0,3,0,4},{0,3,0,4,2,2,2,2},
{0,3,1,3,1,3,1,4},{1,3,0,3,1,4,2,2},{0,3,1,3,0,3,0,4},
{1,3,1,4,2,2,2,2},{1,3,0,4,2,2,2,2},{1,3,1,3,1,4,2,2},
{0,3,1,3,1,3,0,4},{1,3,1,3,0,3,1,4},{0,3,1,3,0,4,2,2},
{0,3,0,3,0,4,2,2},{1,4,2,2,2,2,2,2},{0,3,0,3,1,4,2,2},
{0,3,0,3,0,3,1,4},{0,3,1,3,1,4,2,2},{1,3,0,3,0,3,1,4},
{1,3,0,3,1,3,1,4},{1,3,1,3,0,3,0,4},{2,2,2,2,2,2,2,5}};
/*26 Alphabets + WORD space	*/

// 0: Dot
// 1: Dash
// 2: Nothing
// 3: Intra-Letter Gap
// 4: Inter-Letter Gap
// 5: WORD Gap


void generateBeacon(char input[])
{
	uint8_t inputLength= strlen(input); //Length of string input
	currentIndex = 0;
	uint8_t integerInput[inputLength]; //Array containing integer values corresponding to each character from input string

	for(int j=0;j<inputLength;j++)
	{
		if(((int)input[j])==32) //Condition for [space]
		{integerInput[j]= 26;}
		else                    //Condition for other characters
		{integerInput[j]= input[j]-'A';}
	}

	totalLength=8*inputLength;
	for(int m=0; m<BEACON_LENGTH; m++) //Convert each value in morseOutput array to '2'(action corresponding to do nothing) to avoid garbage values in the unused space
	{
		morseOutput[m]=2;
	}

	for (int k=0;k<inputLength;k++)
	{for(int l=0;l<8;l++) //Concatenating each individual character operations to make the all-inclusive morseOutput array
		{
			morseOutput[(8*k)+l]=morseTable[integerInput[k]][l];
		}
	}
		PORTD_DIRSET=PIN2_bm;
		PORTD_OUTCLR=PIN2_bm;
		PMIC_EnableMediumLevel();				//Enable interrupts : Low level for timer
		TCC0.CTRLA = TC_CLKSEL_DIV64_gc;		//Set Prescaler 1(Same as CPU_PRESCALER)
		TCC0.CTRLB= TC_WGMODE_NORMAL_gc;    //Wave generation mode : Normal
		TCC0.INTCTRLA = TC_OVFINTLVL_MED_gc;	//Enable overflow interrupt
		TCC0.PER=100;		    //Initialize Period
		TCC0.CNT=0;
		sei(); //Set interrupt
		_delay_ms(1000);
}

ISR(TCC0_OVF_vect) //Pin for interrupt: Port F- Pin 1
{
	if(morseOutput[currentIndex]==0)
	{   TCC0.PER = DOT_TIME;
		PORTD_OUTSET=PIN2_bm;
	}
	
	else if(morseOutput[currentIndex]==1)
	{   TCC0.PER = DASH_TIME;
		PORTD_OUTSET=PIN2_bm;
	}
	
	else if(morseOutput[currentIndex]==3)
	{   TCC0.PER = DOT_TIME;
		PORTD_OUTCLR=PIN2_bm;
	}
	
	else if(morseOutput[currentIndex]==4)
	{   TCC0.PER = DASH_TIME;
		PORTD_OUTCLR=PIN2_bm;
	}
	
	else if(morseOutput[currentIndex]==5)
	{	TCC0.PER = WORD_TIME;
		PORTD_OUTCLR=PIN2_bm;
	}
	
	else if(morseOutput[currentIndex]==2)
	{
		;
	}
	currentIndex++;
	
	if(currentIndex==totalLength) //close the ISR
	{   TCC0.INTCTRLA = TC_OVFINTLVL_OFF_gc;
		cli();
	}
}

