/********************************************************************
*																	*
*	Filename:		main.c											*
*	Originator:		Chris Lewis										*
*	Project:		Net Connector Test Software						*
*	Description:													*
*																	*
********************************************************************/
 
/*==================================================================*/
/*							INCLUDE FILES							*/
/*==================================================================*/

#include "ATMtypes.h"

#include "i2c.h"
#include "asci.h"
#include "menu.h"
#include "main.h"
#include "Timer.h"
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>  
#include <avr/interrupt.h>   

/*==================================================================*/
/*						LOCAL MACRO DEFINITIONS						*/
/*==================================================================*/
/*==================================================================*/
/*						LOCAL CONSTANT DEFINITIONS					*/
/*==================================================================*/
	
/*==================================================================*/
/*		LOCAL INITIALISED VARIABLES (initialised to 0 by default)	*/
/*==================================================================*/
int8 tmpstr[TMPSTR_LEN];
/*==================================================================*/
/* 						LOCAL FUNCTION PROTOTYPES 					*/
/*==================================================================*/

void InitIO(void);

/*==================================================================*/
/* 								FUNCTIONS 							*/
/*==================================================================*/
static BUT but;


/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
int main(void)
{
	InitIO();			//Initialise port I/Os
	ASC_Init_asci();	//Initialise Serial Comms
	I2C_Init();
	TIM_Init_timer();	//initialise 1ms timer tick
	MEN_Init();			// Display Opening menu
	
	//Enter Endless loop
	while(1) 
	{
		MEN_bkproc();	
	}
}
/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
BUT MAI_Get_but(void)
{
	return but;
}

/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
void MAI_Set_but(BUT But)
{
	but  = But;
}

/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/

void InitIO(void)
{
	PORTD = TJ_ID_BIT | ADAPTER_NOT_DETECTED_BIT; // Enable pull up for TJ ID Bit, and Hypertronics Adapter
	// Define and set DEBUG pin at PORTD bit 2 (Pin 20)
	DDRD |= (PORT_EXPANDER_GSN3_BIT|PORT_EXPANDER_GSN2_BIT|PORT_EXPANDER_GSN1_BIT|PORT_EXPANDER_GSN0_BIT); // Ensure PORTD Bit 3 is set as Output
	PORT_EXPANDER_GSN_PORT = (PORT_EXPANDER_GSN_PORT & 0x0f);
	DDRC |= DEBUG_BIT;
	
	//Define Test Jig ID Port and Bit (PORTD Bit 3 (Pin 21) & External Hypertronics Adapter detect bit
	DDRD &= ~(TJ_ID_BIT| ADAPTER_NOT_DETECTED_BIT); // Ensure PORTD Bit 2 & 3 are set as inputs
	
	DEBUG_LO;
	DEBUG_HI;
	DEBUG_LO;
	DEBUG_HI;
	DEBUG_LO; 
	DEBUG_HI;
	DEBUG_LO;
	
	
	// set power control port as output
	DDRB |= POWER_CNTRL_BIT;
	MAI_Set_power(OFF);
}
/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
void MAI_Set_power(ONOFF_ENUM stat)
{
	if (stat == ON)	
		POWER_CNTRL_PORT |= POWER_CNTRL_BIT;
	else
		POWER_CNTRL_PORT &= ~POWER_CNTRL_BIT;
}
/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
int8 MAI_Set_header_cntrl(IPOP_ENUM ipop_stat, HILO_ENUM hilo_stat)
{
	// Check if header control is input
	if(ipop_stat == IP)
	{
		DDRB &= ~HEADER_CONFIG_BIT; // Set for input
	//	PORTB &= ~HEADER_CONFIG_BIT; // disable pull-up
	}
	else
	{	// config is output
		DDRB |= HEADER_CONFIG_BIT; // set for output
		// Now check for HI
		if(hilo_stat == HI)
			HEADER_PORT_WR |= HEADER_CONFIG_BIT; // Set port bit Hi
		else
			HEADER_PORT_WR &= ~HEADER_CONFIG_BIT; // Set port bit Lo
	}
	return HEADER_PORT_RD & HEADER_CONFIG_BIT; // return but status
}
/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
JIG_ID_ENUM MAI_Get_jig_id(void)
{
	int8 x, id_bit,n;

	// Read initial value of ID bit: 0 = FINAL;	1 = Board;
	id_bit = PIND & TJ_ID_BIT;
	
	for(x = 0; x < 4; x++)
	{
		// set 10ms Delay
		TIM_Set_delay(10);
		while(!TIM_Get_delay_flag()); //Wait for delay to expire;
		n = PIND & TJ_ID_BIT;
		
		if(id_bit != n)
			return INVALID_JIGID;
	}
	// Pin is same value so return appropriate enum
	if(id_bit)
		return BD_TEST_JIGID;
	else
		return FINAL_TEST_JIGID;
}
/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
void MAI_Set_port_expander_code(int8 code)
{
	PORT_EXPANDER_GSN_PORT = (PORT_EXPANDER_GSN_PORT & 0x0f) | ((code << 4) & (0xf0 | ADAPTER_NOT_DETECTED_BIT)); // sets code on PORTD and Ensure BIT3 pull-up is enabled

}
/*====================================================================
Name		:
Parameters	:
Returns		:
Description	:
--------------------------------------------------------------------*/
int8 MAI_Check_adapter_fitted(void)
{
	int8 id;
	
	id = PIND & ADAPTER_NOT_DETECTED_BIT;
	if(id)
		return FALSE;
	else
		return TRUE;
}
/*********************************************************************
*						End of main.c								 *
*********************************************************************/
