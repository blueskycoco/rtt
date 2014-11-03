#include	"i2c.h"

BOOL I2C_ClkEn ( I2C_Type *I2Cx )
{
	BOOL ret = TRUE;
	switch ((INT32U)I2Cx) {
		case (INT32U)I2C0:
			SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
			break;
		case (INT32U)I2C1:
			SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL I2C_ClkDis ( I2C_Type *I2Cx )
{
	BOOL ret = TRUE;
	switch ((INT32U)I2Cx) {
		case (INT32U)I2C0:
			SIM->SCGC4 &= ~SIM_SCGC4_I2C0_MASK;
		case (INT32U)I2C1:
			SIM->SCGC4 &= ~SIM_SCGC4_I2C1_MASK;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL I2C_Init ( I2C_Type *I2Cx )
{
	I2Cx->F = 0x70|
	I2C_F_MULT(0x02)|       	/* set MULT and ICR */
	I2C_F_ICR(0x1E);			// 400k bps
    I2Cx->C1 = I2C_C1_IICEN_MASK;       /* enable IIC */
//I2Cx->C1 |= I2C_C1_IICIE_MASK;;
	
	return (TRUE);
}

void Pause(void){
	INT32U n;
	for(n=0;n<I2C_PAUSE;n++) {
		__asm("nop");
	}
}

/******************************************************************************
**    function  : I2C_Wait 
**    parameter : type 0 = wait transfer done
**                type 1 = wait ack
******************************************************************************/
BOOL I2C_Wait ( I2C_Type *I2Cx, INT8U type )
{
	BOOL ret = TRUE;
	INT32U tm_out = I2C_TIME_OUT;
	
	if (type == 0) {
		while((--tm_out) && ((I2Cx->S & I2C_S_IICIF_MASK)==0));
		I2Cx->S |= I2C_S_IICIF_MASK;
		if (tm_out == 0)
			ret = FALSE;
	}
	else {
		while((--tm_out) && ((I2Cx->S & I2C_S_RXAK_MASK)!=0));
		if (tm_out == 0)
			ret = FALSE;
	}

	return (ret);
}

BOOL I2C_Write ( I2C_Type *I2Cx, INT8U slvaddr, INT8U subaddr, INT8U *buff, INT32U len )
{
	BOOL ret = TRUE;
	
	if ((I2Cx->S & I2C_S_BUSY_MASK)==0) {
		//start
		I2Cx->C1 |= I2C_C1_TX_MASK
					|I2C_C1_MST_MASK;
		//slave addr
		I2Cx->D = (slvaddr<<1)| MWSR;
		//wait
		ret &= I2C_Wait(I2Cx, 0);
		ret &= I2C_Wait(I2Cx, 1);
		
		//sub addr
		I2Cx->D = subaddr;
		//wait
		ret &= I2C_Wait(I2Cx, 0);	
		ret &= I2C_Wait(I2Cx, 1);
		
		//write data
		while(len-- != 0) {
			I2Cx->D = *buff++;
			ret &= I2C_Wait(I2Cx, 0);		
			ret &= I2C_Wait(I2Cx, 1);
		}
		//stop
		I2Cx->C1 &= ~I2C_C1_MST_MASK;
		I2Cx->C1 &= ~I2C_C1_TX_MASK;
		Pause();
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}

BOOL I2C_Read ( I2C_Type *I2Cx, INT8U slvaddr, INT8U subaddr, INT8U *buff, INT32U len )
{
	BOOL ret = TRUE;
	
	if ((I2Cx->S & I2C_S_BUSY_MASK)==0) {
		//start
		I2Cx->C1 |= I2C_C1_TX_MASK
					|I2C_C1_MST_MASK;
		//slave addr
		I2Cx->D = (slvaddr<<1)| MWSR;
		//wait
		ret &= I2C_Wait(I2Cx, 0);	
		ret &= I2C_Wait(I2Cx, 1);
		
		//sub addr
		I2Cx->D = subaddr;
		//wait
		ret &= I2C_Wait(I2Cx, 0);
		ret &= I2C_Wait(I2Cx, 1);
		
		//restart
		I2Cx->C1 |= I2C_C1_RSTA_MASK;
		//slave addr
		I2Cx->D = (slvaddr<<1)| MRSW;
		//wait
		ret &= I2C_Wait(I2Cx, 0);	
		ret &= I2C_Wait(I2Cx, 1);
		
		//enter rx mode
		I2Cx->C1 &= ~I2C_C1_TX_MASK;						   
		
		if (len > 1) {
			I2Cx->C1 &= ~I2C_C1_TXAK_MASK;
		}
		else if (len == 1) {
			I2Cx->C1 |= I2C_C1_TXAK_MASK;
		}
		else {
			;
		}
		*buff = I2Cx->D;
			
		while (--len) {
			ret &= I2C_Wait(I2Cx, 0);
			if (len == 1) {
				I2Cx->C1 |= I2C_C1_TXAK_MASK;
			}
			else {
				I2Cx->C1 &= ~I2C_C1_TXAK_MASK;
			}
			*buff++ = I2Cx->D;
		}
		
		ret &= I2C_Wait(I2Cx, 0);

		I2Cx->C1 &= ~I2C_C1_MST_MASK;
		I2Cx->C1 &= ~I2C_C1_TX_MASK;
		
		*buff = I2Cx->D;
		Pause();
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}
