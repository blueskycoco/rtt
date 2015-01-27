#include "pit.h"

BOOL PIT_ClkEn ( PIT_Type *PITx )
{
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PITx->MCR &= ~PIT_MCR_MDIS_MASK;
	PITx->MCR |= PIT_MCR_FRZ_MASK;
	return (TRUE);
}

BOOL PIT_ClkDis ( PIT_Type *PITx )
{
	SIM->SCGC6 &= ~SIM_SCGC6_PIT_MASK;
	return (TRUE);
}

BOOL PIT_ChSetup ( PIT_Type *PITx, INT8U ch, INT32U value )
{
	PITx->CHANNEL[ch].LDVAL = (INT32U)value;
	PITx->CHANNEL[ch].TFLG  |= PIT_TFLG_TIF_MASK;
	PITx->CHANNEL[ch].TCTRL |= PIT_TCTRL_TEN_MASK
	                         |PIT_TCTRL_TIE_MASK;
	return (TRUE);
}

void PIT0_IRQHandler(void)
{
	extern INT32U CounterPIT;
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;	
	CounterPIT++;
}

void PIT1_IRQHandler(void)
{ 
	PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
}

void PIT2_IRQHandler(void)
{
	PIT->CHANNEL[2].TFLG |= PIT_TFLG_TIF_MASK;
}

void PIT3_IRQHandler(void)
{
	PIT->CHANNEL[3].TFLG |= PIT_TFLG_TIF_MASK;
}
