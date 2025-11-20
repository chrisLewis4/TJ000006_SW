/********************************************************************
*																	*
*	Filename:		main.h											*
*	Originator:		Chris Lewis										*
*	Project:		Net Connector EEPROM Programmer 				*
*	Description:													*
*																	*
********************************************************************/
#if !defined _main_H
#define _main_H
/*======================================================================*/
/*							NESTED INCLUDE FILES						*/
/*======================================================================*/
#include "ATMtypes.h"

/*======================================================================*/
/*							PUBLIC TYPEDEFS								*/
/*======================================================================*/

/*======================================================================*/
/*							PUBLIC DEFINES								*/
/*======================================================================*/
#define POWER_CNTRL_PORT PORTB
#define POWER_CNTRL_BIT BIT0


#define HEADER_PORT_WR PORTB
#define HEADER_PORT_RD PINB

#define HEADER_3V3_BIT BIT1
#define HEADER_CONFIG_BIT BIT2

/*======================================================================*/
/*						PUBLIC FUNCTION PROTOTYPES 						*/
/*======================================================================*/

extern void MAI_Set_power(ONOFF_ENUM stat);
extern int8 MAI_Set_header_cntrl(IPOP_ENUM ipop_stat, HILO_ENUM hilo_stat);

/************************************************************************
*						End of menu.h									*
************************************************************************/
#endif
