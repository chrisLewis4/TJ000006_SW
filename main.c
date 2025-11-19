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
	int8 c,tmp;
	int8 buf[20];
	
	int8 x;
	
	InitIO();
	ASC_Init_asci();
	
		
// DISPLAY INSTRUCTIONS

	MEN_Init();
	TIM_Init_timer();
	//MAI_Set_power(ON);
	
	tmp = TRUE;
	TIM_Delay(1000);

	//Enter Endless loop
	while(1) 
	{
			
		if(TIM_Get_delay_flag())
		{
			if(tmp)
			{
				MAI_Set_power(OFF);
				tmp = FALSE;
			}
			else
			{
				MAI_Set_power(ON);
				tmp = TRUE;
			}
			
			TIM_Delay(1000);
		}
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
	DDRC |= BIT6; //Set Port C Bit 6 as Debug port
	DEBUG_LO;
	DEBUG_HI;
	DEBUG_LO;
	DEBUG_HI;
	DEBUG_LO;
	DEBUG_HI;
	DEBUG_LO;
	
	// set power control port as output
	DDRB |= BIT0;
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
		PORTB |= BIT0;
	else
		PORTB &= ~BIT0;
}

/*********************************************************************
*						End of main.c								 *
*********************************************************************/
