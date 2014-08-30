#include "uart.h"

Uart_Index_type UartIndex[5] = {0};

static INT32U Base2Index ( UART_Type *UARTx )
{
	INT32U index;
	switch ((INT32U)UARTx) {
		case (INT32U)UART0:
			index = 0;
			break;
		case (INT32U)UART1:
			index = 1;
			break;
		case (INT32U)UART2:
			index = 2;
			break;
		case (INT32U)UART3:
			index = 3;
			break;
		case (INT32U)UART4:
			index = 4;
			break;
		default:
			;
	}
	return (index);
}

BOOL UART_ClkEn(UART_Type *UARTx)
{
	switch ((INT32U)UARTx) {
		case (INT32U)UART0:
			SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
			break;
		case (INT32U)UART1:
			SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
			break;
		case (INT32U)UART2:
			SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
			break;
		case (INT32U)UART3:
			SIM->SCGC4 |= SIM_SCGC4_UART3_MASK;
			break;
		case (INT32U)UART4:
			SIM->SCGC1 |= SIM_SCGC1_UART4_MASK;
			break;
		default:
			;
	}
	return (TRUE);
}

BOOL UART_ClkDis(UART_Type *UARTx)
{
	switch ((INT32U)UARTx) {
		case (INT32U)UART0:
			SIM->SCGC4 &= ~SIM_SCGC4_UART0_MASK;
			break;
		case (INT32U)UART1:
			SIM->SCGC4 &= ~SIM_SCGC4_UART1_MASK;
			break;
		case (INT32U)UART2:
			SIM->SCGC4 &= ~SIM_SCGC4_UART2_MASK;
			break;
		case (INT32U)UART3:
			SIM->SCGC4 &= ~SIM_SCGC4_UART3_MASK;
			break;
		case (INT32U)UART4:
			SIM->SCGC1 &= ~SIM_SCGC1_UART4_MASK;
			break;
		default:
			;
	}
	return (TRUE);
}

BOOL UART_SetBaud ( UART_Type *UARTx, INT32U baud )
{
	BOOL ret = TRUE;
	
	INT16U sbr,tmp,brfa;

    sbr = (INT32U)((UART_PCLK) / (baud * 16));
    tmp = UARTx->BDH & ~UART_BDH_SBR_MASK;
	UARTx->BDH = tmp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));

    UARTx->BDL = (INT8U)(sbr & UART_BDL_SBR_MASK);

    brfa = (((UART_PCLK * 32)/(baud * 16)) - (sbr * 32));
    tmp = UARTx->C4 & ~UART_C4_BRFA_MASK;
    UARTx->C4 = tmp |  UART_C4_BRFA(brfa); 
	
	return (ret);
}

BOOL UART_Init ( UART_Type *UARTx, INT32U baud, INT32U ctr, INT8U *rx_buff, INT32U rx_len, INT32U ow)
{	
	INT32U index;
	
	index = Base2Index(UARTx);
	UARTx->C1 = 0x00;
	UARTx->C2 = ctr;
	UARTx->C3 = 0x00;
	
	UART_SetBaud ( UARTx, baud );
	
	if (ctr & RX_EN) {										//enable receive interrupt		
		UartIndex[index].Rx.Rx_ptr = rx_buff;
		UartIndex[index].Rx.Rx_buffsize = rx_len;
		UartIndex[index].Rx.OW = ow;
		UartIndex[index].Rx.OWF= 0; 
		UARTx->C2 |= UART_C2_RIE_MASK;
	}
	return (TRUE);
}

BOOL UART_Tx ( UART_Type *UARTx, INT8U *buff, INT32U tx_len)
{
	BOOL ret = TRUE;
	INT32U index;
	
	index = Base2Index(UARTx);
	if (UartIndex[index].Tx.Tx_len == 0) {
		UartIndex[index].Tx.Tx_ptr = buff;
		UartIndex[index].Tx.Tx_len = tx_len;
		
		UARTx->C2 |= UART_C2_TCIE_MASK;
	}
	else {
		ret = FALSE;
	}
	return (ret);
}

INT32U Get_RxCount ( UART_Type *UARTx )
{	
	return (UartIndex[Base2Index(UARTx)].Rx.Rx_cnt);
}

BOOL Clr_OWF ( UART_Type *UARTx )
{
	UartIndex[Base2Index(UARTx)].Rx.OWF = 0;
	return (TRUE);
}

