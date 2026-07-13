/*
 * eeprom.c
 *
 * Created: 02/04/2026 12:35:41
 * Author: chris.lewis
 * Project: TJ000006_SW EGI Net connector/Adapter Test Firmware

 This module is responsible for the interface to the EEPROM on the Assembly under test
 */ 

#include "ATMtypes.h"
#include "eeprom.h"
#include "i2c.h"
#include "asci.h"
#include "menu.h"
#include "menumsg.h"
#include "romdata.h"
#include <stdio.h>
#include <string.h>


#define EE_WR_TIMEOUT 0xff
#define EEPROM_RETRY_COUNT 1000
#define EEPROM_BYTE_COUNT 512

#define EEPROM_CHECKSUM_LAYOUT_SIZE			2   // 2 bytes - MSB first
#define EEPROM_CHECKSUM_LAYOUT_POS			0xfe



//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------


int8 EE_Eeprom_read(int8 eeprom_addr, int16 byte_addr,int16 byte_count,int8 *data)
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
		MEN_Rom_msg(EEPROM_READ_ERROR_MSG);
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

int8 EE_Eeprom_write(int8 eeprom_addr,int16 byte_addr, int16 byte_count,int8 *data)
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

void EE_Eeprom_fill(int8 cur_i2C_addr, int8 set_char)
{
	int8 buf[EEPROM_PAGE_SIZE+1];
	int8 inbuf[EEPROM_PAGE_SIZE+1];
	int16 x,n,pc_complete;
	
	for(n = 0; n < EEPROM_PAGE_SIZE; n++)
		buf[n] = set_char;
	
	for(x = 0; x < EEPROM_BYTE_COUNT; x += EEPROM_PAGE_SIZE)
	{
		pc_complete = ((x * 100)/EEPROM_BYTE_COUNT);
		while(!ASC_Asci_tx_empty());
		sprintf((char *)tmpstr,"\rResetting All of EEPROM to %02x - %03d%% done",(int16)buf,pc_complete+1);
		ASC_Asci_msg(tmpstr);
		
		// Write char to EEPROM
		EE_Eeprom_write(cur_i2C_addr,x,EEPROM_PAGE_SIZE,buf);
		
		// Now check data has been set
		EE_Eeprom_read(cur_i2C_addr,x,EEPROM_PAGE_SIZE,inbuf);	//read 16 byte of data from EEPROM

		for(n = 0; n < EEPROM_PAGE_SIZE; n++)
		{
			if(inbuf[n] != set_char)
			{
				while(!ASC_Asci_tx_empty());
				sprintf((char *)tmpstr,"\n\rErase ERR %x\n\r",(int16)inbuf[n]);
				ASC_Asci_msg(tmpstr);
				return;
			}
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

void Eeprom_hex_dump(int8 cur_i2C_addr)
{
	int8 n,buf[EEPROM_PAGE_SIZE+2];
	int16 x;

	ASC_Asci_msg((int8 *)"\n\n\rHEX DUMP\n\r\n");
	buf[0] = 0x00;	//set initial read byte addr to 0

	for(x = 0;x < EEPROM_BYTE_COUNT;x += EEPROM_PAGE_SIZE)
	{
		sprintf((char *)tmpstr,"Addr = %04x: ",x);
		ASC_Asci_msg(tmpstr);
		
		EE_Eeprom_read(cur_i2C_addr,x,EEPROM_PAGE_SIZE,buf);

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

int16 EE_Calc_stored_checksum(int8 cur_i2C_addr)
{
	int8 n,buf[EEPROM_PAGE_SIZE+2];
	int16 x, checksum, stored_sum;

	checksum = 0;
	buf[0] = 0x00;	//set initial read byte addr to 0

	// add all bytes in eeprom to sum
	for(x = 0;x < EEPROM_BYTE_COUNT;x += EEPROM_PAGE_SIZE)
	{
		EE_Eeprom_read(cur_i2C_addr,x,EEPROM_PAGE_SIZE,buf);

		for(n = 0; n < EEPROM_PAGE_SIZE; n++)
		checksum += (int16)buf[n];
	}
	stored_sum = EE_Get_stored_checksum(cur_i2C_addr);
	checksum -= (stored_sum & 0x00ff);
	checksum -= ((stored_sum >> 8) & 0x00ff);
	checksum = ~checksum;
	return checksum;
}
//====================================================================
// Name			:
// Parameters	:
// Returns		:
// Description	:
//--------------------------------------------------------------------

int16 EE_Get_stored_checksum(int8 cur_i2C_addr)
{
	int8  buf[EEPROM_CHECKSUM_LAYOUT_SIZE+1];
	int16 checksum;
	
	EE_Eeprom_read(cur_i2C_addr, EEPROM_CHECKSUM_LAYOUT_POS,EEPROM_CHECKSUM_LAYOUT_SIZE, buf);

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

void EE_Store_checksum(int8 cur_i2C_addr, int16 checksum)
{
	int8 buf[EEPROM_CHECKSUM_LAYOUT_SIZE+1];
	
	buf[0] = (int8)(checksum >> 8) & 0xff;
	buf[1] = (int8)(checksum & 0x00ff);
	
	//	sprintf(tmpstr,"\n\r** Storing %04x, %02x, %02x **\n\r", checksum, (int16)buf[0], (int16)buf[1]);
	//	ASC_Asci_msg(tmpstr);
	EE_Eeprom_write(cur_i2C_addr, EEPROM_CHECKSUM_LAYOUT_POS,EEPROM_CHECKSUM_LAYOUT_SIZE, buf);
}
