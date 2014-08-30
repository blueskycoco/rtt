#ifndef __FLEXTIMER_h
#define __FLEXTIMER_h

#include "kinetis.h"
#include "type.h"

BOOL FTM_ClkEn ( FTM_Type *FTMx );
BOOL FTM_ClkDis ( FTM_Type *FTMx );
BOOL FTM_Stop ( FTM_Type *FTMx );
BOOL FTM_PWM_Init ( FTM_Type *FTMx, INT32U rate );
BOOL FTM_PWM_Set ( FTM_Type *FTMx, INT8U channel, INT32U duty );

#endif
