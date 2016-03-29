#ifndef	__DAC_H
#define	__DAC_H

#include "kinetis.h"
#include "type.h"

BOOL DAC_ClkEn ( DAC_Type *DACx );
BOOL DAC_ClkDis ( DAC_Type *DACx );
BOOL DAC_Init ( DAC_Type *DACx );
BOOL DAC_Convert ( DAC_Type *DACx, INT16U stage );

#endif
