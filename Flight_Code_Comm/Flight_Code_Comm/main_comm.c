/*
 * Flight_Code_Comm.c
 *
 * Created: 17-06-2019 19:43:12
 * Author : Puneet Shrivas
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "TC_driver.h"
#include "avr_compiler.h"
#include "PD90.h"
#include "usart.h"
#include "common.h"
#include "beacon_interrupts.h"

/* Global variables */
USART_data_t USART_datac0;
uint8_t usartc0_receive=0x01;
uint8_t op_mode=PREFLIGHT_MODE;       //next state of operational mode
char * beacon_string;

bool op_mode_check_beacon(uint8_t data);
bool op_mode_check_downlink(uint8_t data);
bool op_mode_check_uplink(uint8_t data);

int main(void)
{
	setUp16MhzExternalOsc();
	SSTVinit();
	USART0_INIT(&PORTC,&USART_datac0,&USARTC0); 
	PORTF.DIRSET=0xFF;
	/* Global interrupt*/
	PMIC.CTRL |= PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
	
	/*Testing initializations*/
	op_mode=NOM_TRANSMIT_MODE;
	beacon_string="PRASHANT";
		
    while (1)
    {
		if(op_mode_check_uplink(op_mode))
		{
			PORTF.OUT=0x03;//do something
			_delay_ms(100);
			PORTF.OUT=0x00;
		}
		if(op_mode_check_beacon(op_mode))
		{
			PORTF.OUT=0xF0;//Transmit Beacon (Interrupt pin PORTF PIN2)
			generateBeacon(beacon_string);
			PORTF.OUT=0x00;
			PORTD_OUTCLR=PIN2_bm;
			_delay_ms(1000);//do something
		}
		if(op_mode_check_downlink(op_mode))
		{
			PORTF.OUT=0x0C;//Transmit SSTV
			SSTVbegin();
			PORTF.OUT=0x00;
			_delay_ms(100);
			          //do something
		}
    }
}


bool op_mode_check_beacon(uint8_t data)
{
	if ( (data == NOM_IDLE_MODE)| (data == NOM_TRANSMIT_MODE) | (data == NOM_UPLINK_MODE) | (data==LOW_POWER_MODE))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool op_mode_check_downlink(uint8_t data)
{
	if ( data == NOM_TRANSMIT_MODE )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool op_mode_check_uplink(uint8_t data)
{
	if ( (data == NOM_IDLE_MODE)| (data == NOM_TRANSMIT_MODE) | (data == NOM_UPLINK_MODE))
	{
		return true;
	}
	else
	{
		return false;
	}
}


ISR(USARTC0_RXC_vect)
{

	USART_RXComplete(&USART_datac0);
    if(USART_RXBufferData_Available(&USART_datac0))
	{                                               // modified by  me
		op_mode = USART_RXBuffer_GetByte(&USART_datac0); // receive the data      // modified
	}
	UART_TXBuffer_PutByte(&USART_datac0, op_mode);	                     // send data
}

ISR(USARTC0_DRE_vect)
{
	USART_DataRegEmpty(&USART_datac0);
}
