/********************************************************************
                                                                   
Filename:   menu.c                                              
Originator: C.Lewis                                             
Project:    TJ000006_SW EGI Net connector/Adapter Test Firmware 

This module is responsible for the debug and test menus used to test the following:
	* Net Connector PCB assembly - 6519-01
	* Net Adapter PCB assembly - 6768-01
	* Net final assemblies - many various P/Ns
	* Net Extender final assembly - 6708-00
	* Net adapter final assembly - 6771-00 (256 Chan) & 6772-00 (128 chan)
	
The board assy tests are configured by means of a jumper on the TJ000006 Hardware
	6519-01 Net connector board:
		* Verifies the EEPROM reading, writing and data retention
		* Checking the function of the I²C address jumper 
		* Storing the WO number, SN and assy numbers according to specification detailed in TD6500006
	6768-01 Net Adapter board
		* Verifies the EEPROM reading, writing and data retention
		* Verifies the correct operation of the I²C Port expander which read the net's Legacy configuration
	Net final assemblies
		* Programs the final assy no and SN of the finished net
	6771-00 (256 Chan) & 6772-00 (128 chan) Net adapter final assemblies
		* Programs the final assy no and SN of the finished net adapters
		* Verifies the correct operation of the I²C Port expander which read the net's Legacy configuration
	Net extender
		* Programs the final assy no and SN of the finished net extenders
		
********************************************************************/

/*==================================================================*/
/*                          INCLUDE FILES                           */
/*==================================================================*/
#include "ATMtypes.h"
#include "romdata.h"
#include "menu.h"
#include "Asci.h"
#include "romdata.h"
#include "main.h"
#include "version.h"
#include "Timer.h"
#include "i2c.h"
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>


/*==================================================================*/
/*                      LOCAL FUNCTION PROTOTYPES                   */
/*==================================================================*/
// main menus
static void Debug_menu(void);
static void Start_menu(void);
static void Select_bd_menu(void);
static void Fstart_menu(void);
			
//data entry funcs
static void Get_wo_no(void);
static void Get_final_assy_no(void);
static void Get_assy_rev_no(void);
static void Bd_test_start_menu(void);
static void Start_eeprom_prog(void);
static void Get_serial_no(void);
static void Connect_bd_menu(void);
static void Display_assy_details(void);

// Menu state MC funcs
static void Test_msg_func(void);
static int8 Cmd_check(int8);
static void Rom_msg(const int8  *MSG_PTR);


//EEPROM specific functions
static void Eeprom_debug_menu(void);
static int8 Eeprom_write(int8 eeprom_addr,int16 byte_addr,int16 byte_count,int8 *buf);
static int8 Eeprom_read(int8 eeprom_addr,int16 byte_addr,int16 byte_count,int8 *buf);
static void Eeprom_fill(int8 set_char);
static void Eeprom_hex_dump(void);
static int16 Get_stored_checksum(void);
static int16 Calc_stored_checksum(void);
static void Store_checksum(int16 checksum);
static int8 Get_stored_digit_string(int16 eeprom_pos, int16 size, int8 *buf);
static int8 Get_stored_alpha_string(int16 eeprom_pos, int16 size, int8*buf);
static void	Display_formatted_assy_info(void);
static int8 Verify_stored_data(void);
static int8 Check_alpha_char(int8 c);
static int8 Check_number_char(int8 c);
static int8 Wait_for_x_or_enter(void);
static void Prog_debug_details(void);




// Port Expander (PE) specific funcs
static void Init_pe_debug_menu(void);
static void Port_expander_debug_menu(void);
static int8 Port_expander_test(void);

// Final test specific funcs
static void Get_final_assy_rev_no(void);
static void Display_final_assy_details(void);
static void Get_final_assy_sn(void);
static void Final_test_start_menu(void);
static void Prog_final_assy_details(void);
static void Prog_same_final_assy_menu(void);
static void Retry_prog_final_assy_menu(void);
static void Get_secret_access_code(void);
static void Net_id_menu(void);










/*==================================================================*/
/*                      LOCAL TYPE DEFINITIONS                      */
/*==================================================================*/
typedef enum final_assy_types {NET_ASSY, EXTENSION_ASSY, ADAPTER_ASSY, LASTASSY_TYPE} FINAL_ASSY_TYPES;
/*==================================================================*/
/*                      LOCAL MACRO DEFINITIONS                     */
/*==================================================================*/
// User input defines
#define MAX_USER_IP_LEN 20
#define ACCESS_CODE_LEN 6

/* definition used to echo the terminal keys pressed */
#define CMD_ECHO        TRUE
#define CMD_NO_ECHO     FALSE

/* NVRAM definitions */
#define WRITE_CHAR_LOW 0x5A
#define WRITE_CHAR_HIGH 0xA5

#define CHECKSUM_MSB_ADDR 0x7e
#define CHECKSUM_LSB_ADDR 0x7f


#define EEPROM_SET_CHAR 0xff
#define EEPROM_RESET_CHAR 0x00
#define BYTE_WRITE_ADDR_HI 0x10f
#define BYTE_WRITE_ADDR_LO 0x040

#define MAX_WO_STRING_LEN 10
#define ASSY_STRING_LEN 4
#define ASSY_REV_STRING_LEN 3
#define BD_SN_STRING_LEN 3
#define FINAL_SN_STRING_LEN 12


#define EEPROM_ADDR_LO 0x50
#define EEPROM_ADDR_HI 0x54
#define EEPROM_BYTE_COUNT 512
#define EEPROM_PAGE_SIZE 16

// Define valid adresses
#define NET_CONN_I2C_ADDR		0x50
#define NET_EXTEND_I2C_ADDR		0x54
#define ADC_BOT_I2C_ADDR		0x52
#define ADC_TOP_I2C_ADDR		0x56
#define PORT_EXPAND_I2C_ADDR	0x41

// define storage layout positions from TD6500006 Net Spec
#define EEPROM_FINAL_PROD_NUM_LAYOUT_POS		0x00	//
#define EEPROM_FINAL_PROD_CODE_LAYOUT_POS		0x04 // This will always be 00
#define EEPROM_FINAL_PROD_REV_LAYOUT_POS		0x06
#define EEPROM_FINAL_PROD_SN_LAYOUT_POS			0x09
#define EEPROM_FINAL_PROD_NETID_LAYOUT_POS		0x22
#define EEPROM_FINAL_PROD_CHANCOUNT_LAYOUT_POS	0x23

#define EEPROM_ASSY_NUM_STRING_LAYOUT_POS	0xe2 // Full PCB assy num with null termination: xxxx-01-nny/0
#define EEPROM_ASSY_WO_STRING_LAYOUT_POS	0xef
#define EEPROM_ASSY_SN_STRING_LAYOUT_POS	0xfa
#define EEPROM_CHECKSUM_LAYOUT_POS			0xfe

// define storage layout sizes from TD6500006 Net Spec
#define EEPROM_FINAL_PROD_NUM_LAYOUT_SIZE		ASSY_STRING_LEN
#define EEPROM_FINAL_PROD_CODE_LAYOUT_SIZE		2
#define EEPROM_FINAL_PROD_REV_LAYOUT_SIZE		ASSY_REV_STRING_LEN
#define EEPROM_FINAL_PROD_SN_LAYOUT_SIZE		(FINAL_SN_STRING_LEN+1)
#define EEPROM_FINAL_PROD_NETID_LAYOUT_SIZE		1
#define EEPROM_FINAL_PROD_CHANCOUNT_LAYOUT_SIZE	1

#define EEPROM_ASSY_NUM_STRING_LAYOUT_SIZE	12	// Full PCB assy num with null termination: xxxx-01-nny/0
#define EEPROM_ASSY_WO_STRING_LAYOUT_SIZE		(MAX_WO_STRING_LEN+1)	// Null terminated string padded with 0's
#define EEPROM_BD_ASSY_SN_STRING_LAYOUT_SIZE	(BD_SN_STRING_LEN+1)	// Null terminated string
#define EEPROM_CHECKSUM_LAYOUT_SIZE			2   // 2 bytes - MSB first

/*==================================================================*/
/*                      GLOBAL CONSTANT DEFINITIONS                  */
/*==================================================================*/
int8 const NEWLINE_MSG[] PROGMEM =		{"\n\r"};
int8 const NEWPAGE_MSG[] PROGMEM =		{"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\r"};
int8 const DOT_MSG[] PROGMEM =		{"."};
/*==================================================================*/
/*                      LOCAL CONSTANT DEFINITIONS                  */
/*==================================================================*/
 // Opening Menu Msgs
 int8 const COPYRIGHT_MSG1[]  PROGMEM =	
 {"**********************************************\n\r"
  "| Copyright(c) 2026 The Magstim Company Ltd. | \n\r"
  "|   EEG Net Connector - I2C Test Firmware    |\n\r"
  "**********************************************\n\n\r"
  "Firmware ID: "};


int8 const FINAL_TEST_JIG_MSG[] PROGMEM = {"\n\nFinal Test Firmware\n\r"
											   "-------------------\n\r"};
int8 const INVALID_JIG_ID_MSG[] PROGMEM = {"\n\n\r!!! INVALID TEST JIG ID DETECTED !!!\n\r"};								  
int8 const BD_TEST_JIG_MSG[] PROGMEM = {"\n\nBoard Test Firmware\n\r"
										    "-------------------\n\n\r"};
 /* common menu messages */
 static int8 const CMD_NOT_IMPLEMENTED_MSG[] PROGMEM =	{" Command not implemented\n\r"};
 
// define Net Type Msgs
int8 const NET_CONN_PCB_MSG[] PROGMEM = {"Net Connector PCB"};
int8 const NET_ADAPT_PCB_MSG[] PROGMEM = {"Net Adapter PCB"};
	
int8 const STORED_DETAILS_MSG[] PROGMEM = {"\n\rCurrent EEPROM Contents\n\r"
										       "=======================\n\n\r"};
/* declare Terminal Menus text*/
//*********************  Start Menu  ***************************
int8 const FSTART_MENU_MSG[] PROGMEM =
{
	"\n\n\r"
	"Final Test Start Menu\n\r"
	"=====================\n\n\r"	
	"Select the Product Type\n\r"
	"1 - NET Assembly\n\r"
	"2 - NET Extension\n\r"
	"3 - NET Adapter\n\n\r"
	"V - View Stored Info\n\r"
};
int8 const INVALID_ENTRY_MSG[] PROGMEM =
{
	"\n\n\r"
	"!!! INVALID ENTRY - Please Retry\n\r"
};
int8 const ENTER_FINAL_ASSY_NO_MSG[] PROGMEM =
{
	"\n\n\r"
	"Enter the Product Part Number - 4 Numeric characters\n\r"
};
int8 const ENTER_FINAL_ASSY_REV_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Enter the Product's Revision - 3 Characters (e.g. ""00A"")\n\r"
};
static const int8 NET_ASSY_MSG[] PROGMEM = {"NET ASSEMBLY"};
static const int8 EXTENSION_ASSY_MSG[] PROGMEM = {"NET EXTENSION ASSEMBLY"};
static const int8 ADAPTER_ASSY_MSG[] PROGMEM = {"NET ADAPTER ASSEMBLY"};

int8 const ENTER_FINAL_ASSY_SN_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Enter the Serial no. of the Final Assembly under test\n\r(MAX = 12 Characters)\n\r"
};

