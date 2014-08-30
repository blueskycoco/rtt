#ifndef	__SPI_H
#define	__SPI_H

#include "kinetis.h"
#include "type.h"

////////////////////////////////////////////////////////////////////////////

BOOL SPI_ClkEn ( SPI_Type *SPIx );
BOOL SPI_ClkDis ( SPI_Type *SPIx );

BOOL SPI_Init ( SPI_Type *SPIx, INT32U baud );

INT8U SPI_SendReceive(SPI_Type *SPIx, INT8U tx_dat);
INT32U SPI_Send ( SPI_Type *SPIx, INT8U *buff, INT32U len );
INT32U SPI_Receive ( SPI_Type *SPIx, INT8U *buff, INT32U len );

#endif
