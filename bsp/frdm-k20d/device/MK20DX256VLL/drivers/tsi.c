#include "kinetis.h"
#include "type.h"
#include "tsi.h"

INT32 Tsich_cal[4] = {0x200, 0x200, 0x200, 0x200};
static INT8S sKeyVal = 0;

INT8S TSI_GetKey(TSI_Type *TSIx)
{
	return (sKeyVal);
}

BOOL TSI_ClkEn(TSI_Type *TSIx)
{
	BOOL ret = TRUE;
	switch ((INT32U)TSIx) {
		case (INT32U)TSI0:
			SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;
			break;
		default :
			ret = FALSE;
	}
	return (ret);
}

BOOL TSI_ClkDis(TSI_Type *TSIx)
{
	BOOL ret = TRUE;
	switch ((INT32U)TSIx) {
		case (INT32U)TSI0:
			SIM->SCGC5 &= ~SIM_SCGC5_TSI_MASK;
			break;
		default :
			ret = FALSE;
	}
	return (ret);
}

BOOL TSI_Init(TSI_Type *TSIx)
{		
	TSI_ClkEn(TSIx);
	
	TSIx->GENCS =   TSI_GENCS_LPSCNITV(4)|
					TSI_GENCS_NSCN(9)|
					TSI_GENCS_PS(2)|
					TSI_GENCS_TSIIE_MASK|
					TSI_GENCS_STM_MASK|
					TSI_GENCS_ESOR_MASK;
	
	TSIx->SCANC |=  TSI_SCANC_EXTCHRG(3)|
					TSI_SCANC_REFCHRG(31)|
					TSI_SCANC_SMOD(0)|
					TSI_SCANC_AMPSC(0);
	
	TSIx->PEN   |=  TSI_PEN_PEN0_MASK|			// enable the chanel 0,6,7,8
					TSI_PEN_PEN6_MASK|
					TSI_PEN_PEN7_MASK|
					TSI_PEN_PEN8_MASK;

	TSIx->GENCS |=  TSI_GENCS_TSIEN_MASK;		// enables tsi
	return (TRUE);
}
	
BOOL TSI_Cal(TSI_Type *TSIx)
{
	TSIx->GENCS |= TSI_GENCS_SWTS_MASK;
	while(!(TSIx->GENCS&TSI_GENCS_EOSF_MASK));	
	
	Tsich_cal[0] = TSI0->CNTR1 & (INT32U)0xFFFF;
	Tsich_cal[1] = TSI0->CNTR7 & (INT32U)0xFFFF;
	Tsich_cal[2] = TSI0->CNTR7 >> 16;
	Tsich_cal[3] = TSI0->CNTR9 & (INT32U)0xFFFF;
	
	TSIx->THRESHOLD = TSI0->CNTR9 & (INT32U)0xFFFF + TSI_TRIG_THR;

	TSIx->GENCS |= TSI_GENCS_OUTRGF_MASK;
	TSIx->GENCS |= TSI_GENCS_EOSF_MASK;
	
	return (TRUE);
}

void TSI0_IRQHandler(void)
{
	/* a key = two electrode   1=0&1 6=0&2 5=0&3 2=1&2 4=1&3 3=2&3 */
	const INT8 cKeyTable[6] = {0x01, 0x06, 0x05, 0x02, 0x04, 0x03};
	INT32 tsich_val[4];
	INT8 i,j;
	INT8 kcnt = 0;
	INT8 k = 0;
	INT8S key_val = 0;

	TSI0->GENCS |=  TSI_GENCS_OUTRGF_MASK|
					TSI_GENCS_EOSF_MASK|
					TSI_GENCS_OVRF_MASK|
					TSI_GENCS_EXTERF_MASK;
	
	tsich_val[0] = (TSI0->CNTR1 & (INT32U)0xFFFF) - Tsich_cal[0];
	tsich_val[1] = (TSI0->CNTR7 & (INT32U)0xFFFF) - Tsich_cal[1];
	tsich_val[2] = (TSI0->CNTR7 >> 16)            - Tsich_cal[2];
	tsich_val[3] = (TSI0->CNTR9 & (INT32U)0xFFFF) - Tsich_cal[3];
	
	for (i=0; i<4; i++) {
		for (j=i+1; j<4; j++) {
			if ((tsich_val[i]>TSI_TRIG_THR) && (tsich_val[j]>TSI_TRIG_THR)) {
				key_val = cKeyTable[k];
				kcnt++;
			}
			k++;
		}
	}
	
	if (kcnt == 0) {
		key_val = 0;
	}
	else if (kcnt > 1) {		// muti key press
		key_val = -1;
	}
	
	sKeyVal = key_val;
}