int8 const SELECT_NET_ID_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Select the Net Size ID from the following list\n\n\r"
	"    ID                DESCRIPTION\n\r"
	"-----------------------------------------------------\n\r"
	"0 - 0  64  Channel Geodesic Sensor Nets 2.x\n\r"
	"1 - 1  128 Channel Geodesic Sensor Nets 2.x\n\r"
	"2 - 2  256 Channel Geodesic Sensor Nets 2.x\n\r"
	"3 - 3  32  Channel Hydrocel Geodesic Sensor Nets 1.0\n\r"
	"4 - 4  64  Channel Hydrocel Geodesic Sensor Nets 1.0\n\r"
	"5 - 5  128 Channel Hydrocel Geodesic Sensor Nets 1.0\n\r"
	"6 - 6  256 Channel Hydrocel Geodesic Sensor Nets 1.0\n\r"
	"7 - 7  32  Channel Microcel Geodesic Sensor Nets 1.0\n\r"
	"8 - 8  64  Channel Microcel Geodesic Sensor Nets 1.0\n\r"
	"9 - 9  128 Channel Microcel Geodesic Sensor Nets 1.0\n\r"
	"a - 10 256 Channel Microcel Geodesic Sensor Nets 1.0\n\r"
};
int8 const NET_ID0_MSG[] PROGMEM = {"0: 64 Channel Geodesic Sensor Nets 2.x"};
int8 const NET_ID1_MSG[] PROGMEM = {"1: 128 Channel Geodesic Sensor Nets 2.x"};
int8 const NET_ID2_MSG[] PROGMEM = {"2: 256 Channel Geodesic Sensor Nets 2.x"};
int8 const NET_ID3_MSG[] PROGMEM = {"3: 32 Channel Hydrocel Geodesic Sensor Nets 1.0"};
int8 const NET_ID4_MSG[] PROGMEM = {"4: 64 Channel Hydrocel Geodesic Sensor Nets 1.0"};
int8 const NET_ID5_MSG[] PROGMEM = {"5: 128 Channel Hydrocel Geodesic Sensor Nets 1.0"};
int8 const NET_ID6_MSG[] PROGMEM = {"6: 256 Channel Hydrocel Geodesic Sensor Nets 1.0"};
int8 const NET_ID7_MSG[] PROGMEM = {"7: 32 Channel Microcel Geodesic Sensor Nets 1.0"};
int8 const NET_ID8_MSG[] PROGMEM = {"8: 64 Channel Microcel Geodesic Sensor Nets 1.0"};
int8 const NET_ID9_MSG[] PROGMEM = {"9: 128 Channel Microcel Geodesic Sensor Nets 1.0"};
int8 const NET_ID10_MSG[] PROGMEM = {"10: 256 Channel Microcel Geodesic Sensor Nets 1.0"};

static int8 const *net_id_list[11] = 
{
	NET_ID0_MSG,
	NET_ID1_MSG,
	NET_ID2_MSG,
	NET_ID3_MSG,
	NET_ID4_MSG,
	NET_ID5_MSG,
	NET_ID6_MSG,
	NET_ID7_MSG,
	NET_ID9_MSG,
	NET_ID9_MSG,
	NET_ID10_MSG,
};
// chan count list NOTE: value is chan count -1 
static int8 chan_count_list[11] = {63,127,255,31,63,127,255,31,63,127,255};	

int8 const TEST_SAME_FINAL_ASSY_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"*********************************************************\n\r"
	"*** Final Assembly successfully Programmed and Tested ***\n\r"
	"*********************************************************\n\n\r"
	"Check the detail shown above are correct\n\r"
	"Remove Tested Assembly and complete the necessary Paperwork\n\n\r"
	"Press 'ENTER' to program a Final assembly of the same type\n\r"
	"Press 'X' to exit to start menu\n\r"
};

int8 const PROG_SAME_FINAL_ASSY_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"**********************************************\n\r"
	"*** Final Assembly successfully Programmed ***\n\r"
	"**********************************************\n\r"
	"Check the detail shown on the screen are correct\n\r"
	"Remove Tested Assembly and complete the necessary Paperwork\n\n\r"
	"Press 'ENTER' to program a Final assembly of the same type\n\r"
	"Press 'X' to exit to start menu\n\r"
};

int8 const FINAL_ASSY_PROG_FAIL_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"!!! FAILED TO PROGRAM DETAILS !!!\n\r"
	"Check:\n\r"
	"   *The the final assembly type is correct\n\r"
	"   *The Assembly is correctly fitted to the Test Jig\n\n\r"
	"Press 'ENTER' to RETRY  of 'X' to exit to start menu\n\r"
};

int8 const CONNECT_FINAL_ASSY_TO_JIG_MSG[] PROGMEM =
{
	"\n\n\r"
	"### Connect the Final Assembly to the Test Jig ###\n\n\r"
};


int8 const CONNECT_LEGACY_ADAPTER_MSG[] PROGMEM =
{
	"\n\n\r"
	"*** Final Assembly successfully Programmed ***\n\n\r"
	"### Connect the Legacy Adapter end of the Final Assembly to the Test Jig ###\n\r"
	"Press any key when ready\n\r"
};

int8 const ASSY_ID_TEST_FAILED_MSG[] PROGMEM =
{
	"\n\r!!! NET IDENTIFICATION TEST FAILED !!!\n\r"
	"Press 'X' to exit or ENTER key to Retry\n\r"
};

int8 const FINAL_ASSY_TEST_FAIL_MSG[] PROGMEM =
{
	"!!!!!!!!!!!!!!!!!!!\n\r"
	"!!! TEST FAILED !!!\n\r"
	"!!!!!!!!!!!!!!!!!!!\n\n\r"
	"Remove Assembly and quarantine or retest\n\n\r"
	"Press 'ENTER' to program a Final assembly of the same type\n\r"
	"Press 'X' to exit to start menu\n\r"	
};


int8 const ASSY_TYPE_EXTENDER_MSG[] PROGMEM =
{
	"\n\n\n\rAssembly Type is NET EXTENDER\n\n\r"
};
int8 const ASSY_TYPE_CONN_ADAPTER_MSG[] PROGMEM =
{
	"\n\n\n\rAssembly Type is NET CONNECTOR/ADAPTER\n\r"
};
int8 const BD_TYPE_NET_CONN_MSG[] PROGMEM =
{
	"\n\n\n\rBoard Type is NET CONNECTOR\n\n\r"
};
int8 const BD_TYPE_EXTENDER_MSG[] PROGMEM =
{
	"\n\n\n\rBoard Type is NET EXTENDER\n\n\r"
};
				
				

int8 const ASSY_NOT_DETECTED_MSG[] PROGMEM =
{
	"\n\n\n\r!!!No Assembly Connected to Jig !!!\n\n\r"
};

int8 const START_MENU_MSG[] PROGMEM =
{
	"\n\n\r"
	"Start MENU\n\r"
	"============\n\r"
	"T - Board tests\n\r"
	"D - Debug Menu\n\r"
	"V - View Stored Info\n\r"
	"<ENTER>- To refresh screen\n\n\r"
};

int8 const SELECT_BD_MSG[] PROGMEM =
{
	"\n\n\r"
	"Select Board Under Test (BUT)\n\r"
	"=============================\n\r"
	"1 - 6519-01 - Net Connector PCB\n\r"
	"2 - 6768-01 - Net Adapter PCB\n\r"
	"X - Exit to Start Menu\n\r"
	"<ENTER>- To refresh screen\n\n\r"
};

int8 const NET_CONN_SELECTED_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"6519-01 Net Connector PCB selected\n\r"
};
int8 const NET_ADAPT_SELECTED_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"6768-01 Net Adapter PCB selected\n\r"
};

int8 const ENTER_WO_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Enter the Works Order No (WO) of the batch of boards Under Test\n\r(MAX = 10 Characters)\n\r"
};
int8 const ENTER_BD_ASSY_NO_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Enter the board's Assembly No of the batch of boards Under Test\n\r(4 Characters)\n\r"
};
int8 const ENTER_BD_ASSY_REV_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Enter the board's Assembly Revision No of the batch of boards Under Test\n\r(3 Characters)\n\r"
};

int8 const CHECK_DETAILS_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Check the following details are correct\n\n\r"
};
int8 const PRESS_X_OR_PROCEED_MSG[] PROGMEM =
{
	"Press 'X' to exit or ENTER key to proceed\n\n\n\r"
};

int8 const ENTER_SN_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Enter the board's Serial No.\n\r(3 Characters) or 'X' to exit\n\r"
};
int8 const ENTER_NEXT_SN_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Enter the Next board's Serial No\n\r"
};

int8 const CONNECT_BD_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Connect the board Under Test to the Jig\n\r"
	"ENSURE FLYING LEAD IS CONNECTED to JUMPER\n\n\r"
	"Press 'X' to exit or ENTER key to proceed\n\r"
};
int8 const CONNECT_ADAPTER_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Connect the Adapter board Under Test to the Jig\n\r"
	"Press 'X' to exit or ENTER key to proceed\n\r"
};

int8 const PROG_EEPROM_MSG[] PROGMEM =
{
	"\n\n\n\r"
	"Automatically checking and Programming EEPROM\n\r"
};

int8 const PROG_SUCCESS_MSG[] PROGMEM =
{
	"\n\rSuccessfully Checked and Programmed EEPROM!!\n\r"
};

int8 const PORT_EXPANDER_TEST_FAILED_MSG[] PROGMEM =
{
	"\n\r!!! NET IDENTIFICATION TEST FAILED !!!\n\r"
	"Press 'X' to exit or ENTER key to Retry\n\r"
};

int8 const BD_TEST_SUCCESS_MSG[] PROGMEM =
{
	"\n\n\r*******************\n\r"
	"*** TEST PASSED ***\n\r"
	"*******************\n\n\r"
	"Remove Board and apply TESTED MARK\n\n\r"
};

int8 const BD_TEST_FAIL_MSG[] PROGMEM =
{
	"!!!!!!!!!!!!!!!!!!!\n\r"
	"!!! TEST FAILED !!!\n\r"
	"!!!!!!!!!!!!!!!!!!!\n\n\r"
	"Remove Board and quarantine or retest board\n\n\r"
};

int8 const MAX_USER_IP_LEN_EXEEDED_MSG[] PROGMEM =
{
	"\n\n\r!!! Too many characters entered - Please retry !!!\n\r"	
};

int8 const NOT_ENOUGH_ASSY_NO_IP_CHARS_MSG[] PROGMEM =
{
	"\n\n\r!!! Not Enough characters entered - 4 Required !!!\n\rPlease retry\n\r"
};
int8 const NOT_ENOUGH_ASSY_REV_IP_CHARS_MSG[] PROGMEM =
{
	"\n\n\r!!! Not Enough characters entered - 3 Required !!!\n\rPlease retry\n\r"
};
int8 const NO_FINAL_ASSY_REV_SN_IP_CHARS_MSG[] PROGMEM =
{
	"\n\n\r!!! No characters entered - maximum of 12 Required !!!\n\rPlease retry\n\r"
};

int8 const NOT_ENOUGH_WO_NO_IP_CHARS_MSG[] PROGMEM =
{
	"\n\n\r!!! Not Enough characters for Works Order - 1 Required !!!\n\rPlease retry\n\r"
};

int8 const CHECK_HEADER_MSG[] PROGMEM =
{
	"\n\n\r!!! Check Header is fitted correctly !!!\n\rPress 'x' to ENTER key to retry\n\r"
};
int8 const CHECKSUM_MISMATCH_MSG[] PROGMEM =
{
	"\n\n\r!!! Checksum mismatch !!!\n\rSetting up EEPROM\n\r"
};
int8 const CHECKSUM_MATCH_MSG[] PROGMEM =
{
	"\n\n\r!!! WARNING !!!\n\rCalculated and Stored Checksum Match\n\n\r"
};
int8 const DATA_NOT_SET_MSG[] PROGMEM =
{
	"Undefined"
};

