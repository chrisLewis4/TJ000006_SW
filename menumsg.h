/*
 * menumsg.h
 *
 * Created: 02/04/2026 12:25:15
 *  Author: chris.lewis
 */ 


#ifndef MENUMSG_H_
#define MENUMSG_H_

extern int8 const NEWPAGE_MSG[];
extern int8 const COPYRIGHT_MSG[];
extern int8 const FIRMWARE_ID_MSG[];
extern int8 const CHECKSUM_MSG[];
extern int8 const HARDWARE_ID_MSG[];
extern int8 const NEWLINE_MSG[];
extern int8 const DEBUG_MENU_MSG[];
extern int8 const START_MENU_MSG[];

// Opening Menu Msgs
 
extern int8 const FINAL_TEST_JIG_MSG[];
extern int8 const INVALID_JIG_ID_MSG[];
extern int8 const BD_TEST_JIG_MSG[];
 /* common menu messages */
extern int8 const CMD_NOT_IMPLEMENTED_MSG[];
 
// define Net Type Msgs
extern int8 const NET_CONN_PCB_MSG[];
extern int8 const NET_ADAPT_PCB_MSG[];
	
extern int8 const STORED_DETAILS_MSG[];
/* declare Terminal Menus text*/
//*********************  Start Menu  ***************************
extern int8 const FSTART_MENU_MSG[];
extern int8 const INVALID_ENTRY_MSG[];
extern int8 const ENTER_FINAL_ASSY_NO_MSG[];
extern int8 const ENTER_FINAL_ASSY_REV_MSG[];
extern const int8 NET_ASSY_MSG[];
extern const int8 EXTENSION_ASSY_MSG[];
extern const int8 ADAPTER_ASSY_MSG[];

extern int8 const ENTER_FINAL_ASSY_SN_MSG[];

extern int8 const SELECT_NET_ID_MSG[];
extern int8 const ADAPTER_ID_MSG[];

extern int8 const NET_ID0_MSG[];
extern int8 const NET_ID1_MSG[];
extern int8 const NET_ID2_MSG[];
extern int8 const NET_ID3_MSG[];
extern int8 const NET_ID4_MSG[];
extern int8 const NET_ID5_MSG[];
extern int8 const NET_ID6_MSG[];
extern int8 const NET_ID7_MSG[];
extern int8 const NET_ID8_MSG[];
extern int8 const NET_ID9_MSG[];
extern int8 const NET_ID10_MSG[];

extern int8 const TEST_SAME_FINAL_ASSY_MSG[];
extern int8 const PROG_SAME_FINAL_ASSY_MSG[];
extern int8 const FINAL_ASSY_PROG_FAIL_MSG[];
extern int8 const CONNECT_FINAL_ASSY_TO_JIG_MSG[];
extern int8 const CONNECT_LEGACY_ADAPTER_MSG[];
extern int8 const ASSY_ID_TEST_FAILED_MSG[];
extern int8 const FINAL_ASSY_TEST_FAIL_MSG[];
extern int8 const HYPER_ADAPTER_NOT_FITTED_MSG[];
extern int8 const ASSY_TYPE_EXTENDER_MSG[];
extern int8 const ASSY_TYPE_ADAPTER_MSG[];
extern int8 const ASSY_TYPE_NET_MSG[];
extern int8 const BD_TYPE_NET_CONN_MSG[];
extern int8 const BD_TYPE_EXTENDER_MSG[];
extern int8 const BD_TYPE_ADAPTER_MSG[];
extern int8 const ASSY_NOT_DETECTED_MSG[];
extern int8 const START_MENU_MSG[];
extern int8 const SELECT_BD_MSG[];
extern int8 const NET_CONN_SELECTED_MSG[];
extern int8 const NET_ADAPT_SELECTED_MSG[];
extern int8 const ENTER_WO_MSG[];
extern int8 const ENTER_BD_ASSY_NO_MSG[];
extern int8 const ENTER_BD_ASSY_REV_MSG[];
extern int8 const CHECK_DETAILS_MSG[];
extern int8 const PRESS_X_OR_PROCEED_MSG[];
extern int8 const ENTER_SN_MSG[];
extern int8 const ENTER_NEXT_SN_MSG[];
extern int8 const CONNECT_BD_MSG[];
extern int8 const CONNECT_ADAPTER_MSG[];
extern int8 const PROG_EEPROM_MSG[];
extern int8 const PROG_SUCCESS_MSG[];
extern int8 const PORT_EXPANDER_TEST_FAILED_MSG[];
extern int8 const PORT_EXPANDER_TEST_MENU_MSG[];
extern int8 const BD_TEST_SUCCESS_MSG[];
extern int8 const BD_TEST_FAIL_MSG[];
extern int8 const MAX_USER_IP_LEN_EXEEDED_MSG[];
extern int8 const NOT_ENOUGH_ASSY_NO_IP_CHARS_MSG[];
extern int8 const NOT_ENOUGH_ASSY_REV_IP_CHARS_MSG[];
extern int8 const NO_FINAL_ASSY_REV_SN_IP_CHARS_MSG[];
extern int8 const NOT_ENOUGH_WO_NO_IP_CHARS_MSG[];
extern int8 const CHECK_HEADER_MSG[];
extern int8 const CHECKSUM_MISMATCH_MSG[];
extern int8 const CHECKSUM_MATCH_MSG[];
extern int8 const DATA_NOT_SET_MSG[];
extern int8 const PRODUCT_PN_MSG[];
extern int8 const PRODUCT_SN_MSG[];
extern int8 const PCB_ASSY_PN_MSG[];
extern int8 const PCB_ASSY_SN_MSG[];
extern int8 const CHECKING_DATA_RETENTION_MSG[];
extern int8 const DATA_RETENTION_OK_MSG[];
extern int8 const DATA_RETENTION_BAD_MSG[];
extern int8 const EEPROM_READ_ERROR_MSG[];
extern int8 const EEPROM_WRITE_ERROR_MSG[];
extern int8 const ALPHANUMERIC_CHARS_ONLY_MSG[];
extern int8 const NUMERIC_CHARS_ONLY_MSG[];
extern int8 const EEPROM_DEBUG_MSG[];
extern int8 const DEBUG_MENU_MSG[];
extern int8 const PORT_EXPANDER_DEBUG_MSG[];
extern int8 const PORT_EXPANDER_TEST_MSG[];
extern int8 const ADAPTER_BD_NOT_DETECTED_MSG[];
extern int8 const PROG_DETAILS_MSG[];
#endif /* MENUMSG_H_ */