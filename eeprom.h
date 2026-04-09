/*
 * eeprom.h
 *
 * Created: 02/04/2026 12:37:07
 *  Author: chris.lewis
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include "ATMtypes.h"

extern int8 EE_Eeprom_read(int8 eeprom_addr, int16 byte_addr,int16 byte_count,int8 *data);
extern int8 EE_Eeprom_write(int8 eeprom_addr,int16 byte_addr, int16 byte_count,int8 *data);
extern void EE_Eeprom_fill(int8 cur_i2C_addr, int8 set_char);
extern void Eeprom_hex_dump(int8 cur_i2C_addr);
extern int16 EE_Calc_stored_checksum(int8 cur_i2C_addr);
extern int16 EE_Get_stored_checksum(int8 cur_i2C_addr);
extern void EE_Store_checksum(int8 cur_i2C_addr, int16 checksum);


#define EEPROM_PAGE_SIZE 16

// Define valid I²C addresses
#define NET_CONN_I2C_ADDR		0x50
#define NET_EXTEND_I2C_ADDR		0x54
#define PORT_EXPAND_I2C_ADDR	0x41
#define ADC_BOT_I2C_ADDR		0x52
#define ADC_TOP_I2C_ADDR		0x56


#endif /* eeprom.h */