int8 const PRODUCT_PN_MSG[] PROGMEM =
{
	"Product P/N  = "
};
int8 const PRODUCT_SN_MSG[] PROGMEM =
{
	"Product S/N  = "
};
int8 const PCB_ASSY_PN_MSG[] PROGMEM =
{
	"PCB Assy P/N = "
};
int8 const PCB_ASSY_SN_MSG[] PROGMEM =
{
	"PCB Assy S/N = "
};
int8 const CHECKING_DATA_RETENTION_MSG[] PROGMEM =
{
	"\n\rChecking data retention\n\r"
	"Powering OFF for 3 seconds\n\n\r"
};
int8 const DATA_RETENTION_OK_MSG[] PROGMEM =
{
	"\n\rData retention OK\n\n\r"
};
int8 const DATA_RETENTION_BAD_MSG[] PROGMEM =
{
	"\n\rData retention Check FAILED!!\n\r"
};

int8 const EEPROM_READ_ERROR_MSG[] PROGMEM =
{
	"\n\n\r !!! EEPROM READ ERROR !!!\n\n\r"
};
int8 const ALPHANUMERIC_CHARS_ONLY_MSG[] PROGMEM =
{
	"\n\n\r !!! Please Re-Enter using Alpha-numeric characters only ('A'-'Z', '0'-'9') !!!\n\r"
};
int8 const NUMERIC_CHARS_ONLY_MSG[] PROGMEM =
{
	"\n\n\r !!! Please Re-Enter using numeric characters only ('0'-'9') !!!\n\r"
};

int8 const EEPROM_DEBUG_MSG[] PROGMEM =
{
	"\n\n\n\n\rEEPROM Debug"
	"\n\r============\n\n\r"
	"D - Display Stored Details\n\r"
	"H - Hex Dump\n\r"
	"W - Write\n\r"
	"R - Reset EEPROM to 00\n\r"
	"S - Set EEPROM to FF\n\r"
	"C - Display Checksums\n\r"
	"P - Program Board Details\n\r"
	"X - Return to Debug Menu\n\n\r"
};
int8 const DEBUG_MENU_MSG[] PROGMEM =
{
	"\n\n\n\n\r"
	"DEBUG MENU\n\r"
	"==========\n\r"
	"\n\rSelect EEPROM Address:\n\r"
	"1 - 0x50: Net Connector/Net Adapter\n\r"
	"2 - 0x52: ADC Bottom Bd\n\r"
	"3 - 0x54: Net Extension\n\r"
	"4 - 0x56: ADC Top Bd\n\r"
	"5 - 0x41: Port Expander (HW000112 Only)\n\r"	
	"\n\rPress X to exit\n\n\r"
};

int8 const PORT_EXPANDER_DEBUG_MSG[] PROGMEM =
{
	"\n\n\n\n\r"
	"Port Expander Debug\n\r"
	"===================\n\r"
	"Press 'X' to Exit\n\n\r"
	"GSN3 GSN2 GSN1 GSN0\n\r"
};
int8 const PORT_EXPANDER_TEST_MSG[] PROGMEM =
{
	"\n\r"
	"Net Identification Test\n\r"
	"=======================\n\r"
	"Bit3 = GSN3, Bit2 = GSN2, Bit1 = GSN1, Bit0 = GSN0\n\n\r"
	"OUT    IN\n\r"
};
int8 const ACCESS_CODE[ACCESS_CODE_LEN] PROGMEM = {"270463"};

/*==================================================================*/
/*      LOCAL INITIALISED VARIABLES (initialised to 0 by default)   */
/*==================================================================*/
/* general test menu vars */
static void (*cmd_bk_func)(void);
static void (*next_cmd_func)(void);

static const int8 *MEN_MSG_PTR;

static int8 user_ip_buf[MAX_USER_IP_LEN+1];
static int8 user_ip_buf_ix;
static int8 user_ip_max_chars;

// Define assy info storage
static int8 wo_no_str[MAX_WO_STRING_LEN+1];
static int8 assy_no_str[ASSY_STRING_LEN+1];
static int8 assy_rev_str[ASSY_REV_STRING_LEN+1];
static int8 bd_serial_no_str[BD_SN_STRING_LEN+1];
static int8 final_assy_serial_no_str[FINAL_SN_STRING_LEN+1];
static int8 final_assy_no_str[ASSY_STRING_LEN+1];
static int8 final_assy_rev_str[ASSY_REV_STRING_LEN+1];
static int8 formatted_assy_no_string[EEPROM_ASSY_NUM_STRING_LAYOUT_SIZE+1];
// define EEPROM address storage
static int8 cur_i2C_addr;
static int8 last_i2C_addr;
static int16 cur_calc_checksum;
static int16 cur_stored_checksum;

//define PCB type flag
static int8 adapter_flag;
static int8 debug_flag;
static JIG_ID_ENUM bd_id;

//define Final Assy type
static FINAL_ASSY_TYPES final_assy_type;
static int8 const *final_assy_name;
static int8 net_id;
static int8 extender_flag;

/*********************************************************************
*                               FUNCTIONS                            *
*********************************************************************/
/*====================================================================
Name        :MEN_Init
Parameters  :NONE
Returns     :NONE
Description :Initializes some of the test module variables just in case
            :test menu is run latter. Sets the test state machine inactive
--------------------------------------------------------------------*/
void MEN_Init(void)
{
	Rom_msg(NEWPAGE_MSG);
	Rom_msg(COPYRIGHT_MSG1);		//display Copyright msg
	Rom_msg(VER_Get_sw_pn());	//Display Firmware PN and Version
	Rom_msg(NEWLINE_MSG);
	
	bd_id = MAI_Get_jig_id();
	
	if(bd_id == FINAL_TEST_JIGID)
	{
		Rom_msg(FINAL_TEST_JIG_MSG);	//display not Bd Test
		MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu); // Display Final test start menu
	}
	else if(bd_id == INVALID_JIGID)
	{
		Rom_msg(INVALID_JIG_ID_MSG);	//display not valid
		cmd_bk_func = NULL;
	}
	else
	{
		Rom_msg(BD_TEST_JIG_MSG);	//display not Bd Test 
		MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu); // Display start menu
	}
}
/*====================================================================
Name        :MEN_bkproc
Parameters  :NONE
Returns     :NONE
Description :Called from the main loop, it executes the current cmd func if set
--------------------------------------------------------------------*/
void MEN_bkproc(void)
{
    /* check for and execute cmd func */
    if(cmd_bk_func)
        (*cmd_bk_func)();
}


/*====================================================================
Name        :MEN_Shutdown
Parameters  :NONE
Returns     :NONE
Description :Called from main() during system shutdown.
             It invalidates the cmd func
--------------------------------------------------------------------*/
void MEN_Shutdown(void)
{
    cmd_bk_func = NULL;
}

/*====================================================================
Name        :MEN_Set_cmd_bk_func
Parameters  :msg to display, next cmd func to execute
Returns     :NONE
Description :Loads the passed vars to local vars and sets the cmd func to display
             the passed message and set the passed cmd func
--------------------------------------------------------------------*/
void MEN_Set_cmd_bk_func(const int8  *MSG_PTR, void * const next_cmd_ptr)
{
    /* store passed params locally */
    MEN_MSG_PTR = MSG_PTR;
    next_cmd_func = next_cmd_ptr;
    /* set cmd func */
    cmd_bk_func = Test_msg_func;
}
/*====================================================================
Name        :Rom_msg
Parameters  :msg to display
Returns     :NONE
Description :Saves tyhe current bk task then calls the Test_msg_func 
			repeatedly until the message has been sent.
			Then restores the current bk task
--------------------------------------------------------------------*/
static void Rom_msg(const int8  *MSG_PTR)
{
	void *cur_cmd_ptr;
	
	//wait for empty serial tx 
	while(!ASC_Asci_tx_empty());
	
	cur_cmd_ptr = cmd_bk_func; // save current bk task
    MEN_MSG_PTR = MSG_PTR;		// set menu msg ptr
	
	while(MEN_MSG_PTR)
		Test_msg_func();
	
	cmd_bk_func = cur_cmd_ptr; // restore current bk task
	
}
/*====================================================================
Name        :Test_msg_func
Parameters  :NONE
Returns     :NONE
Description :Displays the message, stored in the program space,  pointed
             to by msg_ptr var.
             If the message length exceeds the MAX_ASCI_STRING length, it
             displays MAX_ASCI_STRING characters of the message and then
             exits. When the background process calls this function again
             it displays the next MAX_ASCI_STRING characters. It repeats
             this until the whole string has been displayed.
             Once the message has been displayed it sets the cmd func
             according to the next_cmd_func var.
--------------------------------------------------------------------*/
static void Test_msg_func(void)
{
    int16 len;
    int8 *ram_ptr;

    /* check for valid msg */
    if(MEN_MSG_PTR)
    {
        /* first set the len var according to the length of the string */
        len = ROM_Romstrlen(MEN_MSG_PTR);       /* get length of ROM string */
        if(len > MAX_ASCI_STRING)       /* check if len exceeds max length */
            len = MAX_ASCI_STRING;      /* limit string length */

        /*   wait until enough space in ASCI buffer */
        while(ASC_Get_asci_tx_space() < len);

        /* now move data from ROM to RAM and send to asci */
        ram_ptr = ROM_Read_romdata(MEN_MSG_PTR,len);
        MEN_MSG_PTR += len;                 /* adjust msg ptr */

        ASC_Asci_tx((const int8 *)ram_ptr,len);       /* send 'len' bytes of data */

        /* reset msg ptr if NUL char detected */
        if(pgm_read_byte(MEN_MSG_PTR) == 0)         /* check for '\0' char */
            MEN_MSG_PTR = NULL;                     /* reset msg ptr */
    }
    else  /* msg ptr is NULL so set new cmd func */
        cmd_bk_func = next_cmd_func;
}

/*====================================================================
Name        :Cmd_check
Parameters  :echo ON/OFF stat
Returns     :RS232 RX char, or 0 if empty
Description :Checks the ASC_Asci buffer for RX chars
             If detected it echos the char if the echo_stat is TRUE
--------------------------------------------------------------------*/
static int8 Cmd_check(int8 echo_stat)
{
    char c;

    /* return if no RX chars available */
    if(ASC_Asci_getchar((int8 *)&c) == ASCI_EMPTY)
        return 0;

    /* check echo_stat */
    if(echo_stat)
    {
        /* echo RX char */
        if(c == '\r')
            Rom_msg(NEWLINE_MSG);
        else
            ASC_Asci_tx((const int8 *)&c,1);
    }
    /* return RX char */
    return c;
}

//========================================================================================================================================
//	Product Specific Menus
//========================================================================================================================================
//====================================================================
// Name			:Start_menu
// Parameters	:NONE
// Returns		:NONE
// Description	:Set as the cmd_func when command menu is invoked
//             It gets any RX char and processes them according to their value
//--------------------------------------------------------------------

/*====================================================================
Name        :
Parameters  :
Returns     :
Description :
--------------------------------------------------------------------*/

