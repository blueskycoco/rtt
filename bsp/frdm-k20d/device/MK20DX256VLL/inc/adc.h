#ifndef	__ADC_H_
#define	__ADC_H_

#include "kinetis.h"
#include "type.h"

BOOL ADC_ClkEn ( ADC_Type *ADCx );
BOOL ADC_ClkDis ( ADC_Type *ADCx );

BOOL ADC_Init ( ADC_Type *ADCx );

INT16S ADC_Diff_GetCh ( ADC_Type *ADCx, INT8U ch );

INT16U ADC_Sgl_GetCh ( ADC_Type *ADCx, INT8U ch );

#endif
