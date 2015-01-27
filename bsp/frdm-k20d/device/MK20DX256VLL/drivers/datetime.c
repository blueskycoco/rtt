#include "datetime.h"

#define SecsPerComYear  3153600    //(365*3600*24)
#define SecsPerLeapYear 31622400//(366*3600*24)
#define SecsPerFourYear 126230400//((365*3600*24)*3+(366*3600*24))
#define SecsPerDay      (3600*24)

const INT32S Year_Secs_Accu[5] =
{
	0,
	31622400,
	63158400,
	94694400,
	126230400
};

const INT32S Month_Secs_Accu_C[13] = 
{
	0,
	2678400,
	5097600,
	7776000,
	10368000,
	13046400,
	15638400,
	18316800,
	20995200,
	23587200,
	26265600,
	28857600,
	31536000
};

const INT32S Month_Secs_Accu_L[13] = 
{
	0,
	2678400,
	5184000,
	7862400, 
	10454400,
	13132800,
	15724800,
	18403200,
	21081600,
	23673600,
	26352000,
	28944000,
	31622400
};

const INT16U Month_Days_Accu_C[13] = {0,31,59,90,120,151,181,212,243,273,304,334,365};
const INT16U Month_Days_Accu_L[13] = {0,31,60,91,121,152,182,213,244,274,305,335,366};

#define SecsPerDay (3600*24)

BOOL Date_Init(void)
{	
	RTC_Init();	
	return TRUE;
}

INT32U DateTimeToSeconds(DateTime *ct)
{
	INT32U Tmp_Year=0xFFFF, Tmp_Month=0xFF, Tmp_Date=0xFF;
	INT32U LeapY, ComY, TotSeconds, TotDays;
	INT32U Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;
	
	Tmp_Year  = ct->year/*+2000*/;
	Tmp_Month = ct->month;
	Tmp_Date  = ct->day;
	Tmp_HH    = ct->hours;
	Tmp_MM    = ct->minute;
	Tmp_SS    = ct->second;
	
	if(Tmp_Year==2000)
		LeapY = 0;
	else
		LeapY = (Tmp_Year - 2000 -1)/4 +1;
	
	ComY = (Tmp_Year - 2000)-(LeapY);
	
	if (Tmp_Year%4)
	{
		//common year
		TotDays = LeapY*366 + ComY*365 + Month_Days_Accu_C[Tmp_Month-1] + (Tmp_Date-1); 
	}
	else
	{
		//leap year
		TotDays = LeapY*366 + ComY*365 + Month_Days_Accu_L[Tmp_Month-1] + (Tmp_Date-1); 
	}
	TotSeconds = TotDays*SecsPerDay + (Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS);
	return TotSeconds;
}

INT8U RTC_SetTime(DateTime *tp)
{
	RTC_SetSec(DateTimeToSeconds(tp));
	return 0;
}

void RTC_GetTime(DateTime *tp)
{ 
	INT32U TY = 0;
	INT32U TM = 1;
	INT32U TD = 0;
	INT32S Num4Y,NumY, OffSec, Off4Y = 0;
	INT32U i;
	INT32S NumDay;
	INT32S AllNumDay;
	//INT32S OffDay;
	INT32U THH = 0;
	INT32U TMM = 0;
	INT32U TSS = 0;
	INT32U TimeVar;
	
	TimeVar = RTC_GetSec();
	AllNumDay = TimeVar/SecsPerDay;
	Num4Y = TimeVar/SecsPerFourYear;
	OffSec = TimeVar%SecsPerFourYear;
	
	i=1;
	while(OffSec > Year_Secs_Accu[i++])
	{
		Off4Y++;
	}
	NumY = Num4Y*4 + Off4Y;
	TY = NumY;
	OffSec = OffSec - Year_Secs_Accu[i-2];
	i=0;
	if((TY+2000)%4)
	{// common year
		while(OffSec > Month_Secs_Accu_C[i++]);
		TM = i-1;
		OffSec = OffSec - Month_Secs_Accu_C[i-2];
	}
	else
	{// leap year
		while(OffSec > Month_Secs_Accu_L[i++]);
		TM = i-1;
		OffSec = OffSec - Month_Secs_Accu_L[i-2];
	}
	NumDay = OffSec/SecsPerDay;
	OffSec = OffSec%SecsPerDay;
	TD = NumDay+1;
	THH = OffSec/3600;
	TMM = (OffSec % 3600)/60;
	TSS = (OffSec % 3600)% 60;
	//
	tp->week = (AllNumDay-1) % 7;
	tp->year = TY+2000;
	tp->month = TM;
	tp->day = TD;
	tp->hours = THH;
	tp->minute = TMM;
	tp->second = TSS;  
}