static void Start_menu(void)
{
	int8 rx_byte, *msg;

	if(bd_id == FINAL_TEST_JIGID)
		MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu); // Display Final test start menu
	
	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
		return;

	/* now process RX char */
	switch(rx_byte)
	{
		case 'T':
		case 't':
			user_ip_buf_ix = 0;	//reset user input buf index
			user_ip_max_chars = MAX_WO_STRING_LEN;
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(SELECT_BD_MSG,Select_bd_menu);
			debug_flag = FALSE;
			break;
		case 'D':
		case 'd':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(DEBUG_MENU_MSG,Debug_menu);
			debug_flag = TRUE;
			break;
		case 'V':
		case 'v':
			Rom_msg(NEWPAGE_MSG);
			extender_flag = FALSE;
			MAI_Set_power(ON);
			TIM_Wait(100);
			cur_i2C_addr = EEPROM_ADDR_LO;
			msg = ROM_Read_romstr(BD_TYPE_NET_CONN_MSG);
			if(!I2C_ping_addr(cur_i2C_addr))
			{
				cur_i2C_addr = EEPROM_ADDR_HI;	// no response so set Hi addr
				if(I2C_ping_addr(cur_i2C_addr))
				{
					extender_flag = TRUE;
					msg = ROM_Read_romstr(BD_TYPE_EXTENDER_MSG);
				}
				else
				{
					Rom_msg(ASSY_NOT_DETECTED_MSG);
					MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
					MAI_Set_power(OFF);
					return;
				}
			}
			ASC_Asci_msg(msg);
			Display_formatted_assy_info();
			MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
			MAI_Set_power(OFF);
			return;
		default:
			Rom_msg(CMD_NOT_IMPLEMENTED_MSG);
			MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
		break;
	}
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Select_bd_menu(void)
{
	int8 rx_byte, *msg = NULL;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
	return;

	/* now process RX char */
	switch(rx_byte)
	{
		case '1':
			strcpy((char *)assy_no_str,"6519");
			adapter_flag = FALSE;
			msg = ROM_Read_romstr(NET_CONN_SELECTED_MSG);
			break;
		case '2':
			strcpy((char *)assy_no_str,"6768");
			adapter_flag = TRUE;
			msg = ROM_Read_romstr(NET_ADAPT_SELECTED_MSG);
			break;
		case 'x':
		case 'X':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
			return;
		default:
			Rom_msg(CMD_NOT_IMPLEMENTED_MSG);
			MEN_Set_cmd_bk_func(SELECT_BD_MSG,Select_bd_menu);
			return;
	}
	Rom_msg(NEWPAGE_MSG);
	ASC_Asci_msg(msg);
	user_ip_buf_ix = 0;	//reset user input buf index
	user_ip_max_chars = MAX_WO_STRING_LEN;
	MEN_Set_cmd_bk_func(ENTER_WO_MSG,Get_wo_no);
}

/*==================================================================
Name		:
Parameters	:
Returns		:
Description	:
------------------------------------------------------------------*/

static void Get_wo_no(void)
{
	int8 rx_byte;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
	return;

	/* now process RX char */
	switch(rx_byte)
	{
		case '\n':
		case '\r':
			if(user_ip_buf_ix == 0)
			{
				Rom_msg(NOT_ENOUGH_WO_NO_IP_CHARS_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = MAX_WO_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_WO_MSG,Get_wo_no);
				return;
			}
			// else break to set next data input
			break;
		case 'x':
		case 'X': // exit to start menu and return
			if(user_ip_buf_ix == 0)
			{
				Rom_msg(NEWPAGE_MSG);
				MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
				return;
			} 
			// don't break so that 'x' can be in WO no. with default, but not first char
		default:
			if(Check_alpha_char(rx_byte))	//check for valid char
			{
				user_ip_buf[user_ip_buf_ix++] = rx_byte;
				if(user_ip_buf_ix < user_ip_max_chars)
					return;
				else
					break;
			}
			else
			{
				Rom_msg(ALPHANUMERIC_CHARS_ONLY_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = MAX_WO_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_WO_MSG,Get_wo_no);
				return;
			}
			
			break;
	}
	user_ip_buf[user_ip_buf_ix] = '\0'; // terminate string
	strcpy((char *)wo_no_str,(char *)user_ip_buf);	// copy string
	// pad out WO string with 0's
	for( ; user_ip_buf_ix < MAX_WO_STRING_LEN; user_ip_buf_ix++)
		wo_no_str[user_ip_buf_ix] = '\0';

	user_ip_buf_ix = 0;	//reset user input buf index
	user_ip_max_chars = ASSY_REV_STRING_LEN;
	MEN_Set_cmd_bk_func(ENTER_BD_ASSY_REV_MSG,Get_assy_rev_no);

}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Get_assy_rev_no(void)
{
int8 rx_byte;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
		return;

	/* now process RX char */
	switch(rx_byte)
	{
		case '\n':
		case '\r':
			if(user_ip_buf_ix < user_ip_max_chars)
			{
				Rom_msg(NOT_ENOUGH_ASSY_REV_IP_CHARS_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = ASSY_REV_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_BD_ASSY_REV_MSG,Get_assy_rev_no);
				return;
			}
			break;
		case 'x':
		case 'X':
			if(user_ip_buf_ix == 0)
			{
				Rom_msg(NEWPAGE_MSG);
				MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
				return;
			}
			// don't break so that 'x' can be in WO no. with default, but not first char
		default:
			if(Check_alpha_char(rx_byte))
			{
				user_ip_buf[user_ip_buf_ix] = rx_byte;
				if(++user_ip_buf_ix < user_ip_max_chars)
					return;
			}
			else
			{
				Rom_msg(ALPHANUMERIC_CHARS_ONLY_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = ASSY_REV_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_BD_ASSY_REV_MSG,Get_assy_rev_no);
				return;

			}
			break;
	}
	user_ip_buf[user_ip_buf_ix] = 0; // terminate string
	strcpy((char *)assy_rev_str,(char *)user_ip_buf);
	// Display entered data
	Rom_msg(NEWPAGE_MSG);
	Display_assy_details();
	
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Display_assy_details(void)
{
	int8 *msg;
	Rom_msg(NEWPAGE_MSG);
	Rom_msg(CHECK_DETAILS_MSG);
	MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Bd_test_start_menu);
	sprintf((char *)tmpstr,"WO No   = %s\n\rAssy No = %s-01-%s  ",wo_no_str,assy_no_str,assy_rev_str);
	ASC_Asci_msg(tmpstr);
	if(adapter_flag)
		msg = ROM_Read_romstr(NET_ADAPT_PCB_MSG);
	else
		msg = ROM_Read_romstr(NET_CONN_PCB_MSG);
	sprintf((char *)tmpstr,"%s\n\n\r",msg);
	ASC_Asci_msg(tmpstr);
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------


static void Bd_test_start_menu(void)
{
	int8 rx_byte;

	// get any RX chars *
	rx_byte = Cmd_check(CMD_ECHO);
	// return if none available
	if(!rx_byte)
		return;

	// now process RX char
	switch(rx_byte)
	{
		case 'X':
		case 'x':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
			break;
		case '\n':
		case '\r':
			user_ip_buf_ix = 0;	//reset user input buf index
			user_ip_max_chars = BD_SN_STRING_LEN;
			MEN_Set_cmd_bk_func(ENTER_SN_MSG,Get_serial_no);
			break;
		default:
			Rom_msg(INVALID_ENTRY_MSG);
			MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Bd_test_start_menu);
			break;
	}
}

//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static const int8 *cur_menu_msg;

