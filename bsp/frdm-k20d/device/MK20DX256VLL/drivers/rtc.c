#include "rtc.h"

BOOL RTC_ClkEn (void)
{
	SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;;
	return (TRUE);
}

BOOL RTC_ClkDis (void)
{
	SIM->SCGC6 &= ~SIM_SCGC6_RTC_MASK;;
	return (TRUE);
}
BOOL RTC_Start ( void )
{
    RTC->SR |= RTC_SR_TCE_MASK;
	return (TRUE);
}

BOOL RTC_Stop ( void )
{
    RTC->SR &= ~RTC_SR_TCE_MASK;
	return (TRUE);
}

BOOL RTC_Init (void)
{
    RTC_ClkEn();
    if (RTC->TSR == 0) {
		RTC->CR  = RTC_CR_SWR_MASK;
		RTC->CR  &= ~RTC_CR_SWR_MASK;
		RTC->TSR = 0;
		RTC->TAR = 0;
	}
    RTC->CR |= RTC_CR_OSCE_MASK;
    
    RTC->TCR = RTC_TCR_CIR(0) | RTC_TCR_TCR(0);  
    RTC->TPR = 0;
	RTC_Start();
	
	return (TRUE);
}

BOOL RTC_SetSec ( INT32U sec )
{
	RTC_Stop();
	RTC->TSR = sec;
	RTC_Start();
	return (TRUE);
}
INT32U RTC_GetSec ( void )
{
	return (RTC->TSR);
}
BOOL RTC_SetAlarm ( INT32U alarm )
{
    RTC->TAR = alarm;
	return (TRUE);
}

