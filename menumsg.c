/*
 * menumsg.c
 *
 * Created: 02/04/2026 12:25:15
 *  Author: chris.lewis
 */ 
#include "ATMtypes.h"
#include "menumsg.h"
#include "romdata.h"
#include <avr/pgmspace.h>

int8 const NEWLINE_MSG[] PROGMEM =		{"\n\r"};
int8 const NEWPAGE_MSG[] PROGMEM =		{"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\r"};
int8 const DOT_MSG[] PROGMEM =		{"."};
	
// Opening Menu Msgs
 int8 const COPYRIGHT_MSG[] PROGMEM =	
 {
  "**********************************************\n\r"
  "| Copyright(c) 2026 The Magstim Company Ltd. | \n\r"
  "| EEG Net Connector/Adapter - Test Firmware  |\n\r"
  "**********************************************\n\n\r"
  "Firmware ID: "
  };


int8 const FINAL_TEST_JIG_MSG[] PROGMEM = {"\n\nFinal Test Firmware\n\r"
											   "-------------------\n\r"};
int8 const INVALID_JIG_ID_MSG[] PROGMEM = {"\n\n\r!!! INVALID TEST JIG ID DETECTED !!!\n\r"};								  
int8 const BD_TEST_JIG_MSG[] PROGMEM = {"\n\nBoard Test Firmware\n\r"
										    "-------------------\n\n\r"};
 /* common menu messages */
int8 const CMD_NOT_IMPLEMENTED_MSG[] PROGMEM =	{" Command not implemented\n\r"};
 
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
int8 const NET_ASSY_MSG[] PROGMEM = {"NET ASSEMBLY"};
int8 const EXTENSION_ASSY_MSG[] PROGMEM = {"NET EXTENSION ASSEMBLY"};
int8 const ADAPTER_ASSY_MSG[] PROGMEM = {"NET ADAPTER ASSEMBLY"};

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

int8 const HYPER_ADAPTER_NOT_FITTED_MSG[] PROGMEM =
{
	"\n\n\n\rHYPERTRONICS Adapter NOT fitted\n\r"
	"Fit adapter and then retry\n\n\r"
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
	"\n\n\n\r!!! No Assembly Detected !!!\n\n\r"
};

int8 const START_MENU_MSG[] PROGMEM =
{
	"\n\n\r"
	"Start MENU\n\r"
	"==========\n\r"
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
	"5 - 0x41: Port Expander (6768-01 Only)\n\r"
	"\n\rPress X to exit\n\n\r"
};

int8 const PORT_EXPANDER_DEBUG_MSG[] PROGMEM =
{
	"\n\n\n\n\r"
	"Port Expander Debug\n\r"
	"===================\n\r"
	"Press 'X' to Exit or press SPACEBAR to pause and resume\n\n\r"
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




