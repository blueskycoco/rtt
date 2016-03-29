#ifndef		__I2C_H
#define		__I2C_H
///////////////////////////////////////////////////////////////////////////////
#include "kinetis.h"
#include "type.h"
///////////////////////////////////////////////////////////////////////////////
#define I2C_TIME_OUT           ((INT32U)0x00010000)
#define I2C_PAUSE              ((INT32U)0x00008000)

#define MWSR                   ((INT8U)0x00)  /* Master write  */
#define MRSW                   ((INT8U)0x01)  /* Master read */

#define EE24x02_CY             ((INT32U)0x00000100)
#define EE24x02_I2C_ADDRESS    ((INT8U)0x50)
#define CS43L22_I2C_ADDRESS    ((INT8U)0x4A)
	
BOOL I2C_ClkEn(I2C_Type *I2Cx);
BOOL I2C_ClkDis(I2C_Type *I2Cx);	
BOOL I2C_Init(I2C_Type *I2Cx);	

BOOL I2C_Write(I2C_Type *I2Cx, INT8U slvaddr, INT8U subaddr, INT8U *buff, INT32U len);
BOOL I2C_Read(I2C_Type *I2Cx, INT8U slvaddr, INT8U subaddr, INT8U *buff, INT32U len);

#endif
