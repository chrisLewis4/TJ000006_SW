/********************************************************************
*																	*
*	Filename:		main.c											*
*	Originator:		Chris Lewis										*
*	Project:		Interface Board I2C Test Software				*
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
	DDRD |= BIT2; //Set Port C Bit 6 as Debug port
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
		DDRB &= ~HEADER_CONFIG_BIT; // Set for input
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

/*********************************************************************
*						End of main.c								 *
*********************************************************************/
