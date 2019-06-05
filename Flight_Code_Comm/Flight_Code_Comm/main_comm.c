/*
 * Flight_Code_Comm.c
 *
 * Created: 05-06-2019 11:35:49
 * Author : PRASHANT KURREY
 */ 

#include <avr/io.h>
#include "usart.h"
#include "common.h"



/* Global variables */
//TWI_Master_t twiMaster;           /*!< TWI master module. */
USART_data_t USART_datac0;
uint8_t usartc0_receive=0x01;
uint8_t op_mode=PREFLIGHT_MODE;       //next state of oprational mode


bool op_mode_check_beacon(uint8_t data);
bool op_mode_check_downlink(uint8_t data);
bool op_mode_check_uplink(uint8_t data);

int main(void)
{
	USART_ENABLE_C();
	PORTF.DIRSET=0xFF;
	//PORTF.OUTSET=0x0F;
	
	/* Global interrupt*/
	PMIC.CTRL |= PMIC_MEDLVLEN_bm|PMIC_LOLVLEN_bm;
	sei();
    /* Replace with your application code */
    while (1) 
    {
		
		UART_TXBuffer_PutByte(&USART_datac0, 'A');	
		_delay_ms(100);	
		
		
		for (int i=0; (i<=10) & op_mode_check_uplink(op_mode) ; i++ )
		{
			PORTF.OUT=0x03;//do something
			_delay_ms(100);
			PORTF.OUT=0x00;
			_delay_ms(100);
		};
		for (int i=0; (i<=10) & op_mode_check_downlink(op_mode) ; i++ )
		{
			PORTF.OUT=0x0C;//do something
			_delay_ms(100);
			PORTF.OUT=0x00;
			_delay_ms(100);
			          //do something
		};
		for (int i=0; (i<=10) & op_mode_check_beacon(op_mode) ; i++ )
		{
			PORTF.OUT=0xF0;//do something
			_delay_ms(100);
			PORTF.OUT=0x00;
			_delay_ms(100);//do something
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
	if (USART_RXBufferData_Available(&USART_datac0)) 
	{                                               // modified by  me
		op_mode = USART_RXBuffer_GetByte(&USART_datac0); // receive the data      // modified
	}                  
	UART_TXBuffer_PutByte(&USART_datac0, op_mode);	                     // send data 
	
}
/*! \brief Data register empty  interrupt service routine.
 *
 *  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */
ISR(USARTC0_DRE_vect)
{
	USART_DataRegEmpty(&USART_datac0);
}