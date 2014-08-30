//////////////////////////////////////////////////////////////////////////////
#define _SPIFLASH_C_
//////////////////////////////////////////////////////////////////////////////

#include "spiflash.h"

 
unsigned char SPI_Flash_ReadByte(unsigned int addr)
{
	unsigned char *p;
	unsigned char txdbuff[6];
	txdbuff[0] = 0x03;
	p = (unsigned char *)&addr;
	txdbuff[3] = *p++;
	txdbuff[2] = *p++;
	txdbuff[1] = *p;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 4 );
	SPI_FLASH_RECEIVE(txdbuff, 1 );
	SPI_FLASH_DIS();
	return txdbuff[0];
}

void SPI_Flash_Read(unsigned int addr, unsigned char *dat, unsigned int n)
{
	unsigned char *p;
	unsigned char txdbuff[6];
	txdbuff[0] = 0x0B;
	p = (unsigned char *)&addr;
	txdbuff[3] = *p++;
	txdbuff[2] = *p++;
	txdbuff[1] = *p;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 5 );
	SPI_FLASH_RECEIVE(dat, n );
	SPI_FLASH_DIS();
}

void SPI_Flash_WriteEnable(void)
{
	unsigned char txdbuff = 0x06;
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1 );	
	SPI_FLASH_DIS();	
}

void SPI_Flash_WriteDisable(void)
{
	unsigned char txdbuff = 0x04;
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1 );	
	SPI_FLASH_DIS();	
}

unsigned char SPI_Flash_ReadStaReg(void)
{
	unsigned char txdbuff = 0x05;
	SPI_FLASH_EN();
	SPI_FLASH_SEND(&txdbuff, 1 );	
	SPI_FLASH_RECEIVE(&txdbuff, 1 );
	SPI_FLASH_DIS();
	return txdbuff;
}

void SPI_Flash_WriteStaReg(unsigned char sta)
{
	unsigned char txdbuff = 0x01;
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1 );	
	SPI_FLASH_SEND((unsigned char *)&sta, 1 );
	SPI_FLASH_DIS();	
}


void SPI_Flash_PageWrite(unsigned int addr, unsigned char *dat, unsigned int n)
{
	unsigned char txdbuff[6];

	SPI_Flash_WriteEnable();
	
	txdbuff[0] = 0x02;
	txdbuff[1] = addr >> 16;
	txdbuff[2] = addr >> 8;
	txdbuff[3] = addr;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 4 );
	SPI_FLASH_SEND(dat, n );
	SPI_FLASH_DIS();
	while (SPI_Flash_Busy());
}

void SPI_Flash_Write(unsigned int addr, unsigned char *dat, unsigned int n)
{
	unsigned int m;
	if (((addr & 0xff) + n) > 0x100) {
		m = 0x100 - (addr & 0xff);
		SPI_Flash_PageWrite(addr, dat, m);
		SPI_Flash_PageWrite((addr + m), dat + m, n - m);
	}
	else {
		SPI_Flash_PageWrite(addr, dat, n);
	}
}

void SPI_Flash_ChipErase(void)
{
	unsigned char txdbuff = 0x60;
	SPI_Flash_WriteEnable();	
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1);	
	SPI_FLASH_DIS();
	while (SPI_Flash_Busy());
}


void SPI_Flash_Init(void)
{
	SPI_Flash_WriteStaReg(0x00);
}

unsigned char SPI_Flash_Busy(void)
{
	return (SPI_Flash_ReadStaReg() & 0x01);
}

unsigned char SPI_Flash_DevID(void)
{
	unsigned char txdbuff[6];
	txdbuff[0] = 0x90;										//fast read
	txdbuff[3] = 0x00;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 4 );
	SPI_FLASH_RECEIVE(txdbuff, 2 );
	SPI_FLASH_DIS();
	return txdbuff[1];
}
