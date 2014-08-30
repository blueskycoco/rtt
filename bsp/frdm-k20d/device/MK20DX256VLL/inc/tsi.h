#ifndef	__TSI_H
#define	__TSI_H

#include "kinetis.h"
#include "type.h"

#define START_SCANNING()	TSI0->GENCS |= TSI_GENCS_STM_MASK
#define ENABLE_EOS_INT()	TSI0->GENCS |= (TSI_GENCS_TSIIE_MASK|TSI_GENCS_ESOR_MASK)
#define ENABLE_TSI()		TSI0->GENCS |= TSI_GENCS_TSIEN_MASK
#define DISABLE_TSI()		TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK

#define TSI_TRIG_THR		((INT32)0x00000080)

BOOL TSI_ClkEn(TSI_Type *TSIx);
BOOL TSI_ClkDis(TSI_Type *TSIx);
BOOL TSI_Init(TSI_Type *TSIx);
BOOL TSI_Cal(TSI_Type *TSIx);
INT8S TSI_GetKey(TSI_Type *TSIx);
void TSI0_IRQHandler(void);

#endif
