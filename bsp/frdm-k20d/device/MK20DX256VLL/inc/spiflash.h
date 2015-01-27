/*------------------------------------------------------------------------------
*						Winbond W25 Series SPI Flash Driver
------------------------------------------------------------------------------*/
////////////////////////////////////////////////////////////////////////////////
#ifndef __SPIFLASH_H
#define __SPIFLASH_H
////////////////////////////////////////////////////////////////////////////////
#include "kinetis.h"
#include "type.h"				


////////////////////////////////////////////////////////////////////////////////
#ifdef _SPIFLASH_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif
#undef GLOBAL

#define SPI_FLASH_EN()			FSL_SPI_FLASH_EN()
#define SPI_FLASH_DIS()			FSL_SPI_FLASH_DIS()

#ifdef SW_SPI
	extern void SW_SPI_Tx(void *tx_buff, unsigned int tx_len);
	extern void SW_SPI_Rx(void *rx_buff, unsigned int rx_len);
	#define SPI_FLASH_SEND(tx_buff,tx_len)			SW_SPI_Tx((tx_buff),(tx_len))
	#define SPI_FLASH_RECEIVE(rx_buff,rx_len)		SW_SPI_Rx((rx_buff),(rx_len))
#else
	extern INT32U SPI_Send ( SPI_Type *SPIx, INT8U *buff, INT32U len );
	extern INT32U SPI_Receive ( SPI_Type *SPIx, INT8U *buff, INT32U len );
	#define SPI_FLASH_SEND(tx_buff,tx_len)			SPI_Send ( SPI0, (tx_buff),(tx_len))
	#define SPI_FLASH_RECEIVE(rx_buff,rx_len)		SPI_Receive ( SPI0, (rx_buff),(rx_len))
#endif

////////////////////////////////////////////////////////////////////////////////
unsigned char SPI_Flash_DevID(void);
unsigned char SPI_Flash_ReadStaReg(void);
unsigned char SPI_Flash_Busy(void); 
unsigned char SPI_Flash_ReadByte(unsigned int addr);
void SPI_Flash_Read(unsigned int addr, unsigned char *dat, unsigned int n);
void SPI_Flash_Write(unsigned int addr, unsigned char *dat, unsigned int n);

//low level driver
void SPI_Flash_WriteEnable(void);
void SPI_Flash_WriteDisable(void);
void SPI_Flash_WriteStaReg(unsigned char sta);
void SPI_Flash_PageWrite(unsigned int addr, unsigned char *dat, unsigned int n);
void SPI_Flash_ChipErase(void);
void SPI_Flash_Init(void);


////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////






