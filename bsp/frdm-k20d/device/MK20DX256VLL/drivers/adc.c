#include "adc.h"

BOOL ADC_ClkEn ( ADC_Type *ADCx )
{
	BOOL ret = TRUE;
	switch ((INT32U)ADCx) {
		case (INT32U)ADC0:
			SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
			break;
		case (INT32U)ADC1:
			SIM->SCGC3 |= SIM_SCGC3_ADC1_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL ADC_ClkDis ( ADC_Type *ADCx )
{
	BOOL ret = TRUE;
	switch ((INT32U)ADCx) {
		case (INT32U)ADC0:
			SIM->SCGC6 &= ~SIM_SCGC6_ADC0_MASK;
			break;
		case (INT32U)ADC1:
			SIM->SCGC3 &= ~SIM_SCGC3_ADC1_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL ADC_Init ( ADC_Type *ADCx )
{	
	ADCx->CFG1       |=	ADC_CFG1_MODE(3)|				// 16bit conversion
						ADC_CFG1_ADIV(3)|				// div 8
						ADC_CFG1_ADICLK(0)|				// clock source:bus clock
						ADC_CFG1_ADLSMP_MASK;			// long sample time
	
	ADCx->CFG2       |= ADC_CFG2_ADLSTS(0)|				// 20 extra ADC clock
						ADC_CFG2_ADHSC_MASK;			// hight speed
	return (TRUE);
}

INT16S ADC_Diff_GetCh ( ADC_Type *ADCx, INT8U ch )
{
	INT16S adcval;

	ADCx->SC1[0]  =	ADC_SC1_DIFF_MASK|
					ADC_SC1_ADCH(ch);
	
	while (!(ADCx->SC1[0] & ADC_SC1_COCO_MASK)){;}
 	ADCx->SC1[0] &= ~ADC_SC1_COCO_MASK;

	adcval = ADCx->R[0];
	return (adcval);
}

INT16U ADC_Sgl_GetCh ( ADC_Type *ADCx, INT8U ch )
{
	INT16U adcval;

	ADCx->SC1[0]  =	ADC_SC1_ADCH(ch);
	
	while (!(ADCx->SC1[0] & ADC_SC1_COCO_MASK)){;}
 	ADCx->SC1[0] &= ~ADC_SC1_COCO_MASK;

	adcval = ADCx->R[0];
	return (adcval);
}
