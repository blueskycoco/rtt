#ifndef	__UART_H
#define	__UART_H

#include "kinetis.h"
#include "type.h"

#define UART_PCLK		(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT)+1))
#define TX_EN			UART_C2_TE_MASK
#define RX_EN			UART_C2_RE_MASK

#define OW_EN			((INT8U)0x1)
#define OW_DIS			((INT8U)0x0)

typedef struct
{
	INT8U  *Tx_ptr;
	INT32U Tx_len;
}Uart_TxIndex_type;

typedef struct
{
	INT8U  *Rx_ptr;
	INT16U OW;			//over write
	INT16U OWF;
	INT32U Rx_cnt;
	INT32U Rx_buffsize;
}Uart_RxIndex_type;

typedef struct
{
	Uart_TxIndex_type Tx;
	Uart_RxIndex_type Rx;
}Uart_Index_type;

BOOL UART_ClkEn ( UART_Type *UARTx );
BOOL UART_ClkDis ( UART_Type *UARTx );
BOOL UART_Init ( UART_Type *UARTx, INT32U baud, INT32U ctr, INT8U *rx_buff, INT32U rx_len, INT32U ow);
BOOL UART_Tx ( UART_Type *UARTx, INT8U *buff, INT32U tx_len);
INT32U Get_RxCount ( UART_Type *UARTx );
BOOL Clr_RxCount ( UART_Type *UARTx );
BOOL Clr_OWF ( UART_Type *UARTx );

#endif
