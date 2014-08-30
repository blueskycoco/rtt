#include "dac.h"

BOOL DAC_ClkEn ( DAC_Type *DACx )
{
	BOOL retval = TRUE;
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
	return (retval);
}

BOOL DAC_ClkDis ( DAC_Type *DACx )
{
	BOOL retval = TRUE;
	SIM->SCGC2 &= ~SIM_SCGC2_DAC0_MASK;
	return (retval);
}

BOOL DAC_Init ( DAC_Type *DACx)
{  
	BOOL retval = TRUE;
	
	DACx->C0 |= DAC_C0_DACEN_MASK
	           |DAC_C0_DACRFS_MASK
	           |DAC_C0_DACTRGSEL_MASK;
	
//	DACx->C1 |=	DAC_C1_DACBFEN_MASK
//	           |DAC_C1_DACBFMD(1);
	
	return (retval);
}

BOOL DAC_Convert ( DAC_Type *DACx, INT16U stage )
{
	BOOL retval = TRUE;
	
	DACx->DAT[0].DATL = stage &  0xFF;
	DACx->DAT[0].DATH = stage >> 8;
	DACx->C0 |=	DAC_C0_DACSWTRG_MASK;
	
	return (retval);
}
