#include "cmp.h"

BOOL CMP_ClkEn ( CMP_Type *CMPx)
{
	BOOL retval = TRUE;
	
	switch ((INT32U)CMPx) {
		case (INT32U)CMP0:
		case (INT32U)CMP1:
		case (INT32U)CMP2:
			SIM->SCGC4 |= SIM_SCGC4_CMP_MASK;
			break;
		default:
			retval = FALSE;
			break;
	}
	
	return (retval);
}

BOOL CMP_ClkDis ( CMP_Type *CMPx)
{
	BOOL retval = TRUE;
	
	switch ((INT32U)CMPx) {
		case (INT32U)CMP0:
		case (INT32U)CMP1:
		case (INT32U)CMP2:
			SIM->SCGC4 &= ~SIM_SCGC4_CMP_MASK;
			break;
		default:
			retval = FALSE;
			break;
	}
	
	return (retval);
}

BOOL CMP_Init ( CMP_Type *CMPx, INT8U Vref, INT8U inp_n, INT8U inm_n)
{
	BOOL retval = TRUE;
	
	CMPx->CR0 = 0x00;
	CMPx->CR1 = 0x00;
	CMPx->FPR = 0x00;
	
	CMPx->SCR = CMP_SCR_CFF_MASK
	           |CMP_SCR_CFR_MASK;
			   
	CMPx->DACCR = 0x00;
	CMPx->MUXCR = 0x00;
	
	CMPx->CR0 = 0x00;
	CMPx->CR1 = 0x15;
	
	CMPx->FPR = 0x00;
	CMPx->SCR = 0x1E;
	
	if (Vref==0) {
		CMPx->DACCR &= ~CMP_DACCR_VRSEL_MASK;
	}
	else if (Vref==1) {
		CMPx->DACCR |= CMP_DACCR_VRSEL_MASK;
	}
	else {
		retval = FALSE;
	}

	if (inp_n<8 && inm_n<8) {
		CMPx->MUXCR |= CMP_MUXCR_PSEL(inp_n)
					  |CMP_MUXCR_MSEL(inm_n);
	}
	else {
		retval = FALSE;
	}
	
	CMPx->CR1 |= CMP_CR1_OPE_MASK; 
		
	return (retval);
}

BOOL CMP_DacSetVal ( CMP_Type *CMPx, INT8U stage )
{
	BOOL retval = TRUE;
	
	CMPx->DACCR &= ~CMP_DACCR_VOSEL_MASK;
	CMPx->DACCR |= CMP_DACCR_VOSEL(stage);
	CMPx->DACCR |= CMP_DACCR_DACEN_MASK;
	
	return (retval);
}

BOOL CMP_IntEn ( CMP_Type *CMPx )
{
	BOOL retval = TRUE;
	IRQn_Type irq_n = CMP0_IRQn;
	
	switch ((INT32U)CMPx) {
		case (INT32U)CMP0:
			break;
		case (INT32U)CMP1:
			irq_n += 1;
			break;
		case (INT32U)CMP2:
			irq_n += 2;
			break;
		default:
			retval = FALSE;
	}
	
	CMPx->SCR |=  CMP_SCR_IEF_MASK  | CMP_SCR_IER_MASK; 
	NVIC_EnableIRQ(irq_n);   
	CMPx->SCR |= CMP_SCR_CFR_MASK;
	CMPx->SCR |= CMP_SCR_CFF_MASK;
	
	return (retval);
}

BOOL CMP_IntDis ( CMP_Type *CMPx )
{
	BOOL retval = TRUE;
	IRQn_Type irq_n = CMP0_IRQn;
	
	switch ((INT32U)CMPx) {
		case (INT32U)CMP0:
			break;
		case (INT32U)CMP1:
			irq_n += 1;
			break;
		case (INT32U)CMP2:
			irq_n += 2;
			break;
		default:
			retval = FALSE;
	}
	
	CMPx->SCR &=  ~(CMP_SCR_IEF_MASK  | CMP_SCR_IER_MASK); 
	NVIC_DisableIRQ(irq_n);
	return (retval);
}

BOOL CMP_GetOutVal ( CMP_Type *CMPx )
{
	BOOL retval = FALSE;
	
	if (CMPx->SCR & CMP_SCR_COUT_MASK)
		retval = TRUE;
		
	return (retval);
}

void CMP0_IRQHandler (void)
{
	CMP0->SCR |= CMP_SCR_CFR_MASK
	            |CMP_SCR_CFF_MASK;
}

void CMP1_IRQHandler (void)
{
	CMP1->SCR |= CMP_SCR_CFR_MASK
	            |CMP_SCR_CFF_MASK;
}

void CMP2_IRQHandler (void)
{
	CMP2->SCR |= CMP_SCR_CFR_MASK
	            |CMP_SCR_CFF_MASK;
}
