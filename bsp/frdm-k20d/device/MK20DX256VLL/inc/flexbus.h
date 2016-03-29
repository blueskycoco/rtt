#ifndef	__FLEXBUS_H
#define	__FLEXBUS_H

#include "kinetis.h"
#include "type.h"

#define		CS0_BASE				((INT32U)0x60000000)
#define		CS2_BASE				((INT32U)0x80000000)

#define		PSRAM_CY				((INT32U)0x100000)

BOOL FLEXBUS_ClkEn ( void );
BOOL FLEXBUS_ClkDis ( void );
BOOL FLEXBUS_Init ( INT32U div_n );

BOOL Mem_Rd ( INT16U *dat, INT32U offset );
BOOL Mem_Wr ( INT16U dat, INT32U offset );

#endif
