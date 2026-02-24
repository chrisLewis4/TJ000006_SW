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
typedef enum tesjigid {FINAL_TEST_JIGID,BD_TEST_JIGID,INVALID_JIGID} JIG_ID_ENUM;
/*======================================================================*/
/*							PUBLIC DEFINES								*/
/*======================================================================*/
#define POWER_CNTRL_PORT PORTB
#define POWER_CNTRL_BIT BIT0

#define TJ_ID_BIT	BIT2
#define TJ_ID_PORT	PORTD

#define HEADER_PORT_WR PORTB
#define HEADER_PORT_RD PINB

#define HEADER_3V3_BIT BIT1
#define HEADER_CONFIG_BIT BIT2

#define PORT_EXPANDER_GSN_PORT PORTD
#define PORT_EXPANDER_GSN0_BIT BIT4
#define PORT_EXPANDER_GSN1_BIT BIT5
#define PORT_EXPANDER_GSN2_BIT BIT6
#define PORT_EXPANDER_GSN3_BIT BIT7

/*======================================================================*/
/*						PUBLIC FUNCTION PROTOTYPES 						*/
/*======================================================================*/

extern void MAI_Set_power(ONOFF_ENUM stat);
extern int8 MAI_Set_header_cntrl(IPOP_ENUM ipop_stat, HILO_ENUM hilo_stat);
extern JIG_ID_ENUM MAI_Get_jig_id(void);
extern void MAI_Set_port_expander_code(int8 code);


/************************************************************************
*						End of menu.h									*
************************************************************************/
#endif