static void Get_serial_no(void)
{
	int8 rx_byte;

	// get any RX chars *
	rx_byte = Cmd_check(CMD_ECHO);
	// return if none available 
	if(!rx_byte)
	return;

	// now process RX char 
	switch(rx_byte)
	{
		case 'x':
		case 'X':
			if(user_ip_buf_ix == 0)
				Display_assy_details();
			break;
		case '\n':
		case '\r':
			if(user_ip_buf_ix < user_ip_max_chars)
			{
				Rom_msg(NOT_ENOUGH_ASSY_REV_IP_CHARS_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = BD_SN_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_SN_MSG,Get_serial_no);
			}
			break;
		default:
			if(Check_number_char(rx_byte))
			{
				user_ip_buf[user_ip_buf_ix] = rx_byte;
				if(++user_ip_buf_ix == user_ip_max_chars)
				{
					user_ip_buf[user_ip_buf_ix] = 0; // terminate string
					strcpy((char *)bd_serial_no_str,(char *)user_ip_buf);
					Rom_msg(NEWPAGE_MSG);
					sprintf((char *)tmpstr,"\n\nBoard ID: %s-%s is selected for programming\n\n\r",wo_no_str,bd_serial_no_str);
					ASC_Asci_msg(tmpstr);
					if(debug_flag)
					{
						Prog_debug_details();
						Display_formatted_assy_info();
						MEN_Set_cmd_bk_func(EEPROM_DEBUG_MSG,Eeprom_debug_menu);
					}
					else if(adapter_flag)
						cur_menu_msg = CONNECT_ADAPTER_MSG;
					else
						cur_menu_msg = CONNECT_BD_MSG;

					MEN_Set_cmd_bk_func(cur_menu_msg,Connect_bd_menu);
				}
			}
			else
			{
				Rom_msg(NUMERIC_CHARS_ONLY_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = BD_SN_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_SN_MSG,Get_serial_no);
			}
			break;
	}
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Connect_bd_menu(void)
{
	int8 rx_byte;

	// get any RX chars *
	rx_byte = Cmd_check(CMD_ECHO);
	// return if none available
	if(!rx_byte)
	return;

	// now process RX char
	switch(rx_byte)
	{
		case 'x':
		case 'X':
			Display_assy_details();
			break;
		case '\n':
		case '\r':
			MEN_Set_cmd_bk_func(PROG_EEPROM_MSG,Start_eeprom_prog);
			break;
		default:
			Rom_msg(PRESS_X_OR_PROCEED_MSG);
			MEN_Set_cmd_bk_func(cur_menu_msg,Connect_bd_menu);
			break;
	}
	
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Start_eeprom_prog(void)
{
	int8 rx_byte;

	Rom_msg(NEWPAGE_MSG);
	sprintf((char *)tmpstr,"Programming Board %s-%s\n\r",wo_no_str,bd_serial_no_str);
	ASC_Asci_msg(tmpstr);
	// Turn Power on
	MAI_Set_power(ON);
	// Add delay to allow voltages to settle
	TIM_Set_delay(1000);	
	while(!TIM_Get_delay_flag());
	
	if(!adapter_flag)
	{
		
		// Do Header Check
		while(((HEADER_PORT_RD & HEADER_3V3_BIT) == 0) || ((HEADER_PORT_RD & HEADER_CONFIG_BIT) != 0))
		{
			Rom_msg(CHECK_HEADER_MSG);	// display error message
			sprintf((char *)tmpstr,"Header Port = %02x\n\r",((int16)HEADER_PORT_RD & 0x06));
			ASC_Asci_msg(tmpstr);
			// wait for user input
			rx_byte  = Wait_for_x_or_enter();
			if(rx_byte == 'x')
			{
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = BD_SN_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_SN_MSG,Get_serial_no);
				MAI_Set_power(OFF);
				return;
			}
		}
	}
	// Header is ok so set config as output low
	//****************************************
	MAI_Set_header_cntrl(OP,LO);
	cur_i2C_addr = EEPROM_ADDR_LO;

	cur_calc_checksum = Calc_stored_checksum();
	cur_stored_checksum = Get_stored_checksum();
//	sprintf(tmpstr,"\n\rCalc = %04x\n\rStored = %04x\n\n\r",cur_calc_checksum,cur_stored_checksum);
//	ASC_Asci_msg(tmpstr);


	// get, calc and display checksum status
	if(cur_calc_checksum != cur_stored_checksum)
	{
		Rom_msg(CHECKSUM_MISMATCH_MSG);	// display error message
	}
	else
	{
		Rom_msg(CHECKSUM_MATCH_MSG);		// display match message and prompt for proceed
		Display_formatted_assy_info();
		Rom_msg(NEWLINE_MSG);		// display match message and prompt for proceed
		Rom_msg(PRESS_X_OR_PROCEED_MSG);	// display prompt for proceed

		rx_byte  = Wait_for_x_or_enter();
		if(rx_byte == 'x')
		{
			user_ip_buf_ix = 0;	//reset user input buf index
			user_ip_max_chars = BD_SN_STRING_LEN;
			MEN_Set_cmd_bk_func(ENTER_SN_MSG,Get_serial_no);
			MAI_Set_power(OFF);
			return;
		}			
	}

	//Now set all EEPROm to 0's and store user data
	Eeprom_fill(EEPROM_RESET_CHAR); // Reset all data

	Prog_debug_details();

	// Now turn Power off for 3 secs
	Rom_msg(CHECKING_DATA_RETENTION_MSG);
	MAI_Set_power(OFF);
	TIM_Set_delay(3000);
	while(!TIM_Get_delay_flag());
	MAI_Set_power(ON);

	if(!adapter_flag)
	{
		//Set upper address and check data
		MAI_Set_header_cntrl(OP,HI);
		cur_i2C_addr = EEPROM_ADDR_HI;
	}
	if(Verify_stored_data())
	{
		Rom_msg(DATA_RETENTION_OK_MSG);
		Display_formatted_assy_info();
		Rom_msg(PROG_SUCCESS_MSG);
	}
	else
	{
		Rom_msg(DATA_RETENTION_BAD_MSG);
		//Eeprom_hex_dump();
	}
	
	if(adapter_flag)
	{
		while(!Port_expander_test())
		{
			Rom_msg(PORT_EXPANDER_TEST_FAILED_MSG);
			rx_byte  = Wait_for_x_or_enter();
			if(rx_byte == 'x')
			{
				Rom_msg(NEWPAGE_MSG);
				Rom_msg(BD_TEST_FAIL_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = BD_SN_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_SN_MSG,Get_serial_no);
				MAI_Set_power(OFF);
				return;
			}
			Rom_msg(NEWPAGE_MSG);
			
		}
	}	

	MAI_Set_power(OFF);
	MAI_Set_header_cntrl(IP,LO);

	Rom_msg(NEWPAGE_MSG);
	Rom_msg(BD_TEST_SUCCESS_MSG);

	// Set Params for SN
	user_ip_buf_ix = 0;	//reset user input buf index
	user_ip_max_chars = BD_SN_STRING_LEN;

	MEN_Set_cmd_bk_func(ENTER_NEXT_SN_MSG,Get_serial_no);

}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static int8 Wait_for_x_or_enter(void)
{
	int8 rx_byte;

	while (ASC_Asci_getchar(&rx_byte) != ASCI_EMPTY);

	do
	{
		rx_byte = Cmd_check(CMD_ECHO);
		// return if none available
		switch (rx_byte)
		{
			case 'x':
			case 'X':
				return 'x';
				break;
			case '\n':
			case '\r':
				return '\n';
				break;
			case 0:
				break;
			default:
				Rom_msg(PRESS_X_OR_PROCEED_MSG);
				rx_byte = 0;
				break;
		}
	} while (!rx_byte);
		

	return '\n';

}
//********************************************************************
//********************************************************************


//********************************************************************
//********************************************************************



//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------


static void Debug_menu(void)
{
	int8 rx_byte;
	
	MAI_Set_power(ON);
	/* get rx char */
	rx_byte = Cmd_check(CMD_ECHO);
	/* check for valid rx char */
	if(!rx_byte)
	return;
	/* process rx char */
	switch(rx_byte)
	{
		case '1':
			cur_i2C_addr = NET_CONN_I2C_ADDR;
			break;
		case '2':
			cur_i2C_addr = ADC_BOT_I2C_ADDR;
			break;
		case '3':
			cur_i2C_addr = NET_EXTEND_I2C_ADDR;
			break;
		case '4':
			cur_i2C_addr = ADC_BOT_I2C_ADDR;
			break;
		case '5':
			cur_i2C_addr = PORT_EXPAND_I2C_ADDR;
			if(I2C_ping_addr(cur_i2C_addr))
			{
				Init_pe_debug_menu();
				return;
			}
			break;
		case 'x':
		case 'X':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
			MAI_Set_power(OFF);

			return;	
		default:
			Rom_msg(CMD_NOT_IMPLEMENTED_MSG);
			MEN_Set_cmd_bk_func(DEBUG_MENU_MSG,Debug_menu);
			return;
	}
		
	if(!I2C_ping_addr(cur_i2C_addr))
	{
		Rom_msg(ASSY_NOT_DETECTED_MSG);
		MEN_Set_cmd_bk_func(START_MENU_MSG,Start_menu);
		MAI_Set_power(OFF);	
	}
	else
		MEN_Set_cmd_bk_func(EEPROM_DEBUG_MSG,Eeprom_debug_menu);

}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Eeprom_debug_menu(void)
{
	int8 rx_byte, buf[EEPROM_PAGE_SIZE+2];
	int16 storedsum,calcsum;
	
		/* get rx char */
	rx_byte = Cmd_check(CMD_ECHO);
	/* check for valid rx char */
	if(!rx_byte)
	return;
	/* process rx char */
	switch(rx_byte)
	{

		case 'D':
		case 'd':
		Display_formatted_assy_info();
/*		Eeprom_read()
		ptr = SPI_Nvr_read(&buf[0],0,18);
		ptr[EEPROM_PAGE_SIZE-1] = '\0';
		sprintf((char *)tmpstr,"\n\r%s\n\r",ptr);
		ASC_Asci_msg(tmpstr);*/
		break;
	
		//Check for Hex Dump CMD
		case 'H':
		case 'h':
			Eeprom_hex_dump();
			break;
		//Check for Calc Checksum CMD
		case 'C':
		case 'c':
			calcsum = Calc_stored_checksum();
			storedsum = Get_stored_checksum();
			while(!ASC_Asci_tx_empty());
			sprintf((char *)tmpstr,"\n\rCalculated Checksum = %04x\n\rStored Checksum     = %04x\n\r",calcsum,storedsum);
			ASC_Asci_msg(tmpstr);
			break;
		
		case 'W':
		case 'w':
			// do lower area of EEPROM (0x000 - 0x0ff)
		//	buf[0] = (int8)(BYTE_WRITE_ADDR_LO & 0xff);
			buf[0] = WRITE_CHAR_LOW;

			Eeprom_write(cur_i2C_addr,BYTE_WRITE_ADDR_LO,1,buf);		
			while(!ASC_Asci_tx_empty());
			sprintf((char *)tmpstr,"\n\rData %02x written to address %04x\n\r",(int16)buf[0],(int16)BYTE_WRITE_ADDR_LO);
			ASC_Asci_msg(tmpstr);

			Eeprom_read(cur_i2C_addr,BYTE_WRITE_ADDR_LO,1,buf);	//read 1 byte of data from EEPROM

			while(!ASC_Asci_tx_empty());
			sprintf((char *)tmpstr,"\n\rData %02x read from address %04x\n\r",(int16)buf[0],(int16)BYTE_WRITE_ADDR_LO);
			ASC_Asci_msg(tmpstr);

			// do upper area of EEPROM (0x100 - 0x1ff)
	//		buf[0] = (int8)(BYTE_WRITE_ADDR_HI & 0xff);
			buf[0] = WRITE_CHAR_HIGH;

			Eeprom_write(cur_i2C_addr,BYTE_WRITE_ADDR_HI,1,buf);
			while(!ASC_Asci_tx_empty());
			sprintf((char *)tmpstr,"\n\rData %02x written to address %04x\n\r",(int16)buf[0],(int16)BYTE_WRITE_ADDR_HI);
			ASC_Asci_msg(tmpstr);

			Eeprom_read(cur_i2C_addr,BYTE_WRITE_ADDR_HI,1,buf);	//read 1 byte of data from EEPROM

			while(!ASC_Asci_tx_empty());
			sprintf((char *)tmpstr,"\n\rData %02x read from address %04x\n\r",(int16)buf[0],(int16)BYTE_WRITE_ADDR_HI);
			ASC_Asci_msg(tmpstr);
	
			break;

		case 's':
		case 'S':
			Eeprom_fill(EEPROM_SET_CHAR);
			break;
		case 'R':
		case 'r':
			Eeprom_fill(EEPROM_RESET_CHAR);
			break;

		case 'P':
		case 'p':
			user_ip_buf_ix = 0;	//reset user input buf index
			user_ip_max_chars = MAX_WO_STRING_LEN;
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(SELECT_BD_MSG,Select_bd_menu);
			return;
			
		case 'X':
		case 'x':
			MEN_Set_cmd_bk_func(DEBUG_MENU_MSG,Debug_menu);
			MAI_Set_power(OFF);
			return;
			break;
		default:
			Rom_msg(CMD_NOT_IMPLEMENTED_MSG);
			break;
		
	}
	MEN_Set_cmd_bk_func(EEPROM_DEBUG_MSG,Eeprom_debug_menu);
 

}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

#define EEPROM_RETRY_COUNT 1000

static int8 Eeprom_read(int8 eeprom_addr, int16 byte_addr,int16 byte_count,int8 *data)
{
	int8 cur_addr,x, buf[EEPROM_PAGE_SIZE+1] ;
	int16 n;
	
	// set D8 in address according to memory location to access
	if(byte_addr >= 0x100)
	{
		cur_addr = eeprom_addr+1;
	}
	else
	{
		cur_addr = eeprom_addr;
	}
	buf[0] = (int8)(byte_addr & 0xff);

	// Fist set read address by performing a dummy write
	for( n = 0; n < EEPROM_RETRY_COUNT;n++) 
	{
		if(I2C_Write(cur_addr,1,buf) == I2C_VALID_ADDR)
			break;
	}
	// Check for error condition
	if(n == EEPROM_RETRY_COUNT)
	{	
		Rom_msg(EEPROM_READ_ERROR_MSG);
		return FALSE;
	}
	
	I2C_Read(cur_addr,byte_count,&buf[1]);	//read x bytes of data from EEPROM
	for(x = 0; x < byte_count;x++)
		data[x] = buf[x+1];
	
	return TRUE;
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------
#define EE_WR_TIMEOUT 0xff

static int8 Eeprom_write(int8 eeprom_addr,int16 byte_addr, int16 byte_count,int8 *data)
{
	int8 cur_addr,x, buf[EEPROM_PAGE_SIZE+1] ;
	int16 n;
	
	for(x = 0; x < byte_count;x++)
	{
		// set D8 in address according to memory location to access	
		if(byte_addr >= 0x100)
		{
			cur_addr = eeprom_addr+1;
		}
		else
		{
			cur_addr = eeprom_addr;
		}
		buf[0] = (int8)(byte_addr & 0xff); // set byte addr and post increment for next byte write
		buf[1] = data[x];

		// Fist set write address by performing a dummy write
		I2C_Write(cur_addr,1,buf); 
		// EEPROM is ready so write address(buf[0]) and 1 byte of data (buf[1])
		I2C_Write(cur_addr,2,buf);	//write 1 byte of data to EEPROM
		for(n = 0; n < EE_WR_TIMEOUT; n++)
		{
			if((I2C_Write(cur_addr,1,buf)) == I2C_VALID_ADDR) // keep trying until the EEPROM  is not busy writing
			{
				byte_addr++;
				break;
			}
		}
		if( n == EE_WR_TIMEOUT)
		{
			ASC_Asci_msg((int8 *)"\n\n\r!!! EEPROM Write Error !!!\n\n\r");
			return FALSE;
		}
	}
	return TRUE;
}

//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Eeprom_fill(int8 set_char)
{
	int8 buf;
	int16 x,pc_complete;
	
	buf = set_char;
			
	for(x = 0; x < EEPROM_BYTE_COUNT; x++)
	{
		pc_complete = ((x * 100)/EEPROM_BYTE_COUNT);
		while(!ASC_Asci_tx_empty());
		sprintf((char *)tmpstr,"\rResetting All of EEPROM to %02x - %03d%% done",(int16)buf,pc_complete+1);
		ASC_Asci_msg(tmpstr);
				
		// Write char to EEPROM
		Eeprom_write(cur_i2C_addr,x,1,&buf);
				
		// Now check data has been set
		Eeprom_read(cur_i2C_addr,x,1,&buf);	//read 1 byte of data from EEPROM

		if(buf != set_char)
		{
			while(!ASC_Asci_tx_empty());
			sprintf((char *)tmpstr,"\n\rErase ERR %x\n\r",(int16)buf);
			ASC_Asci_msg(tmpstr);
			break;
		}
	

	}
	while(!ASC_Asci_tx_empty());
}

//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Eeprom_hex_dump(void)
{
	int8 n,buf[EEPROM_PAGE_SIZE+2];
	int16 x;

	ASC_Asci_msg((int8 *)"\n\n\rHEX DUMP\n\r\n");
	buf[0] = 0x00;	//set initial read byte addr to 0

	for(x = 0;x < EEPROM_BYTE_COUNT;x += EEPROM_PAGE_SIZE)
	{
		sprintf((char *)tmpstr,"Addr = %04x: ",x);
		ASC_Asci_msg(tmpstr);
				
		Eeprom_read(cur_i2C_addr,x,EEPROM_PAGE_SIZE,buf);

		for(n = 0; n < EEPROM_PAGE_SIZE; n++)
		{
			sprintf((char *)&tmpstr[(n*3)],"%02x ",buf[n]);
		}
		while(!ASC_Asci_tx_empty());
		ASC_Asci_msg(tmpstr);
		ASC_Asci_msg((int8 *)"\n\r");
	}
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static int16 Get_stored_checksum(void)
{
	int8  buf[EEPROM_CHECKSUM_LAYOUT_SIZE+1];
	int16 checksum;
	
	Eeprom_read(cur_i2C_addr, EEPROM_CHECKSUM_LAYOUT_POS,EEPROM_CHECKSUM_LAYOUT_SIZE, buf);

	checksum = ((int16)buf[0] << 8) & 0xff00;
	checksum |= (int16)buf[1] & 0x00ff;

//	sprintf((char *)tmpstr,"\n\rAddr = %04x:\n\rPos = %04x\n\rData size = %04x\n\rData = %02x,%02x\n\rChecksum = %04x\n\n\r",cur_i2C_addr,EEPROM_CHECKSUM_LAYOUT_POS,EEPROM_CHECKSUM_LAYOUT_SIZE,(int16)ptr[0],(int16)ptr[1],checksum);
//	ASC_Asci_msg(tmpstr);


	return checksum;
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Store_checksum(int16 checksum)
{
	int8 buf[EEPROM_CHECKSUM_LAYOUT_SIZE+1];
	
	buf[0] = (int8)(checksum >> 8) & 0xff;
	buf[1] = (int8)(checksum & 0x00ff);
	
//	sprintf(tmpstr,"\n\r** Storing %04x, %02x, %02x **\n\r", checksum, (int16)buf[0], (int16)buf[1]);
//	ASC_Asci_msg(tmpstr);
	Eeprom_write(cur_i2C_addr, EEPROM_CHECKSUM_LAYOUT_POS,EEPROM_CHECKSUM_LAYOUT_SIZE, buf);
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static int16 Calc_stored_checksum(void)
{
	int8 n,buf[EEPROM_PAGE_SIZE+2];
	int16 x, checksum, stored_sum;

	checksum = 0;
	buf[0] = 0x00;	//set initial read byte addr to 0

	// add all bytes in eeprom to sum
	for(x = 0;x < EEPROM_BYTE_COUNT;x += EEPROM_PAGE_SIZE)
	{
		Eeprom_read(cur_i2C_addr,x,EEPROM_PAGE_SIZE,buf);

		for(n = 0; n < EEPROM_PAGE_SIZE; n++)
			checksum += (int16)buf[n];
	}
	stored_sum = Get_stored_checksum();
	checksum -= (stored_sum & 0x00ff);
	checksum -= ((stored_sum >> 8) & 0x00ff);
	checksum = ~checksum;
	return checksum;
}
///====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------
static int8 Get_stored_digit_string(int16 eeprom_pos, int16 size, int8*buf)
{
	int8 x;
	
	if(!Eeprom_read(cur_i2C_addr,eeprom_pos,size,buf))
		return FALSE;
	for(x = 0; x < size;x++)
	{
		if(!Check_number_char(buf[x]))
		{
			buf[x] = '\0';	//terminate string
			return FALSE;
		}
	}
	buf[x] = '\0';	//terminate string
	return TRUE;
}
static int8 Check_number_char(int8 c)
{
	return ((c >= 0x30) && (c <= 0x39));
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------
static int8 Get_stored_alpha_string(int16 eeprom_pos, int16 size, int8*buf)
{
	int8 x;
	
	Eeprom_read(cur_i2C_addr,eeprom_pos,size,buf);
	for(x = 0; x < size;x++)
	{
		
		if((buf[x] == '\0') && (x > 0))
			return TRUE;
		else if (!Check_alpha_char(buf[x]))
		{
			buf[x] = '\0';	//terminate string
			return FALSE;
		}	
	}
	buf[x] = '\0';	//terminate string
	return TRUE;
}
static int8 Check_alpha_char(int8 c)
{
	return ( ((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '-') );
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void	Display_formatted_assy_info(void)
{
	int8 *ptr, *ptr1, data_set_flag, tmp[EEPROM_BD_ASSY_SN_STRING_LAYOUT_SIZE];
	
	// Now display contents and checksum & status
	//*******************************************
	Rom_msg(NEWLINE_MSG);	// newline
	Rom_msg(STORED_DETAILS_MSG);
	cur_stored_checksum = Get_stored_checksum();
	sprintf((char *)tmpstr,"Stored Checksum     = %04x\n\r",cur_stored_checksum);
	ASC_Asci_msg(tmpstr);

	cur_calc_checksum = Calc_stored_checksum();
	sprintf((char *)tmpstr,"Calculated Checksum = %04x\n\r",cur_calc_checksum);
	ASC_Asci_msg(tmpstr);
	Rom_msg(NEWLINE_MSG);	// newline
	
	//Now display Product No.
	while(!ASC_Asci_tx_empty());
	Rom_msg(PRODUCT_PN_MSG);	// display Product Code message
	ptr = (int8 *)ROM_Read_romstr(DATA_NOT_SET_MSG);
	data_set_flag = FALSE;
	if(Get_stored_digit_string(EEPROM_FINAL_PROD_NUM_LAYOUT_POS,EEPROM_FINAL_PROD_NUM_LAYOUT_SIZE,tmpstr))
	{
		ptr1 = tmpstr + EEPROM_FINAL_PROD_NUM_LAYOUT_SIZE;
		*ptr1++ = '-';
		if(Get_stored_digit_string(EEPROM_FINAL_PROD_CODE_LAYOUT_POS,EEPROM_FINAL_PROD_CODE_LAYOUT_SIZE,ptr1))
		{
			ptr1 += EEPROM_FINAL_PROD_CODE_LAYOUT_SIZE;
			*ptr1++ = '-';
			if(Get_stored_alpha_string(EEPROM_FINAL_PROD_REV_LAYOUT_POS,EEPROM_FINAL_PROD_REV_LAYOUT_SIZE,ptr1))
			{
				*(ptr1 + EEPROM_FINAL_PROD_REV_LAYOUT_SIZE) = '\0';
				ptr = tmpstr;
				data_set_flag = TRUE;
			}
		}
	}
	ASC_Asci_msg(ptr);
	Rom_msg(NEWLINE_MSG);	// Newline

	if(data_set_flag)
	{	
		// Display Net ID
		Eeprom_read(cur_i2C_addr,EEPROM_FINAL_PROD_NETID_LAYOUT_POS,EEPROM_FINAL_PROD_NETID_LAYOUT_SIZE,tmp);
		if(*tmp == 128)
			ptr = ROM_Read_romstr(ADAPTER_ASSY_MSG);
		else if(extender_flag)
			ptr = ROM_Read_romstr(EXTENSION_ASSY_MSG);
		else
			ptr = ROM_Read_romstr(net_id_list[net_id]);
	}
	else
		ptr = (int8 *)ROM_Read_romstr(DATA_NOT_SET_MSG);

	sprintf((char *)tmpstr,"NET ID       = %s\n\r",ptr);
	ASC_Asci_msg(tmpstr);

	//Now display Product S/N.
	while(!ASC_Asci_tx_empty());
	Rom_msg(PRODUCT_SN_MSG);	// display Product SN message
	ptr = (int8 *)ROM_Read_romstr(DATA_NOT_SET_MSG);
	if(Get_stored_alpha_string(EEPROM_FINAL_PROD_SN_LAYOUT_POS,EEPROM_FINAL_PROD_SN_LAYOUT_SIZE,tmpstr))
	{
		ptr1 = tmpstr + EEPROM_FINAL_PROD_SN_LAYOUT_SIZE;
		*ptr1++ = '-';
		if(Get_stored_alpha_string(EEPROM_FINAL_PROD_CODE_LAYOUT_POS,EEPROM_FINAL_PROD_SN_LAYOUT_SIZE,ptr1))
		{
			*(ptr1 + EEPROM_FINAL_PROD_SN_LAYOUT_SIZE) = '\0';
			ptr = tmpstr;
		}
	}
	ASC_Asci_msg(ptr);
	Rom_msg(NEWLINE_MSG);	// newline

	//Now display PCB Assy No.
	while(!ASC_Asci_tx_empty());
	Rom_msg(PCB_ASSY_PN_MSG);	// display PCB assy PN message
	ptr = (int8 *)ROM_Read_romstr(DATA_NOT_SET_MSG);
	if(Get_stored_alpha_string(EEPROM_ASSY_NUM_STRING_LAYOUT_POS,EEPROM_ASSY_NUM_STRING_LAYOUT_SIZE,tmpstr))
		ptr = tmpstr;
		
	ASC_Asci_msg(ptr);
	Rom_msg(NEWLINE_MSG);	// newline
	
	//Now display PCB Assy SN.
	while(!ASC_Asci_tx_empty());
	Rom_msg(PCB_ASSY_SN_MSG);	// display PCB assy SN message
	ptr = (int8 *)ROM_Read_romstr(DATA_NOT_SET_MSG);
	if(Get_stored_alpha_string(EEPROM_ASSY_WO_STRING_LAYOUT_POS,EEPROM_ASSY_WO_STRING_LAYOUT_SIZE,tmpstr))
	{
		strcat((char *)tmpstr,"-");
	//	ptr = tmpstr;
	//	ptr1 = tmpstr + EEPROM_ASSY_WO_STRING_LAYOUT_SIZE;
	//	*ptr1++ = '-';
		if(Get_stored_alpha_string(EEPROM_ASSY_SN_STRING_LAYOUT_POS,EEPROM_BD_ASSY_SN_STRING_LAYOUT_SIZE,tmp))
		{
			strcat((char *)tmpstr,(char *)tmp);
			ptr = tmpstr;
		}
	}
//	ptr = tmpstr;
	ASC_Asci_msg(ptr);
	Rom_msg(NEWLINE_MSG);	// newline

}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static int8 Verify_stored_data(void)
{
	if(Get_stored_alpha_string(EEPROM_ASSY_NUM_STRING_LAYOUT_POS,EEPROM_ASSY_NUM_STRING_LAYOUT_SIZE,tmpstr))
	{
		if(!strcmp((char *)tmpstr,(char *)formatted_assy_no_string))
		{
			if(Get_stored_alpha_string(EEPROM_ASSY_WO_STRING_LAYOUT_POS,EEPROM_ASSY_WO_STRING_LAYOUT_SIZE,tmpstr))
			{
				if(!strcmp((char *)tmpstr,(char *)wo_no_str))
				{
					if(Get_stored_alpha_string(EEPROM_ASSY_SN_STRING_LAYOUT_POS,EEPROM_BD_ASSY_SN_STRING_LAYOUT_SIZE,tmpstr))
					{
						if(!strcmp((char *)tmpstr,(char *)bd_serial_no_str))
						{
							cur_calc_checksum = Calc_stored_checksum();
							cur_stored_checksum = Get_stored_checksum();
							if(cur_stored_checksum == cur_calc_checksum)
								return TRUE;	
						}
						
					}
				}
			}
		}
	}
	return FALSE;
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Prog_debug_details(void)
{
	ASC_Asci_msg((int8 *)"\n\n\rUpdating EEPROM !!\n\n\r");
	sprintf((char *)formatted_assy_no_string,"%s-01-%s",assy_no_str, assy_rev_str);
	Eeprom_write(cur_i2C_addr, EEPROM_ASSY_NUM_STRING_LAYOUT_POS,EEPROM_ASSY_NUM_STRING_LAYOUT_SIZE,formatted_assy_no_string ); // Store PCB assy number, code and rev
	//	Eeprom_hex_dump();
	Eeprom_write(cur_i2C_addr, EEPROM_ASSY_WO_STRING_LAYOUT_POS,EEPROM_ASSY_WO_STRING_LAYOUT_SIZE,wo_no_str ); // Store PCB assy number, code and rev
	//	Eeprom_hex_dump();
	Eeprom_write(cur_i2C_addr, EEPROM_ASSY_SN_STRING_LAYOUT_POS,EEPROM_BD_ASSY_SN_STRING_LAYOUT_SIZE,bd_serial_no_str ); // Store PCB assy number, code and rev

	cur_calc_checksum = Calc_stored_checksum();
	Store_checksum(cur_calc_checksum);

//	Display_formatted_assy_info();

//	MEN_Set_cmd_bk_func(EEPROM_DEBUG_MSG,Eeprom_debug_menu);

}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

#define PE_CODE_COUNT 10
#define SET_PE_ALL_IP_CMD 0x00

static void Init_pe_debug_menu(void)
{
	int8 cur_pe_cmd = SET_PE_ALL_IP_CMD;
	
	cur_i2C_addr = PORT_EXPAND_I2C_ADDR;
	I2C_Write(cur_i2C_addr,1,&cur_pe_cmd);
	MEN_Set_cmd_bk_func(PORT_EXPANDER_DEBUG_MSG,Port_expander_debug_menu);
	TIM_Set_delay(200);
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------


static int8 pe_code = 0;
static int8 pe_count = 0;
static int8 pe_codes[PE_CODE_COUNT] = {0x00,0x01,0x02,0x04,0x08,0x0f,0x0e,0x0d,0x0b,0x07};

static void Port_expander_debug_menu(void)
{
	int8 rx_byte, pe_data;
	
	/* get rx char */
	rx_byte = Cmd_check(CMD_ECHO);
	/* check for valid rx char */
	if(rx_byte)
	{
		// process rx char 
		switch(rx_byte)
		{
			case 'X':
			case 'x':
				// Set all OPs low as 5V tolerant inputs to Port Expander hold up 3V3 power lines if left high
				MAI_Set_port_expander_code(0);
				MEN_Set_cmd_bk_func(DEBUG_MENU_MSG,Debug_menu);
				return;
			default:
				break;
		}
		
	}
	if(TIM_Get_delay_flag())
	{
		pe_code = pe_codes[pe_count];
		MAI_Set_port_expander_code((pe_code & 0x0f));
		TIM_Set_delay(3000);
		I2C_Read(cur_i2C_addr,1,&pe_data);
		//pe_data = pe_code; // Test Code
		sprintf((char *)tmpstr," %01x    %01x    %01x    %01x     - Data Read = %01x%01x%01x%01x\r",((pe_code >> 3) & 0x01),((pe_code >> 2) & 0x01),((pe_code >> 1) & 0x01),(pe_code & 0x01),((pe_data>>3) & 0x01), ((pe_data>>2) & 0x01), ((pe_data>>1) & 0x01), (pe_data & 0x01));
		ASC_Asci_msg(tmpstr);
		if(++pe_count >= PE_CODE_COUNT)
			pe_count = 0;
	}
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static int8 Port_expander_test(void)
{
	int8 pe_data,cur_pe_cmd = SET_PE_ALL_IP_CMD;
	int8 pass_flag = TRUE;
	char *pf_msg;
	
	Rom_msg(PORT_EXPANDER_TEST_MSG);	// display PCB assy PN message
	
	cur_i2C_addr = PORT_EXPAND_I2C_ADDR;
	I2C_Write(cur_i2C_addr,1,&cur_pe_cmd);	
	TIM_Wait(200);
	for(pe_count = 0;pe_count < PE_CODE_COUNT; pe_count++)
	{
		//Set output
		pe_code = pe_codes[pe_count];
		MAI_Set_port_expander_code((pe_code & 0x0f));
		TIM_Wait(100);
		I2C_Read(cur_i2C_addr,1,&pe_data);
		pe_data = pe_data & 0x0f;
		if(pe_data != pe_code)
		{
			pass_flag = FALSE;
			pf_msg = "FAIL";
		}
		else
			pf_msg = "PASS";
		
		// Set all OPs low as 5V tolerant inputs to Port Expander hold up 3V3 power lines if left high
		MAI_Set_port_expander_code(0);

		sprintf((char *)tmpstr,"%01x%01x%01x%01x  ",((pe_code >> 3) & 0x01),((pe_code >> 2) & 0x01),((pe_code >> 1) & 0x01),(pe_code & 0x01));
		ASC_Asci_msg(tmpstr);
		sprintf((char *)tmpstr,"%01x%01x%01x%01x - %s\n\r",((pe_data>>3) & 0x01), ((pe_data>>2) & 0x01), ((pe_data>>1) & 0x01), (pe_data & 0x01),(char*)pf_msg);
		ASC_Asci_msg(tmpstr);
	}
	return pass_flag;
}
/*====================================================================
	FINAL TEST Funcs
====================================================================*/

//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Fstart_menu(void)
{
	int8 rx_byte,*msg;

	// get any RX chars 
	rx_byte = Cmd_check(CMD_NO_ECHO);
	// return if none available 
	if(!rx_byte)
		return;

	/* now process RX char */
	switch(rx_byte)
	{
		case '1':
			final_assy_type = NET_ASSY;
			final_assy_name = NET_ASSY_MSG;
			adapter_flag = FALSE;
			extender_flag = FALSE;
			MAI_Set_header_cntrl(OP,LO);
			cur_i2C_addr = EEPROM_ADDR_LO;
			break;
		case '2':
			final_assy_type = EXTENSION_ASSY;
			final_assy_name = EXTENSION_ASSY_MSG;
			adapter_flag = FALSE;
			extender_flag = TRUE;
			MAI_Set_header_cntrl(OP,HI);
			cur_i2C_addr = EEPROM_ADDR_HI;
			break;
		case '3':
			final_assy_type = ADAPTER_ASSY;
			final_assy_name = ADAPTER_ASSY_MSG;
			adapter_flag = TRUE;
			extender_flag = FALSE;
			MAI_Set_header_cntrl(OP,LO);
			cur_i2C_addr = EEPROM_ADDR_LO;
			break;
		case 'V':
		case 'v':
			Rom_msg(NEWPAGE_MSG);
			MAI_Set_power(ON);
			TIM_Wait(100);
			cur_i2C_addr = EEPROM_ADDR_LO;
			msg = ROM_Read_romstr(ASSY_TYPE_CONN_ADAPTER_MSG);
			if(!I2C_ping_addr(cur_i2C_addr))
			{
				cur_i2C_addr = EEPROM_ADDR_HI;	// no response so set Hi addr
				if(I2C_ping_addr(cur_i2C_addr))
				{
					msg = ROM_Read_romstr(ASSY_TYPE_EXTENDER_MSG);
					extender_flag = TRUE;
				}
				else
				{
					Rom_msg(ASSY_NOT_DETECTED_MSG);
					MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
					MAI_Set_power(OFF);
					return;
				}
			}
			ASC_Asci_msg(msg);
			Display_formatted_assy_info();
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			MAI_Set_power(OFF);
			return;
		case 'D':
			user_ip_buf_ix = 0;
			MEN_Set_cmd_bk_func(NULL,Get_secret_access_code);
			return;	
		default:
			Rom_msg(INVALID_ENTRY_MSG);
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			return;
	}
		
	user_ip_buf_ix = 0;
	user_ip_max_chars = ASSY_STRING_LEN;
	MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_NO_MSG,Get_final_assy_no);
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Get_secret_access_code(void)
{
	int8 rx_byte, *access_code;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_NO_ECHO);
	/* return if none available */
	if(!rx_byte)
		return;

	access_code = ROM_Read_romstr(ACCESS_CODE);
	
	if(Check_number_char(rx_byte))
	{
		if(access_code[user_ip_buf_ix] == rx_byte)
		{
			if(++user_ip_buf_ix < ACCESS_CODE_LEN)
				return;
			else
			{
				MEN_Set_cmd_bk_func(DEBUG_MENU_MSG,Debug_menu);
				return;
			}
		}
	}
	MEN_Set_cmd_bk_func(NULL,Fstart_menu);

}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

static void Get_final_assy_no(void)
{
	int8 rx_byte;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
		return;

	/* now process RX char */
	switch(rx_byte)
	{
		case 'x':
		case 'X':
			if(user_ip_buf_ix == 0)
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			return;
		case '\n':
		case '\r':
			Rom_msg(NOT_ENOUGH_ASSY_NO_IP_CHARS_MSG);
			user_ip_buf_ix = 0;	//reset user input buf index
			user_ip_max_chars = ASSY_STRING_LEN;
			MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_NO_MSG,Get_final_assy_no);
			break;
		default:
		if(Check_number_char(rx_byte))
		{
			user_ip_buf[user_ip_buf_ix] = rx_byte;
			user_ip_buf_ix++;
			if(user_ip_buf_ix >= user_ip_max_chars)
			{
				user_ip_buf[user_ip_buf_ix] = 0; // terminate string
				strcpy((char *)final_assy_no_str,(char *)user_ip_buf);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = ASSY_REV_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_REV_MSG,Get_final_assy_rev_no);
			}
		}
		else
		{
			Rom_msg(NUMERIC_CHARS_ONLY_MSG);
			user_ip_buf_ix = 0;							//reset user input buf index
			user_ip_max_chars = ASSY_STRING_LEN; // Set max chars for assy
			MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_NO_MSG,Get_final_assy_no);
		}
		break;
	}
}

//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------
static void Get_final_assy_rev_no(void)
{
	int8 rx_byte;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
	return;

	/* now process RX char */
	switch(rx_byte)
	{
		case 'x':
		case 'X':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			return;
		case '\n':
		case '\r':
			if(user_ip_buf_ix < user_ip_max_chars)
			{
				Rom_msg(NOT_ENOUGH_ASSY_REV_IP_CHARS_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = ASSY_REV_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_REV_MSG,Get_final_assy_rev_no);
				return;
			}
			break;
		default:
			if(Check_alpha_char(rx_byte))
			{
				user_ip_buf[user_ip_buf_ix] = rx_byte;
				if(++user_ip_buf_ix < user_ip_max_chars)
					return;
			}
			else
			{
				Rom_msg(ALPHANUMERIC_CHARS_ONLY_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = ASSY_REV_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_REV_MSG,Get_final_assy_rev_no);
				return;

			}
			break;
	}
	user_ip_buf[user_ip_buf_ix] = 0; // terminate string
	strcpy((char *)final_assy_rev_str,(char *)user_ip_buf);
	// Display entered data
	Rom_msg(NEWPAGE_MSG);
	final_assy_serial_no_str[0] = '\0'; //reset SN
	if(adapter_flag || extender_flag)
	{
		if(adapter_flag)	
			net_id = 128;
		else
			net_id = 0;

		Display_final_assy_details();
		MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Final_test_start_menu);
		return;

	}
	MEN_Set_cmd_bk_func(SELECT_NET_ID_MSG,Net_id_menu);
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------
static void Net_id_menu(void)
{
	int8 rx_byte;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
		return;

	if(rx_byte >= 0x30 && rx_byte <= 0x39)
		net_id = rx_byte - 0x30;
	else if(rx_byte == 'a' || rx_byte == 'A')
		net_id = 0x0a;
	else
	{
		Rom_msg(INVALID_ENTRY_MSG);
		MEN_Set_cmd_bk_func(SELECT_NET_ID_MSG,Net_id_menu);
		return;
	}		
			
	Display_final_assy_details();
	MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Final_test_start_menu);
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------
static void Display_final_assy_details(void)
{
//	Rom_msg(NEWPAGE_MSG);
	Rom_msg(CHECK_DETAILS_MSG);
	sprintf((char *)tmpstr,"Final Assembly Type: %s\n\r",ROM_Read_romstr(final_assy_name));
	ASC_Asci_msg(tmpstr);
	sprintf((char *)tmpstr,"Final Assembly Number: %s-00-%s\n\r",final_assy_no_str,final_assy_rev_str);
	ASC_Asci_msg(tmpstr);
	if(adapter_flag)
		sprintf((char *)tmpstr,"Net ID: ADAPTER\n\n\r");
	else if(extender_flag)
		sprintf((char *)tmpstr,"Net ID: EXTENDER\n\n\r");
	else		
		sprintf((char *)tmpstr,"Net ID: %s\n\n\r",ROM_Read_romstr(net_id_list[net_id]));
	ASC_Asci_msg(tmpstr);
	
	if(final_assy_serial_no_str[0] != '\0')
	{
		sprintf((char *)tmpstr,"Serial No: %s\n\n\r", final_assy_serial_no_str);
		ASC_Asci_msg(tmpstr);
	}
	
}
/*==================================================================
Name		:
Parameters	:
Returns		:
Description	:
------------------------------------------------------------------*/
static void Final_test_start_menu(void)
{
	int8 rx_byte;

	// get any RX chars *
	rx_byte = Cmd_check(CMD_ECHO);
	// return if none available
	if(!rx_byte)
		return;

	// now process RX char
	switch(rx_byte)
	{
		case 'X':
		case 'x':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			break;
		case '\n':
		case '\r':
			user_ip_buf_ix = 0;	//reset user input buf index
			user_ip_max_chars = FINAL_SN_STRING_LEN;
			MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_SN_MSG,Get_final_assy_sn);
			break;
		default:
			Rom_msg(INVALID_ENTRY_MSG);
			MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Final_test_start_menu);
			break;
	}
}

/*==================================================================
Name		:
Parameters	:
Returns		:
Description	:
------------------------------------------------------------------*/

static void Get_final_assy_sn(void)
{
	int8 rx_byte;

	/* get any RX chars */
	rx_byte = Cmd_check(CMD_ECHO);
	/* return if none available */
	if(!rx_byte)
	return;

	/* now process RX char */
	switch(rx_byte)
	{
		case 'x':
		case 'X': // exit to start menu and return
			if(user_ip_buf_ix == 0)
			{
				Rom_msg(NEWPAGE_MSG);
				MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
				return;
			}
			user_ip_buf[user_ip_buf_ix++] = rx_byte;
			if(user_ip_buf_ix < user_ip_max_chars)
				return;
			break;			
		case '\n':
		case '\r':
			if(user_ip_buf_ix == 0)
			{
				Rom_msg(NO_FINAL_ASSY_REV_SN_IP_CHARS_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = FINAL_SN_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_SN_MSG,Get_final_assy_sn);
				return;
			}
			// else break to set next data input
			break;
		default:
			if(Check_alpha_char(rx_byte))	//check for valid char
			{
				user_ip_buf[user_ip_buf_ix++] = rx_byte;
				if(user_ip_buf_ix < user_ip_max_chars)
					return;
				else
					break;
			}
			else
			{
				Rom_msg(ALPHANUMERIC_CHARS_ONLY_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = FINAL_SN_STRING_LEN;
				MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_SN_MSG,Get_final_assy_sn);
				return;
			}
		
			break;
	}
	user_ip_buf[user_ip_buf_ix] = '\0'; // terminate string
	strcpy((char *)final_assy_serial_no_str,(char *)user_ip_buf);	// copy string
	// pad out WO string with 0's
	for( ; user_ip_buf_ix < FINAL_SN_STRING_LEN; user_ip_buf_ix++)
		final_assy_serial_no_str[user_ip_buf_ix] = '\0';

	Display_final_assy_details();
	Rom_msg(CONNECT_FINAL_ASSY_TO_JIG_MSG);
	MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Prog_final_assy_details);	

}
/*==================================================================
Name		:
Parameters	:
Returns		:
Description	:
------------------------------------------------------------------*/
static void Prog_final_assy_details(void)
{
	int8 rx_byte;

	// get any RX chars *
	rx_byte = Cmd_check(CMD_ECHO);
	// return if none available
	if(!rx_byte)
		return;

	// now process RX char
	switch(rx_byte)
	{
		case 'X':
		case 'x':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			return;
		case '\n':
		case '\r':
			break;
		default:
			Rom_msg(INVALID_ENTRY_MSG);
			MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Final_test_start_menu);
			return;
	}	
	
	Rom_msg(NEWPAGE_MSG);

//	sprintf(tmpstr,"Programming Details\n\r");
//	ASC_Asci_msg(tmpstr);
// Now store details entered  and update checksum

	MAI_Set_power(ON);
	TIM_Wait(100);

	sprintf((char *)tmpstr,"\n\n\rProgramming Details for %s\n\r",(char *)ROM_Read_romstr(final_assy_name));
	ASC_Asci_msg(tmpstr);

	// Store Prod Number
	if(!Eeprom_write(cur_i2C_addr, EEPROM_FINAL_PROD_NUM_LAYOUT_POS,EEPROM_FINAL_PROD_NUM_LAYOUT_SIZE,final_assy_no_str )) // Store PCB assy number, code and rev
	{
		MAI_Set_power(OFF);
		MEN_Set_cmd_bk_func(FINAL_ASSY_PROG_FAIL_MSG,Retry_prog_final_assy_menu);
		return;
	}
		
	//	Store Prod code ("00")
	Eeprom_write(cur_i2C_addr, EEPROM_FINAL_PROD_CODE_LAYOUT_POS,EEPROM_FINAL_PROD_CODE_LAYOUT_SIZE,(int8 *)"00" ); // Store PCB assy number, code and rev
	//	Store Prod Rev
	Eeprom_write(cur_i2C_addr, EEPROM_FINAL_PROD_REV_LAYOUT_POS,EEPROM_FINAL_PROD_REV_LAYOUT_SIZE,final_assy_rev_str ); // Store PCB assy number, code and rev
	// Store Prod SN
	Eeprom_write(cur_i2C_addr, EEPROM_FINAL_PROD_SN_LAYOUT_POS,EEPROM_FINAL_PROD_SN_LAYOUT_SIZE,final_assy_serial_no_str ); // Store PCB assy number, code and rev
	// Store net ID
	Eeprom_write(cur_i2C_addr, EEPROM_FINAL_PROD_NETID_LAYOUT_POS,EEPROM_FINAL_PROD_NETID_LAYOUT_SIZE,&net_id); // Store NET ID
	// store chan count according to type
	if(adapter_flag || extender_flag)
		Eeprom_write(cur_i2C_addr, EEPROM_FINAL_PROD_CHANCOUNT_LAYOUT_POS,EEPROM_FINAL_PROD_CHANCOUNT_LAYOUT_SIZE,0); // Store chan count = 0 if adapter
	else
		Eeprom_write(cur_i2C_addr, EEPROM_FINAL_PROD_CHANCOUNT_LAYOUT_POS,EEPROM_FINAL_PROD_CHANCOUNT_LAYOUT_SIZE,&chan_count_list[net_id]); // Store chan count according to type
	
	// calc current EEPROM checksum
	cur_calc_checksum = Calc_stored_checksum();
	// store new checksum
	Store_checksum(cur_calc_checksum);


	Display_formatted_assy_info();
	MAI_Set_power(OFF);

	if(final_assy_type == ADAPTER_ASSY)
	{
		Rom_msg(CONNECT_LEGACY_ADAPTER_MSG);
		while(!ASC_Kbhit());

		last_i2C_addr = cur_i2C_addr;
		cur_i2C_addr = PORT_EXPAND_I2C_ADDR;
		
		MAI_Set_power(ON);
		TIM_Wait(100);

		while(!Port_expander_test())
		{
			Rom_msg(ASSY_ID_TEST_FAILED_MSG);
			rx_byte  = Wait_for_x_or_enter();
			if(rx_byte == 'x')
			{
				Rom_msg(NEWPAGE_MSG);
	//			Rom_msg(FINAL_ASSY_TEST_FAIL_MSG);
				user_ip_buf_ix = 0;	//reset user input buf index
				user_ip_max_chars = FINAL_SN_STRING_LEN;
				MEN_Set_cmd_bk_func(FINAL_ASSY_TEST_FAIL_MSG,Prog_same_final_assy_menu);
				MAI_Set_power(OFF);
				cur_i2C_addr = last_i2C_addr;
				return;
			}
			Rom_msg(NEWPAGE_MSG);
			
		}

		cur_i2C_addr = last_i2C_addr;
//		Display_final_assy_details();
		Display_formatted_assy_info();

		MAI_Set_power(OFF);
		MEN_Set_cmd_bk_func(TEST_SAME_FINAL_ASSY_MSG,Prog_same_final_assy_menu);

		return;
	}
	cur_i2C_addr = last_i2C_addr;
	MEN_Set_cmd_bk_func(PROG_SAME_FINAL_ASSY_MSG,Prog_same_final_assy_menu);
	//Display_final_assy_details();
	
	
}
static void Prog_same_final_assy_menu(void)
{
	int8 rx_byte;

	// get any RX chars *
	rx_byte = Cmd_check(CMD_ECHO);
	// return if none available
	if(!rx_byte)
		return;

	// now process RX char
	switch(rx_byte)
	{
		case 'X':
		case 'x':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			return;
		case '\n':
		case '\r':
			Rom_msg(NEWPAGE_MSG);
			Prog_final_assy_details();
			break;
		default:
			Rom_msg(INVALID_ENTRY_MSG);
			MEN_Set_cmd_bk_func(NULL,Prog_same_final_assy_menu);
			return;
	}
	final_assy_serial_no_str[0] = '\0';
	Display_final_assy_details();
	user_ip_buf_ix = 0;	//reset user input buf index
	user_ip_max_chars = FINAL_SN_STRING_LEN;
	MEN_Set_cmd_bk_func(ENTER_FINAL_ASSY_SN_MSG,Get_final_assy_sn);
		
}

static void Retry_prog_final_assy_menu(void)
{
	int8 rx_byte;

	// get any RX chars *
	rx_byte = Cmd_check(CMD_ECHO);
	// return if none available
	if(!rx_byte)
	return;

	// now process RX char
	switch(rx_byte)
	{
		case 'X':
		case 'x':
			Rom_msg(NEWPAGE_MSG);
			MEN_Set_cmd_bk_func(FSTART_MENU_MSG,Fstart_menu);
			return;
		case '\n':
		case '\r':
			Display_final_assy_details();
			MEN_Set_cmd_bk_func(PRESS_X_OR_PROCEED_MSG,Prog_final_assy_details);
			break;
		default:
			Rom_msg(INVALID_ENTRY_MSG);
			MEN_Set_cmd_bk_func(NULL,Prog_same_final_assy_menu);
			return;
	}	
}
/*********************************************************************
*                       End of menu.c                                *
*********************************************************************/
