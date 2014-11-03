#ifndef __RTC_H
#define __RTC_H

#include "kinetis.h"
#include "type.h"
BOOL RTC_ClkEn (void);
BOOL RTC_ClkDis (void);
BOOL RTC_Init (void);
BOOL RTC_Start ( void );
BOOL RTC_Stop ( void );
BOOL RTC_SetSec ( INT32U sec );
INT32U RTC_GetSec ( void );
BOOL RTC_SetAlarm ( INT32U alarm );

#endif