BOOL Clr_RxCount ( UART_Type *UARTx )
{
	UartIndex[Base2Index(UARTx)].Rx.Rx_ptr -= UartIndex[Base2Index(UARTx)].Rx.Rx_cnt;
	UartIndex[Base2Index(UARTx)].Rx.Rx_cnt = 0;
	return (TRUE);
}

void UART0_RX_TX_IRQHandler(void)
{
	volatile INT8U sdata;
	
	if (UART0->C2 & UART_C2_TCIE_MASK) {
		if (UART0->S1 & UART_S1_TC_MASK) {
			if (UartIndex[0].Tx.Tx_len > 0) {
				UART0->D = *UartIndex[0].Tx.Tx_ptr++;
				UartIndex[0].Tx.Tx_len--;
			}
			else {
				UART0->C2 &= ~UART_C2_TCIE_MASK;
			}
		}
	}
	
	if (UART0->S1 & UART_S1_RDRF_MASK) {
		sdata = UART0->D;
		if (UartIndex[0].Rx.Rx_cnt < UartIndex[0].Rx.Rx_buffsize) {
			*UartIndex[0].Rx.Rx_ptr++ = sdata;
			UartIndex[0].Rx.Rx_cnt++;
		}
		else {
			if (UartIndex[0].Rx.OW != 0) {
				UartIndex[0].Rx.Rx_ptr -= UartIndex[0].Rx.Rx_buffsize;
				*UartIndex[0].Rx.Rx_ptr++ = sdata;
				UartIndex[0].Rx.Rx_cnt = 1;
				UartIndex[0].Rx.OWF = 1;
			}
		}
	}
}

void UART0_ERR_IRQHandler(void)
{
	;
}

void UART1_RX_TX_IRQHandler(void)
{	
	;
}

void UART1_ERR_IRQHandler(void)
{
	;
}

void UART2_RX_TX_IRQHandler(void)
{	
	;
}

void UART2_ERR_IRQHandler(void)
{
	;
}

void UART3_RX_TX_IRQHandler(void)
{	
	volatile INT8U sdata;
	
	if (UART3->C2 & UART_C2_TCIE_MASK) {
		if (UART3->S1 & UART_S1_TC_MASK) {
			if (UartIndex[3].Tx.Tx_len > 0) {
				UART3->D = *UartIndex[3].Tx.Tx_ptr++;
				UartIndex[3].Tx.Tx_len--;
			}
			else {
				UART3->C2 &= ~UART_C2_TCIE_MASK;
			}
		}
	}
	
	if (UART3->S1 & UART_S1_RDRF_MASK) {
		sdata = UART3->D;
		if (UartIndex[3].Rx.Rx_cnt < UartIndex[3].Rx.Rx_buffsize) {
			*UartIndex[3].Rx.Rx_ptr++ = sdata;
			UartIndex[3].Rx.Rx_cnt++;
		}
		else {
			if (UartIndex[3].Rx.OW != 0) {
				UartIndex[3].Rx.Rx_ptr -= UartIndex[3].Rx.Rx_buffsize;
				*UartIndex[3].Rx.Rx_ptr++ = sdata;
				UartIndex[3].Rx.Rx_cnt = 1;
				UartIndex[3].Rx.OWF = 1;
			}
		}
	}
}

void UART3_ERR_IRQHandler(void)
{
	;
}

void UART4_RX_TX_IRQHandler(void)
{	
	volatile INT8U sdata;
	
	if (UART4->C2 & UART_C2_TCIE_MASK) {
		if (UART4->S1 & UART_S1_TC_MASK) {
			if (UartIndex[4].Tx.Tx_len > 0) {
				UART4->D = *UartIndex[4].Tx.Tx_ptr++;
				UartIndex[4].Tx.Tx_len--;
			}
			else {
				UART4->C2 &= ~UART_C2_TCIE_MASK;
			}
		}
	}
	
	if (UART4->S1 & UART_S1_RDRF_MASK) {
		sdata = UART4->D;
		if (UartIndex[4].Rx.Rx_cnt < UartIndex[4].Rx.Rx_buffsize) {
			*UartIndex[4].Rx.Rx_ptr++ = sdata;
			UartIndex[4].Rx.Rx_cnt++;
		}
		else {
			if (UartIndex[4].Rx.OW != 0) {
				UartIndex[4].Rx.Rx_ptr -= UartIndex[4].Rx.Rx_buffsize;
				*UartIndex[4].Rx.Rx_ptr++ = sdata;
				UartIndex[4].Rx.Rx_cnt = 1;
				UartIndex[4].Rx.OWF = 1;
			}
		}
	}
}

void UART4_ERR_IRQHandler(void)
{
	;
}
