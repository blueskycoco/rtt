#ifndef __KINETIS_H
#define __KINETIS_H

#include "MK20D7.h"

#define FSL_LCD_CS_HIGH()       PTA->PSOR = 1 << 14
#define FSL_LCD_CS_LOW()        PTA->PCOR = 1 << 14
#define FSL_LCD_CD_HIGH()       PTB->PSOR = 1 << 20
#define FSL_LCD_CD_LOW()        PTB->PCOR = 1 << 20

#define FSL_LCD_CLK_HIGH()      PTA->PSOR = 1 << 15
#define FSL_LCD_CLK_LOW()       PTA->PCOR = 1 << 15
#define FSL_LCD_DATA_HIGH()     PTA->PSOR = 1 << 16
#define FSL_LCD_DATA_LOW()      PTA->PCOR = 1 << 16

#define FSL_SPI_FLASH_EN()      PTB->PCOR = 1 << 23
#define FSL_SPI_FLASH_DIS()     PTB->PSOR = 1 << 23

#endif
