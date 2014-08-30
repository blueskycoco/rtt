#include "flextimer.h"

BOOL FTM_ClkEn ( FTM_Type *FTMx )
{
	BOOL ret = TRUE;
	switch ( (INT32U)FTMx ) {
		case (INT32U)FTM0:
			SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
			break;
		case (INT32U)FTM1:
			SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
			break;
		case (INT32U)FTM2:
			SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL FTM_ClkDis ( FTM_Type *FTMx )
{
	BOOL ret = TRUE;
	switch ( (INT32U)FTMx ) {
		case (INT32U)FTM0:
			SIM->SCGC6 &= ~SIM_SCGC6_FTM0_MASK;
			break;
		case (INT32U)FTM1:
			SIM->SCGC6 &= ~SIM_SCGC6_FTM1_MASK;
			break;
		case (INT32U)FTM2:
			SIM->SCGC3 &= ~SIM_SCGC3_FTM2_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL FTM_Stop ( FTM_Type *FTMx )
{
	FTMx->SC &= ~FTM_SC_CLKS_MASK;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

BOOL FTM_PWM_Init ( FTM_Type *FTMx, INT32U rate )
{   
	INT8U ps;
	INT32U mod;
	ps = (SystemCoreClock/rate)/65535;
	if ( ps >= 4 ) ps = 4;
	
	mod = (SystemCoreClock/(rate*(1<<ps)));  
	if ( mod > 0xFFFF ) mod = 0xFFFF;         
		
	FTMx->SC    &= ~FTM_SC_CLKS_MASK;
	FTMx->SC    |= FTM_SC_CLKS(1);
	FTMx->SC    &= ~FTM_SC_PS_MASK;
	FTMx->SC    |= FTM_SC_PS(ps);
	FTMx->CNT   =  0x0;
	FTMx->MOD   =  mod;
	FTMx->CNTIN =  0x0;
	
	FTMx->COMBINE = FTM_COMBINE_COMBINE1_MASK
					|FTM_COMBINE_COMP1_MASK
					|FTM_COMBINE_DTEN1_MASK
					|FTM_COMBINE_SYNCEN1_MASK;
	
	for ( mod=0; mod<1000000; mod++ );
	
//	FTMx->CONTROLS[0].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
//	FTMx->CONTROLS[0].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
//	FTMx->CONTROLS[0].CnV  =  (FTMx->MOD)/2;
//	FTMx->CONTROLS[1].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
//	FTMx->CONTROLS[1].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
//	FTMx->CONTROLS[1].CnV  =  (FTMx->MOD)/2;
	
	FTMx->CONTROLS[2].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
	FTMx->CONTROLS[2].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
	FTMx->CONTROLS[2].CnV  =  (FTMx->MOD)/2;
	FTMx->CONTROLS[3].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
	FTMx->CONTROLS[3].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
	FTMx->CONTROLS[3].CnV  =  (FTMx->MOD)/2;
	return (TRUE);
}

BOOL FTM_PWM_Set ( FTM_Type *FTMx, INT8U channel, INT32U duty )
{
	INT32U cv;
	if ( duty >= 10000 ) duty = 10000;
	cv = (FTMx->MOD)*(10000-duty)/10000;
	FTMx->CONTROLS[channel].CnV = cv;
	return (TRUE);
}
