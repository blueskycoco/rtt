#ifndef __DATETIME_H_
#define __DATETIME_H_
#include "type.h"
#include "rtc.h"

typedef struct 
{
  INT32U week;
  INT32U year;
  INT32U month;
  INT32U day;
  INT32U hours;
  INT32U minute;
  INT32U second;
}DateTime ;

void RTC_GetTime(DateTime *tp);	
INT8U RTC_SetTime(DateTime *tp);

BOOL Date_Init(void);
INT32U DateTimeToSeconds(DateTime *ct);

#endif
