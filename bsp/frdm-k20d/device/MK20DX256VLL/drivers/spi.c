#include "spi.h"


BOOL SPI_ClkEn ( SPI_Type *SPIx )
{
	switch ((INT32U)SPIx) {
		case (INT32U)SPI0:
			SIM->SCGC6 |= SIM_SCGC6_SPI0_MASK;
			break;
		case (INT32U)SPI1:
			SIM->SCGC6 |= SIM_SCGC6_SPI1_MASK;
			break;
//		case (INT32U)SPI2:
//			SIM->SCGC3 |= SIM_SCGC3_SPI2_MASK;
//			break;
		default:
			;
	}
	
	return (TRUE);
}

BOOL SPI_ClkDis ( SPI_Type *SPIx )
{
	switch ((INT32U)SPIx) {
		case (INT32U)SPI0:
			SIM->SCGC6 &= ~SIM_SCGC6_SPI0_MASK;
			break;
		case (INT32U)SPI1:
			SIM->SCGC6 &= ~SIM_SCGC6_SPI1_MASK;
			break;
//		case (INT32U)SPI2:
//			SIM->SCGC3 &= ~SIM_SCGC3_SPI2_MASK;
//			break;
		default:
			;
	}
	
	return (TRUE);
}

BOOL SPI_Init ( SPI_Type *SPIx, INT32U baud )
{
	SPIx->MCR	 = (SPI_MCR_CLR_RXF_MASK|
					SPI_MCR_CLR_TXF_MASK|
					SPI_MCR_PCSIS_MASK	|
					SPI_MCR_HALT_MASK	);	
	SPIx->MCR	|= 	SPI_MCR_MSTR_MASK;
	
	SPIx->CTAR[0] = (//SPI_CTAR_DBR_MASK	|
					SPI_CTAR_BR(7)		|
					SPI_CTAR_FMSZ(7)	|
					SPI_CTAR_CSSCK(4)	|
					SPI_CTAR_ASC(4)		|
					SPI_CTAR_DT(6)		|
					SPI_CTAR_PDT_MASK	//|
// 					SPI_CTAR_CPOL_MASK	|
// 					SPI_CTAR_CPHA_MASK	
					);
						
	SPIx->SR	    |= (SPI_SR_EOQF_MASK	|
					SPI_SR_TFFF_MASK	|
					SPI_SR_TFUF_MASK	|
					SPI_SR_RFDF_MASK	|
					SPI_SR_RFOF_MASK	);

	SPIx->MCR	&=  ~SPI_MCR_HALT_MASK;
	return (TRUE);
}

INT8U SPI_SendReceive ( SPI_Type *SPIx, INT8U tx_dat )
{
	SPIx->SR	 = (SPI_SR_EOQF_MASK	|
				SPI_SR_TFFF_MASK	|
				SPI_SR_TFUF_MASK	|
				SPI_SR_RFDF_MASK	|
				SPI_SR_RFOF_MASK	);
	
	SPIx->MCR |= (SPI_MCR_CLR_RXF_MASK|
				SPI_MCR_CLR_TXF_MASK);	
		
	SPIx->PUSHR = (SPI_PUSHR_CTAS(0)	|
				SPI_PUSHR_EOQ_MASK	|
				SPI_PUSHR_PCS(1)	|	/*SPI Flash cs Conflict with lcd_en(pcs1) spiflash need comment this statement*/
				SPI_PUSHR_TXDATA(tx_dat));
		
 	while((SPIx->SR & SPI_SR_TCF_MASK) == 0);
	SPIx->SR |= SPI_SR_TCF_MASK;
	
	return (SPIx->POPR);
}

INT32U SPI_Send ( SPI_Type *SPIx, INT8U *buff, INT32U len )
{
	while(len-- != 0) 
		SPI_SendReceive ( SPIx, *buff++);
	return (len);
}

INT32U SPI_Receive ( SPI_Type *SPIx, INT8U *buff, INT32U len )
{
	while(len-- != 0)
		*buff++ = SPI_SendReceive ( SPIx, 0xFF);
	return (len);
}

void SPI0_IRQHandler(void)
{
	;
}

void SPI1_IRQHandler(void)
{
	;
}

void SPI2_IRQHandler(void)
{
	;
}
