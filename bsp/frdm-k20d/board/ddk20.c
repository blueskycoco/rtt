#include "MK20D7.h"
#include <rtthread.h>
#include "ddk20.h"

/*adc*/
int ADC_Clk( ADC_Type *ADCx,int on )
{
	int ret = 1;
	switch ((unsigned long)ADCx) {
		case (unsigned long)ADC0:
			if(on)
				SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
			else
				SIM->SCGC6 &= ~SIM_SCGC6_ADC0_MASK;	
			break;
		case (unsigned long)ADC1:
			if(on)
				SIM->SCGC3 |= SIM_SCGC3_ADC1_MASK;
			else
				SIM->SCGC3 &= ~SIM_SCGC3_ADC1_MASK;	
			break;
		default:
			ret = 0;
	}
	return (ret);
}

int ADC_Init ( ADC_Type *ADCx )
{	
	ADCx->CFG1       |=	ADC_CFG1_MODE(3)|				// 16bit conversion
						ADC_CFG1_ADIV(3)|				// div 8
						ADC_CFG1_ADICLK(0)|				// clock source:bus clock
						ADC_CFG1_ADLSMP_MASK;			// long sample time
	
	ADCx->CFG2       |= ADC_CFG2_ADLSTS(0)|				// 20 extra ADC clock
						ADC_CFG2_ADHSC_MASK;			// hight speed
	return (1);
}

signed short ADC_GetCh ( ADC_Type *ADCx, unsigned char ch ,int type)
{
	signed short adcval;

	if(type==0)//diff getch
		ADCx->SC1[0]=ADC_SC1_DIFF_MASK|ADC_SC1_ADCH(ch);
	else//sgl getch
		ADCx->SC1[0]  =	ADC_SC1_ADCH(ch);		
	
	while (!(ADCx->SC1[0] & ADC_SC1_COCO_MASK)){;}
 	ADCx->SC1[0] &= ~ADC_SC1_COCO_MASK;

	adcval = ADCx->R[0];
	return (adcval);
}

/*bmp*/
 
/*-------------------------------------------------------------------------
|	unsigned short Rgb24to16(unsigned char r,unsigned char g,unsigned char b)
|
|
|
|-------------------------------------------------------------------------*/
unsigned short Rgb24to16(unsigned char r,unsigned char g,unsigned char b)
{
	unsigned short rgb16=0;					// RGB  565
	
	r &= 0xF8;
	rgb16 |= (unsigned short)r;
	rgb16 <<= 5;
	
	g &= 0xFC;
	rgb16 |= (unsigned short)g;
	rgb16 <<= 3;

	b >>= 3;
	rgb16 |= (unsigned short)b;
	return rgb16;
}
/*-------------------------------------------------------------------------
|	unsigned char CheckIsBmp(void *pBmp)
|
|
|
|-------------------------------------------------------------------------*/
int CheckIsBmp(void *pBmp)
{
	if(pBmp==0)
	{
		return 0;
	}
	if((*(unsigned char *)pBmp == 'B') && (*((unsigned char *)pBmp + 1) == 'M'))  // if the first two byte is ascll "BM" then that is a Bmp file
	{	
		return 1;
	}
	else
	{
		return 0;
	}
}
/*-------------------------------------------------------------------------
|	int GetBmpFSize(void *pBmp ,unsigned long FSize)
|
|
|
|-------------------------------------------------------------------------*/
int GetBmpFSize(void *pBmp ,unsigned long *FSize)
{
	if(FSize==0)
	{
		return 0;
	}
	if(!CheckIsBmp(pBmp))
	{
		return 0;
	}

	*FSize = *(unsigned long *)((unsigned char *)pBmp + 2);

	return 1;
}
/*-------------------------------------------------------------------------
|	int GetBmpHWSize(void *pBmp ,unsigned long WBmp ,unsigned long HBmp)
|
|
|
|-------------------------------------------------------------------------*/
int GetBmpHWSize(void *pBmp ,unsigned long *WBmp ,unsigned long *HBmp)
{
	if(WBmp==0 || HBmp==0)
	{
		return 0;
	}
	if(!CheckIsBmp(pBmp))
	{
		return 0;
	}

	memcpy(WBmp ,(unsigned char *)pBmp+18 ,4);
	memcpy(HBmp ,(unsigned char *)pBmp+22 ,4);

	return 1;
}
/*-------------------------------------------------------------------------
|	int GetColorOffser(void *pBmp ,unsigned long ColorOffset)
|
|
|
|-------------------------------------------------------------------------*/
int GetColorOffser(void *pBmp ,unsigned long *ColorOffset)
{
	if(ColorOffset==0)
	{
		return 0;
	}
	if(!CheckIsBmp(pBmp))
	{
		return 0;
	}
	memcpy(ColorOffset ,(unsigned char *)pBmp+10 ,4);

	return 1;
}
/*-------------------------------------------------------------------------
|	int GetBmpInfo(void *pBmp ,BmpInfoHeader *BmpInfo)
|
|
|
|-------------------------------------------------------------------------*/
int GetBmpInfo(void *pBmp ,BmpInfoHeader *BmpInfo)
{
	if(BmpInfo==0)
	{
		return 0;
	}
	if(!CheckIsBmp(pBmp))
	{
		return 0;
	}
	
	memcpy(BmpInfo ,(unsigned char *)pBmp+14 ,sizeof(BmpInfoHeader));
	
	return 1;		
}
/*-------------------------------------------------------------------------
|	int DrawBmp(void *pBmp ,)
|
|
|
|-------------------------------------------------------------------------*/
int DrawBmp(unsigned short x ,unsigned short y, void *pBmp)
{
	unsigned long w_temp,WBmp;
	unsigned long h_temp,HBmp;
	unsigned long ColorOffset;
	unsigned short color_temp;
	unsigned long t;
	unsigned char  *offset_temp;

	if(!GetBmpHWSize(pBmp ,&WBmp ,&HBmp)) {
		return 0;
	}
	
	if(!GetColorOffser(pBmp ,&ColorOffset)) {
		return 0;
	}
	
	t = 4-(WBmp*3)%4;													//每行补齐字节数   每行数据为4的整数倍
	if (4==t) {t = 0;}
	
	for (h_temp = 0 ;h_temp < HBmp ; h_temp++) {
		offset_temp = (unsigned char *)pBmp + ColorOffset + (HBmp-h_temp-1)*(3*WBmp+t);

		for (w_temp = 0 ;w_temp < WBmp ; w_temp++) {	
			color_temp = (unsigned short)Rgb24to16(*(offset_temp+w_temp*3+2) ,*(offset_temp+w_temp*3+1) ,*(offset_temp+w_temp*3));
			//DRAW_PIXEL(x+w_temp ,y+h_temp , color_temp);
			lcd_set_pixel(color_temp,x+w_temp,y+h_temp);
		}
	}
	
	return 1;
}

/*cmp*/
int CMP_Clk( CMP_Type *CMPx,int on)
{
	int retval = 1;
	
	switch ((unsigned long )CMPx) {
		case (unsigned long )CMP0:
		case (unsigned long )CMP1:
		case (unsigned long )CMP2:
			if(on)
				SIM->SCGC4 |= SIM_SCGC4_CMP_MASK;
			else
				SIM->SCGC4 &= ~SIM_SCGC4_CMP_MASK;
			break;
		default:
			retval = 0;
			break;
	}
	
	return (retval);
}

int CMP_Init ( CMP_Type *CMPx, unsigned char Vref, unsigned char inp_n, unsigned char inm_n)
{
	int retval = 1;
	
	CMPx->CR0 = 0x00;
	CMPx->CR1 = 0x00;
	CMPx->FPR = 0x00;
	
	CMPx->SCR = CMP_SCR_CFF_MASK
	           |CMP_SCR_CFR_MASK;
			   
	CMPx->DACCR = 0x00;
	CMPx->MUXCR = 0x00;
	
	CMPx->CR0 = 0x00;
	CMPx->CR1 = 0x15;
	
	CMPx->FPR = 0x00;
	CMPx->SCR = 0x1E;
	
	if (Vref==0) {
		CMPx->DACCR &= ~CMP_DACCR_VRSEL_MASK;
	}
	else if (Vref==1) {
		CMPx->DACCR |= CMP_DACCR_VRSEL_MASK;
	}
	else {
		retval = 0;
	}

	if (inp_n<8 && inm_n<8) {
		CMPx->MUXCR |= CMP_MUXCR_PSEL(inp_n)
					  |CMP_MUXCR_MSEL(inm_n);
	}
	else {
		retval = 0;
	}
	
	CMPx->CR1 |= CMP_CR1_OPE_MASK; 
		
	return (retval);
}

int CMP_DacSetVal ( CMP_Type *CMPx, unsigned char stage )
{
	int retval = 1;
	
	CMPx->DACCR &= ~CMP_DACCR_VOSEL_MASK;
	CMPx->DACCR |= CMP_DACCR_VOSEL(stage);
	CMPx->DACCR |= CMP_DACCR_DACEN_MASK;
	
	return (retval);
}

int CMP_IntCtl( CMP_Type *CMPx,int on)
{
	int retval = 1;
	IRQn_Type irq_n = CMP0_IRQn;
	
	switch ((unsigned long )CMPx) {
		case (unsigned long )CMP0:
			break;
		case (unsigned long )CMP1:
			irq_n += 1;
			break;
		case (unsigned long )CMP2:
			irq_n += 2;
			break;
		default:
			retval = 0;
	}
	if(on)
	{
		CMPx->SCR |=  CMP_SCR_IEF_MASK  | CMP_SCR_IER_MASK; 
		NVIC_EnableIRQ(irq_n);   
		CMPx->SCR |= CMP_SCR_CFR_MASK;
		CMPx->SCR |= CMP_SCR_CFF_MASK;
	}
	else
	{
		CMPx->SCR &=  ~(CMP_SCR_IEF_MASK  | CMP_SCR_IER_MASK); 
		NVIC_DisableIRQ(irq_n);
	}
	return (retval);
}

int CMP_GetOutVal ( CMP_Type *CMPx )
{
	int retval = 0;
	
	if (CMPx->SCR & CMP_SCR_COUT_MASK)
		retval = 1;
		
	return (retval);
}

void CMP0_IRQHandler (void)
{
	CMP0->SCR |= CMP_SCR_CFR_MASK
	            |CMP_SCR_CFF_MASK;
}

void CMP1_IRQHandler (void)
{
	CMP1->SCR |= CMP_SCR_CFR_MASK
	            |CMP_SCR_CFF_MASK;
}

void CMP2_IRQHandler (void)
{
	CMP2->SCR |= CMP_SCR_CFR_MASK
	            |CMP_SCR_CFF_MASK;
}
/*dac*/
BOOL DAC_ClkEn ( DAC_Type *DACx )
{
	BOOL retval = TRUE;
	SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
	return (retval);
}

BOOL DAC_ClkDis ( DAC_Type *DACx )
{
	BOOL retval = TRUE;
	SIM->SCGC2 &= ~SIM_SCGC2_DAC0_MASK;
	return (retval);
}

BOOL DAC_Init ( DAC_Type *DACx)
{  
	BOOL retval = TRUE;
	
	DACx->C0 |= DAC_C0_DACEN_MASK
	           |DAC_C0_DACRFS_MASK
	           |DAC_C0_DACTRGSEL_MASK;
	
//	DACx->C1 |=	DAC_C1_DACBFEN_MASK
//	           |DAC_C1_DACBFMD(1);
	
	return (retval);
}

BOOL DAC_Convert ( DAC_Type *DACx, INT16U stage )
{
	BOOL retval = TRUE;
	
	DACx->DAT[0].DATL = stage &  0xFF;
	DACx->DAT[0].DATH = stage >> 8;
	DACx->C0 |=	DAC_C0_DACSWTRG_MASK;
	
	return (retval);
}
/*rtc*/
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

/*flex bus*/
BOOL FLEXBUS_ClkEn (void)
{
	SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
	return (TRUE);
}

BOOL FLEXBUS_ClkDis (void)
{
	SIM->SCGC7 &= ~SIM_SCGC7_FLEXBUS_MASK;
	return (TRUE);
}

BOOL FLEXBUS_Init ( INT32U div_n )
{
	//fb clock divider 3
	SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(div_n);		// div_n
	
	FB->CSPMCR = FB_CSPMCR_GROUP5(2)
				|FB_CSPMCR_GROUP4(1)
				|FB_CSPMCR_GROUP3(2)
				|FB_CSPMCR_GROUP2(2)
				|FB_CSPMCR_GROUP1(0)
				;
	
	//Set Base address
	FB->CS[2].CSAR = CS2_BASE;			// PSRAM

	FB->CS[2].CSCR = FB_CSCR_PS(3)      // 16-bit port
					|FB_CSCR_AA_MASK    // auto-acknowledge
//					|FB_CSCR_EXALE_MASK
 					|FB_CSCR_WRAH(5)//2
 					|FB_CSCR_RDAH(5)//2
// 				 	|FB_CSCR_SWSEN_MASK
// 				 	|FB_CSCR_SWS(2)
					|FB_CSCR_BLS_MASK
					|FB_CSCR_ASET(1)  // assert chip select on second clock edge after address is asserted
					|FB_CSCR_WS(1)    // 1 wait state - may need a wait state depending on the bus speed
					;
	
	FB->CS[2].CSMR = FB_CSMR_V_MASK    //Enable cs signal
					|FB_CSMR_BAM(0x007F);
//--------------------------------------------------------------------------------------------------------------
	//Set Base address
	FB->CS[0].CSAR = CS0_BASE;			// LCD

	FB->CS[0].CSCR = FB_CSCR_PS(3)      // 16-bit port	//ram
					|FB_CSCR_AA_MASK    // auto-acknowledge
					|FB_CSCR_BLS_MASK	// right aligned
					|FB_CSCR_BEM_MASK	// be signal is asserted for rw acess
					|FB_CSCR_ASET(1)    // assert chip select on second clock edge after address is asserted
					|FB_CSCR_EXTS_MASK
					|FB_CSCR_WRAH(0)
					|FB_CSCR_RDAH(0)
					|FB_CSCR_WS(1)      // 1 wait state - may need a wait state depending on the bus speed
					;

	FB->CS[0].CSMR = FB_CSMR_V_MASK;    //Enable cs signal
//--------------------------------------------------------------------------------------------------------------
	return (TRUE);
}

BOOL Mem_Rd ( INT16U *dat, INT32U offset )
{
	BOOL ret = TRUE;
	
	if ( offset < PSRAM_CY ) {
		*dat = *(INT16U*)(CS2_BASE + (offset<<1));
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}

BOOL Mem_Wr ( INT16U dat, INT32U offset )
{
	BOOL ret = TRUE;
		
	if ( offset < PSRAM_CY ) {
		*(INT16U*)(CS2_BASE + (offset<<1)) = dat;
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}

/*flex can*/
INT32U CAN_MBRxflag[2] = { 0 };
INT32U Rx_mb  [2]    = { 0 };
INT32U Tx_rtr_mb [2] = { 0 };
INT32U Tx_mb  [2]    = { 0 };
INT32U Rx_rtr_mb [2] = { 0 };

CAN_msg_t CAN_MsgBox[32] = {0};

BOOL CAN_ClkEn (CAN_Type *CANx)
{
	BOOL retval = TRUE;
	switch ((INT32U)CANx) {
		case CAN0_BASE:
			SIM->SCGC6 |= SIM_SCGC6_FLEXCAN0_MASK;
			break;
//		case CAN1_BASE:
//			SIM->SCGC3 |= SIM_SCGC3_FLEXCAN1_MASK;
//			break;
		default:
			retval = FALSE;
	}
	return (retval);
}

BOOL CAN_ClkDis (CAN_Type *CANx)
{
	BOOL retval = TRUE;
	switch ((INT32U)CANx) {
		case CAN0_BASE:
			SIM->SCGC6 &= ~SIM_SCGC6_FLEXCAN0_MASK;
			break;
//		case CAN1_BASE:
//			SIM->SCGC3 &= ~SIM_SCGC3_FLEXCAN1_MASK;
//			break;
		default:
			retval = FALSE;
	}
	return (retval);
}

BOOL CAN_SetTiming (CAN_Type *CANx, INT32U presdiv, INT8U prop_seg, INT8U pseg1, INT8U pseg2)
{
	BOOL retval = TRUE;
	INT8U rjw;

	/* Check input parameters are in expected boundaries                        */
	if ((!presdiv)  || (presdiv  > 256)  ||
		(!pseg1)    || (pseg1    >   8)  || 
		(!pseg2)    || (pseg2    >   8)  ||
		(!prop_seg) || (prop_seg >   8))  {
		return (FALSE);
	}

	/* Calculate other needed values                                            */
	rjw = (((pseg1 + prop_seg) < pseg2) ? (pseg1 + prop_seg) : (pseg2));
	if (rjw > 4)  rjw = 4;
	if (!rjw)     rjw = 1;
	CANx->CTRL1 &= ~(CAN_CTRL1_PROPSEG_MASK
					|CAN_CTRL1_PSEG2_MASK
					|CAN_CTRL1_PSEG1_MASK
					|CAN_CTRL1_RJW_MASK
					|CAN_CTRL1_PRESDIV_MASK);
	
	CANx->CTRL1 |=  (CAN_CTRL1_PRESDIV(presdiv-1)
					|CAN_CTRL1_RJW(rjw-1)
					|CAN_CTRL1_PSEG1(pseg1-1)
					|CAN_CTRL1_PSEG2(pseg2-1)
					|CAN_CTRL1_SMP_MASK
					|CAN_CTRL1_PROPSEG(prop_seg-1));

	return (retval);
}

BOOL CAN_Init (CAN_Type *CANx, INT32U baud)
{
	BOOL retval;
	INT8U i;
	
	retval = CAN_ClkEn (CANx);
	if (retval == TRUE) {
		CANx->CTRL1 |=  CAN_CTRL1_CLKSRC_MASK;      /* CAN Engine Clock Src is Bus Clock  */		
		CANx->MCR   &= ~CAN_MCR_MDIS_MASK;          /* Enable FlexCAN module              */
		CANx->MCR   |=  CAN_MCR_SOFTRST_MASK;       /* Start Soft Reset                   */
		while (CANx->MCR & CAN_MCR_SOFTRST_MASK);   /* Wait for reset to finish           */
		CANx->MCR   |=  CAN_MCR_FRZ_MASK;          	/* Freeze enable                      */
		CANx->MCR   |=  CAN_MCR_HALT_MASK;          /* Halt FlexCAN                       */
		while (!(CANx->MCR & CAN_MCR_FRZACK_MASK)); /* Wait for freeze mode acknowledge   */
		
		CANx->CTRL2 &=~(CAN_CTRL2_RFFN_MASK |       /* RFFN = 0, Number of Rx FIFO Filters*/
					    CAN_CTRL2_RRS_MASK);        /* RRS = 0, Remote Request generated  */
		CANx->CTRL2 |=  CAN_CTRL2_EACEN_MASK ;      /* EACEN = 0, IDE and RTR compared    */
		CANx->MCR   |=  CAN_MCR_SRXDIS_MASK |       /* SRXDIS = 1 (Self Reception Disable)*/
					    CAN_MCR_IRMQ_MASK |         /* IRQM = 1 (Individual Rx Msk and Q) */
					    CAN_MCR_MAXMB(15) ;         /* Number of Mailboxes is 16          */
		
		retval = CAN_SetTiming(CANx, (CAN_PCLK/baud/12), 2+1, 3+1, 3+1);
		if (retval) {
			for (i = 0; i < 16; i++) {              /* Disable all Mailboxes              */
				CANx->MB[i].CS = 0;
				CANx->MB[i].ID = 0;
				CANx->MB[i].WORD0 = 0;
				CANx->MB[i].WORD1 = 0;
				CANx->RXIMR[i] = 0x1FFFFFFF;
			}
			for (i =0; i<2; i++) {
				Rx_mb[i]     = 0;
				Rx_rtr_mb[i] = 0;
				Tx_mb[i]     = 0;
				Tx_rtr_mb[i] = 0;
				CAN_MBRxflag[i] = 0;
			}
			
			CANx->MCR &= ~CAN_MCR_HALT_MASK;        /* Exit halt mode                     */
			while (CANx->MCR & CAN_MCR_FRZACK_MASK);/* Wait for freeze mode to exit       */
			while (CANx->MCR & CAN_MCR_NOTRDY_MASK);/* Wait for syn to bus                */
		}
		else {
			retval = FALSE;
		}
	}

	return (retval);
}

BOOL CAN_DeInit (CAN_Type *CANx)
{
	BOOL retval;
	
	retval = CAN_ClkDis (CANx);
	if (retval == TRUE) {
		;
	}
	
	return (retval);
}

BOOL CAN_Loopback (CAN_Type *CANx, INT8U val)
{
	(val==0) ? (CANx->CTRL1 &= ~CAN_CTRL1_LPB_MASK) : (CANx->CTRL1 |= CAN_CTRL1_LPB_MASK);
	return (TRUE);
}

BOOL CAN_EnChanelInterrupt(CAN_Type *CANx, INT8U ch)
{
	CANx->IMASK1 |= CAN_IMASK1_BUFLM(1<<ch);
	return (0);
}

BOOL CAN_DisChanelInterrupt(CAN_Type *CANx, INT8U ch)
{
	CANx->IMASK1 &= ~CAN_IMASK1_BUFLM(1<<ch);
	return (0);
}

BOOL CAN_Wr (CAN_Type *CANx, INT8U ch, CAN_msg_t *msg)
{
	INT32U  cs; 
	INT8U remote_type, standard_id;
	INT8U ctrl;

	remote_type = (msg->type   != DATA_FRAME);
	standard_id = (msg->format == STANDARD_FORMAT);
	cs          =  0;
	
	ctrl = (CANx == CAN0) ? 0 : 1;
	
	/* Clear interrupt if it is active                                          */
	if (CANx->IFLAG1 & (1 << ch)) CANx->IFLAG1 = (1 << ch);

	/* Setup the identifier information                                         */
	if (standard_id)  {                                      /* Standard ID     */
		CANx->MB[ch].ID = CAN_ID_STD(msg->id);
	}
	else {                                               /* Extended ID     */
		CANx->MB[ch].ID = CAN_ID_EXT(msg->id);
		cs = CAN_CS_IDE_MASK ;
	}

	if (remote_type) 
	cs |= CAN_CS_RTR_MASK ;

	cs |= CAN_CS_CODE(0x0C) |            /* CODE = DATA or REMOTE              */
	      CAN_CS_DLC(msg->len);            /* DLC                                */

	/* Setup data bytes                                                         */
	CANx->MB[ch].WORD0 = CAN_WORD0_DATA_BYTE_0((INT32U)msg->data[0])| 
	                     CAN_WORD0_DATA_BYTE_1((INT32U)msg->data[1])| 
	                     CAN_WORD0_DATA_BYTE_2((INT32U)msg->data[2])| 
	                     CAN_WORD0_DATA_BYTE_3((INT32U)msg->data[3]);
	CANx->MB[ch].WORD1 = CAN_WORD1_DATA_BYTE_4((INT32U)msg->data[4])| 
	                     CAN_WORD1_DATA_BYTE_5((INT32U)msg->data[5])| 
	                     CAN_WORD1_DATA_BYTE_6((INT32U)msg->data[6])| 
	                     CAN_WORD1_DATA_BYTE_7((INT32U)msg->data[7]);
	
	if (remote_type) { 
		Tx_rtr_mb[ctrl] |= (1 << ch);     /* Send RTR                           */
		Rx_mb[ctrl]     |= (1 << ch);     /* Receive response                   */
		Rx_rtr_mb[ctrl] &=~(1 << ch);
		Tx_mb[ctrl]     &=~(1 << ch);
	}
	else { 
		Tx_mb[ctrl]     |= (1 << ch);     /* Send DATA                          */
		Tx_rtr_mb[ctrl] &=~(1 << ch);
		Rx_mb[ctrl]     &=~(1 << ch);
		Rx_rtr_mb[ctrl] &=~(1 << ch);
	}
	  
	CANx->IMASK1 |= (1 << ch);          /* Enable channel interrupt           */

	CANx->MB[ch].CS = cs;                 /* Activate transfer                  */

	return (TRUE);
}

BOOL CAN_Rd (CAN_Type *CANx, INT8U ch, CAN_msg_t *msg)
{
	BOOL retval;
	INT32U cs;

	cs  = CANx->MB[ch].CS;

	while (cs & CAN_CS_CODE(0x01)) {              /* If BUST, wait for BUSY to end      */
		cs = CANx->MB[ch].CS;
	}

	/* Read identifier information                                              */
	if (!(cs & CAN_CS_IDE_MASK)) {                                  /* Standard ID    */
		msg->format =  0;
		msg->id     =  (CAN_ID_STD_MASK & CANx->MB[ch].ID) >> CAN_ID_STD_SHIFT;
	}  else  {                                                /* Extended ID    */
		msg->format =  1;
		msg->id     =  (CAN_ID_EXT_MASK & CANx->MB[ch].ID) >> CAN_ID_EXT_SHIFT;
	}

	/* Read type information                                                    */
	msg->type = DATA_FRAME;                              /* DATA   FRAME   */

	/* Read length (number of received bytes)                                   */
	msg->len  = (CANx->MB[ch].CS&CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;

	/* Read data bytes                                                          */
	msg->data[0] = (CANx->MB[ch].WORD0 >> CAN_WORD0_DATA_BYTE_0_SHIFT);
	msg->data[1] = (CANx->MB[ch].WORD0 >> CAN_WORD0_DATA_BYTE_1_SHIFT);
	msg->data[2] = (CANx->MB[ch].WORD0 >> CAN_WORD0_DATA_BYTE_2_SHIFT);
	msg->data[3] = (CANx->MB[ch].WORD0 >> CAN_WORD0_DATA_BYTE_3_SHIFT);
	msg->data[4] = (CANx->MB[ch].WORD1 >> CAN_WORD1_DATA_BYTE_4_SHIFT);
	msg->data[5] = (CANx->MB[ch].WORD1 >> CAN_WORD1_DATA_BYTE_5_SHIFT);
	msg->data[6] = (CANx->MB[ch].WORD1 >> CAN_WORD1_DATA_BYTE_6_SHIFT);
	msg->data[7] = (CANx->MB[ch].WORD1 >> CAN_WORD1_DATA_BYTE_7_SHIFT);

	CANx->TIMER;                          /* Read timer to unlock Mailbox       */
	
	return (retval);
}

BOOL CAN_Set (CAN_Type *CANx, CAN_msg_t *msg)
{
	INT32U cs;
	INT8U ch;
	BOOL standard_id;
	INT8U ctrl;
	
	ctrl = (CANx == CAN0) ? 0 : 1;
	
	standard_id = (msg->format == STANDARD_FORMAT);
	cs          =  0;

	/* Clear interrupt if it is active                                          */
	if (CANx->IFLAG1 & (1 << ch)) CANx->IFLAG1 = (1 << ch);

	/* Setup the identifier information                                         */
	if (standard_id)  {                                      /* Standard ID     */
		CANx->MB[ch].ID = (INT32U)(msg->id << CAN_ID_STD_SHIFT);
	}  else  {                                               /* Extended ID     */
		CANx->MB[ch].ID = (INT32U)(msg->id << CAN_ID_EXT_SHIFT);
		cs = CAN_CS_IDE_MASK;
	}

	cs |= CAN_CS_CODE(0x0A) |            /* CODE = RANSWER                     */
	      CAN_CS_DLC(msg->len);            /* DLC                                */

	/* Setup data bytes                                                         */
	CANx->MB[ch].WORD0 = CAN_WORD0_DATA_BYTE_0((INT32U)msg->data[0])| 
	                     CAN_WORD0_DATA_BYTE_1((INT32U)msg->data[1])| 
	                     CAN_WORD0_DATA_BYTE_2((INT32U)msg->data[2])| 
	                     CAN_WORD0_DATA_BYTE_3((INT32U)msg->data[3]);
	CANx->MB[ch].WORD1 = CAN_WORD1_DATA_BYTE_4((INT32U)msg->data[4])| 
	                     CAN_WORD1_DATA_BYTE_5((INT32U)msg->data[5])| 
	                     CAN_WORD1_DATA_BYTE_6((INT32U)msg->data[6])| 
	                     CAN_WORD1_DATA_BYTE_7((INT32U)msg->data[7]);

	Rx_rtr_mb[ctrl] |= (1 << ch);       /* Receive RTR                        */
	Tx_mb[ctrl]     |= (1 << ch);       /* Transmit response                  */
	Tx_rtr_mb[ctrl] &=~(1 << ch);
	Rx_mb[ctrl]     &=~(1 << ch);
  
	CANx->IMASK1 |= (1 << ch);         /* Enable channel interrupt           */

	CANx->MB[ch].CS = cs;                 /* Activate transfer                  */

	return TRUE;
}

BOOL CAN_RxObject (CAN_Type *CANx, INT8U ch, INT32U id, INT32U object_para)
{
	BOOL retval = TRUE;
	INT32U cs; 
	//BOOL remote_type;
	BOOL standard_id;
	INT8U ctrl;
	
	ctrl = (CANx == CAN0) ? 0 : 1;
	//remote_type = (object_para != DATA_FRAME);
	standard_id = (object_para == STANDARD_FORMAT);
	cs          =  0;

	/* Clear interrupt if it is active                                          */
	if (CANx->IFLAG1 & (1 << ch))
		CANx->IFLAG1 = (1 << ch);
	
	/* Setup the identifier information                                         */
	if (standard_id)  {                                      /* Standard ID     */
		CANx->MB[ch].ID = (INT32U)(id << 18);
	}
	else  {                                               /* Extended ID     */
		CANx->MB[ch].ID = (INT32U)(id <<  0);
		cs = CAN_CS_IDE_MASK ;
	}
	
	cs |= CAN_CS_CODE(0x04);             /* CODE = EMPTY                       */
	
	Rx_mb[ctrl]     |= (1 << ch);       /* Receive DATA                       */
	Tx_rtr_mb[ctrl] &=~(1 << ch);
	Rx_rtr_mb[ctrl] &=~(1 << ch);
	Tx_mb[ctrl]     &=~(1 << ch);
  
	CANx->IMASK1  |= (1 << ch);           /* Enable channel interrupt           */
	
	CANx->MB[ch].CS = cs;                 /* Activate transfer                  */
	
	return (retval);
}

void CAN0_ORed_Message_buffer_IRQHandler (void)
{
	INT32U    iflag1, mb;

	iflag1 = CAN0->IFLAG1;
	while (iflag1) {
		for (mb = 0; mb < 32; mb ++) {
			if (iflag1 & (1 << mb)) {         /* If interrupt on Mailbox happened   */
				if ((Rx_rtr_mb[0] & Tx_mb[0]) & (1 << mb)) {
					/* If RTR received and answer sent    */
					/* Disable further RTR reception      */
					CAN0->MB[mb].CS &= ~CAN_CS_CODE(0x0F);    /* Set CODE to INACTIVE        */
				}

				else if (Rx_mb[0] & (1 << mb)) { /* If DATA received            */
					CAN_Rd (CAN0, mb, &CAN_MsgBox[mb]);/* Read received message         */
					CAN_MBRxflag[0] |= 1<<mb;
				}
				iflag1 &= ~(1 << mb);
				CAN0->IFLAG1 |= (1 << mb);
				if (!iflag1)                    /* If all interrupts were handled     */
					return;
			}
		}
		iflag1 = CAN0->IFLAG1;              /* Allow processing of new interrupts */
	}
}

/*flex timer*/
BOOL FTM_ClkEn ( FTM_Type *FTMx )
{
	BOOL ret = TRUE;
	switch ( (INT32U)FTMx ) {
		case (INT32U)FTM0:
			SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
			break;
		case (INT32U)FTM1:
			SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
			break;
		case (INT32U)FTM2:
			SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL FTM_ClkDis ( FTM_Type *FTMx )
{
	BOOL ret = TRUE;
	switch ( (INT32U)FTMx ) {
		case (INT32U)FTM0:
			SIM->SCGC6 &= ~SIM_SCGC6_FTM0_MASK;
			break;
		case (INT32U)FTM1:
			SIM->SCGC6 &= ~SIM_SCGC6_FTM1_MASK;
			break;
		case (INT32U)FTM2:
			SIM->SCGC3 &= ~SIM_SCGC3_FTM2_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL FTM_Stop ( FTM_Type *FTMx )
{
	FTMx->SC &= ~FTM_SC_CLKS_MASK;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

BOOL FTM_PWM_Init ( FTM_Type *FTMx, INT32U rate )
{   
	INT8U ps;
	INT32U mod;
	ps = (SystemCoreClock/rate)/65535;
	if ( ps >= 4 ) ps = 4;
	
	mod = (SystemCoreClock/(rate*(1<<ps)));  
	if ( mod > 0xFFFF ) mod = 0xFFFF;         
		
	FTMx->SC    &= ~FTM_SC_CLKS_MASK;
	FTMx->SC    |= FTM_SC_CLKS(1);
	FTMx->SC    &= ~FTM_SC_PS_MASK;
	FTMx->SC    |= FTM_SC_PS(ps);
	FTMx->CNT   =  0x0;
	FTMx->MOD   =  mod;
	FTMx->CNTIN =  0x0;
	
	FTMx->COMBINE = FTM_COMBINE_COMBINE1_MASK
					|FTM_COMBINE_COMP1_MASK
					|FTM_COMBINE_DTEN1_MASK
					|FTM_COMBINE_SYNCEN1_MASK;
	
	for ( mod=0; mod<1000000; mod++ );
	
//	FTMx->CONTROLS[0].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
//	FTMx->CONTROLS[0].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
//	FTMx->CONTROLS[0].CnV  =  (FTMx->MOD)/2;
//	FTMx->CONTROLS[1].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
//	FTMx->CONTROLS[1].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
//	FTMx->CONTROLS[1].CnV  =  (FTMx->MOD)/2;
	
	FTMx->CONTROLS[2].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
	FTMx->CONTROLS[2].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
	FTMx->CONTROLS[2].CnV  =  (FTMx->MOD)/2;
	FTMx->CONTROLS[3].CnSC &= ~(FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK|FTM_CnSC_MSA_MASK|FTM_CnSC_ELSB_MASK );
	FTMx->CONTROLS[3].CnSC |= (FTM_CnSC_MSB_MASK|FTM_CnSC_ELSA_MASK );
	FTMx->CONTROLS[3].CnV  =  (FTMx->MOD)/2;
	return (TRUE);
}

BOOL FTM_PWM_Set ( FTM_Type *FTMx, INT8U channel, INT32U duty )
{
	INT32U cv;
	if ( duty >= 10000 ) duty = 10000;
	cv = (FTMx->MOD)*(10000-duty)/10000;
	FTMx->CONTROLS[channel].CnV = cv;
	return (TRUE);
}

/*i2c*/
BOOL I2C_ClkEn ( I2C_Type *I2Cx )
{
	BOOL ret = TRUE;
	switch ((INT32U)I2Cx) {
		case (INT32U)I2C0:
			SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
			break;
		case (INT32U)I2C1:
			SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
			break;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL I2C_ClkDis ( I2C_Type *I2Cx )
{
	BOOL ret = TRUE;
	switch ((INT32U)I2Cx) {
		case (INT32U)I2C0:
			SIM->SCGC4 &= ~SIM_SCGC4_I2C0_MASK;
		case (INT32U)I2C1:
			SIM->SCGC4 &= ~SIM_SCGC4_I2C1_MASK;
		default:
			ret = FALSE;
	}
	return (ret);
}

BOOL I2C_Init ( I2C_Type *I2Cx )
{
	I2Cx->F = 0x70|
	I2C_F_MULT(0x02)|       	/* set MULT and ICR */
	I2C_F_ICR(0x1E);			// 400k bps
    I2Cx->C1 = I2C_C1_IICEN_MASK;       /* enable IIC */
//I2Cx->C1 |= I2C_C1_IICIE_MASK;;
	
	return (TRUE);
}

void Pause(void){
	INT32U n;
	for(n=0;n<I2C_PAUSE;n++) {
		__asm("nop");
	}
}

/******************************************************************************
**    function  : I2C_Wait 
**    parameter : type 0 = wait transfer done
**                type 1 = wait ack
******************************************************************************/
BOOL I2C_Wait ( I2C_Type *I2Cx, INT8U type )
{
	BOOL ret = TRUE;
	INT32U tm_out = I2C_TIME_OUT;
	
	if (type == 0) {
		while((--tm_out) && ((I2Cx->S & I2C_S_IICIF_MASK)==0));
		I2Cx->S |= I2C_S_IICIF_MASK;
		if (tm_out == 0)
			ret = FALSE;
	}
	else {
		while((--tm_out) && ((I2Cx->S & I2C_S_RXAK_MASK)!=0));
		if (tm_out == 0)
			ret = FALSE;
	}

	return (ret);
}

BOOL I2C_Write ( I2C_Type *I2Cx, INT8U slvaddr, INT8U subaddr, INT8U *buff, INT32U len )
{
	BOOL ret = TRUE;
	
	if ((I2Cx->S & I2C_S_BUSY_MASK)==0) {
		//start
		I2Cx->C1 |= I2C_C1_TX_MASK
					|I2C_C1_MST_MASK;
		//slave addr
		I2Cx->D = (slvaddr<<1)| MWSR;
		//wait
		ret &= I2C_Wait(I2Cx, 0);
		ret &= I2C_Wait(I2Cx, 1);
		
		//sub addr
		I2Cx->D = subaddr;
		//wait
		ret &= I2C_Wait(I2Cx, 0);	
		ret &= I2C_Wait(I2Cx, 1);
		
		//write data
		while(len-- != 0) {
			I2Cx->D = *buff++;
			ret &= I2C_Wait(I2Cx, 0);		
			ret &= I2C_Wait(I2Cx, 1);
		}
		//stop
		I2Cx->C1 &= ~I2C_C1_MST_MASK;
		I2Cx->C1 &= ~I2C_C1_TX_MASK;
		Pause();
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}

BOOL I2C_Read ( I2C_Type *I2Cx, INT8U slvaddr, INT8U subaddr, INT8U *buff, INT32U len )
{
	BOOL ret = TRUE;
	
	if ((I2Cx->S & I2C_S_BUSY_MASK)==0) {
		//start
		I2Cx->C1 |= I2C_C1_TX_MASK
					|I2C_C1_MST_MASK;
		//slave addr
		I2Cx->D = (slvaddr<<1)| MWSR;
		//wait
		ret &= I2C_Wait(I2Cx, 0);	
		ret &= I2C_Wait(I2Cx, 1);
		
		//sub addr
		I2Cx->D = subaddr;
		//wait
		ret &= I2C_Wait(I2Cx, 0);
		ret &= I2C_Wait(I2Cx, 1);
		
		//restart
		I2Cx->C1 |= I2C_C1_RSTA_MASK;
		//slave addr
		I2Cx->D = (slvaddr<<1)| MRSW;
		//wait
		ret &= I2C_Wait(I2Cx, 0);	
		ret &= I2C_Wait(I2Cx, 1);
		
		//enter rx mode
		I2Cx->C1 &= ~I2C_C1_TX_MASK;						   
		
		if (len > 1) {
			I2Cx->C1 &= ~I2C_C1_TXAK_MASK;
		}
		else if (len == 1) {
			I2Cx->C1 |= I2C_C1_TXAK_MASK;
		}
		else {
			;
		}
		*buff = I2Cx->D;
			
		while (--len) {
			ret &= I2C_Wait(I2Cx, 0);
			if (len == 1) {
				I2Cx->C1 |= I2C_C1_TXAK_MASK;
			}
			else {
				I2Cx->C1 &= ~I2C_C1_TXAK_MASK;
			}
			*buff++ = I2Cx->D;
		}
		
		ret &= I2C_Wait(I2Cx, 0);

		I2Cx->C1 &= ~I2C_C1_MST_MASK;
		I2Cx->C1 &= ~I2C_C1_TX_MASK;
		
		*buff = I2Cx->D;
		Pause();
	}
	else {
		ret = FALSE;
	}
	
	return (ret);
}

/*i2s*/
BOOL I2c_Cs4322_RegRead(INT8U subaddr, INT8U *value)
{
	BOOL retval;
	retval = I2C_Read ( I2C1, CS43L22_I2C_ADDRESS, subaddr, value, 1 );
	return retval;
}

BOOL I2c_Cs4322_RegWrite(INT8U subaddr, INT8U value)
{
	BOOL retval;
	retval = I2C_Write ( I2C1, CS43L22_I2C_ADDRESS, subaddr, &value, 1 );
	return retval;
}

BOOL CS43L22_Init (void)
{
	BOOL ret;
	INT8U id,status;
	
	// check ID of CS43L22
	ret &= I2c_Cs4322_RegRead(0x01, &id);	
	id >>= 3;
	if(id != 0x1C)
		while(1); // error
	//  // Power Control 2: headphone A/B AutoSelect(LOW valid), 
	//  //                  speaker A AutoSelect(HIGH valid) & B Off
	//  I2c_Cs4322_RegWrite(0x04, 0x0D);
	// Power Control 2: headphone A/B AutoSelect(LOW valid), 
	//                  speaker A/B AutoSelect(HIGH valid)
	ret &= I2c_Cs4322_RegWrite(0x04, 0x05);
	//  // test purpose
	//  I2c_Cs4322_RegWrite(0x04, 0xC5);
	//  // test purpose
	//  I2c_Cs4322_RegWrite(0x04, 0x35);
	// Clocking Control: auto sample rate(Fs)
	ret &= I2c_Cs4322_RegWrite(0x05, 0x80);

	// Interface Control 1: slave, I2S, 16-bit data
	ret &= I2c_Cs4322_RegWrite(0x06, 0x07);
	// Interface Control 2: SCLK!=MCLK, speaker/headphone not inverted
	ret &= I2c_Cs4322_RegWrite(0x07, 0x00);
	// Pass Through Select A: no input selected
	ret &= I2c_Cs4322_RegWrite(0x08, 0x00);
	// Pass Through Select B: no input selected
	ret &= I2c_Cs4322_RegWrite(0x09, 0x00);

	//  // Play Back Control 2: speaker mute disabled(default)
	//  I2c_Cs4322_RegWrite(0x0F, 0x00);
	// Play Back Control 2: speaker mute disabled(default), mono enabled
	ret &= I2c_Cs4322_RegWrite(0x0F, 0x02);

	// PCM A Volume: PCM Mute disabled, Volume is 0db(default)
	ret &= I2c_Cs4322_RegWrite(0x1A, 0x00);
	// PCM B Volume: PCM Mute disabled, Volume is 0db(default)
	ret &= I2c_Cs4322_RegWrite(0x1B, 0x00);
	// Beep, Tone Config: Beep off, Tone control disabled(default)
	ret &= I2c_Cs4322_RegWrite(0x1E, 0x00);

	// Headphone A Volume: Headphone Volume is -6db
	ret &= I2c_Cs4322_RegWrite(0x22, (INT8U)(0-12));
	// Headphone B Volume: Headphone Volume is -6db
	ret &= I2c_Cs4322_RegWrite(0x23, (INT8U)(0-12));

	// check status of CS43L22
	ret &= I2c_Cs4322_RegRead(0x2E, &status);
	if(status & 0x40)
	// Serial Port Clock Error for CS43L22
		while(1);//return I2S_TEST_FAIL_2;

	// Charge Pump Frequency: 5 (default)
	ret &= I2c_Cs4322_RegWrite(0x34, 5<<4);

	// Power Control 1: power up
	ret &= I2c_Cs4322_RegWrite(0x02, 0x9E);
	// delay a short time to make reg writing finished

	return ret;
}

BOOL I2S_ClkEn ( I2S_Type *I2Sx )
{
	SIM->SCGC6 |= SIM_SCGC6_I2S_MASK;
	return TRUE;
}

void I2S_Init(void)
{
  /* SIM_SCGC6: SAI0=1 */
  SIM->SCGC6 |= SIM_SCGC6_I2S_MASK;                                                   
  /* I2S0->MCR: MOE=0 */
  I2S0->MCR &= (uint32_t)~(uint32_t)(I2S_MCR_MOE_MASK);                                                   
    while((I2S0->MCR & I2S_MCR_MOE_MASK) != 0U){} /* Wait for MCLK disable*/
  /* I2S0->MCR: MICS=0 */
  I2S0->MCR &= (uint32_t)~(uint32_t)(I2S_MCR_MICS(0x03));                                                   
  /* I2S0->MDR: FRACT=0,DIVIDE=9 */
  I2S0->MDR = (uint32_t)((I2S0->MDR & (uint32_t)~(uint32_t)(
              I2S_MDR_FRACT(0xFF) |
              I2S_MDR_DIVIDE(0x0FF6)
             )) | (uint32_t)(
              I2S_MDR_DIVIDE(0x09)
             ));                                                  
  /* I2S0->MCR: MOE=1 */
  I2S0->MCR |= I2S_MCR_MOE_MASK;                                                   
  /* I2S0->TCSR: TE=0,STOPE=0,DBGE=0,BCE=0,FR=1,SR=0,WSF=1,SEF=1,FEF=1,WSIE=0,SEIE=0,FEIE=0,FWIE=0,FRIE=0,FWDE=0,FRDE=0 */
  I2S0->TCSR = (uint32_t)((I2S0->TCSR & (uint32_t)~(uint32_t)(
               I2S_TCSR_TE_MASK |
               I2S_TCSR_STOPE_MASK |
               I2S_TCSR_DBGE_MASK |
               I2S_TCSR_BCE_MASK |
               I2S_TCSR_SR_MASK |
               I2S_TCSR_WSIE_MASK |
               I2S_TCSR_SEIE_MASK |
               I2S_TCSR_FEIE_MASK |
               I2S_TCSR_FWIE_MASK |
               I2S_TCSR_FRIE_MASK |
               I2S_TCSR_FWDE_MASK |
               I2S_TCSR_FRDE_MASK
              )) | (uint32_t)(
               I2S_TCSR_FR_MASK |
               I2S_TCSR_WSF_MASK |
               I2S_TCSR_SEF_MASK |
               I2S_TCSR_FEF_MASK
              ));                                                  
    while((I2S0->TCSR & I2S_TCSR_TE_MASK) != 0U){} /* Wait for transmitter disable*/
  /* I2S0->RCSR: RE=0,STOPE=0,DBGE=0,BCE=0,FR=1,SR=0,WSF=1,SEF=1,FEF=1,WSIE=0,SEIE=0,FEIE=0,FWIE=0,FRIE=0,FWDE=0,FRDE=0 */
  I2S0->RCSR = (uint32_t)((I2S0->RCSR & (uint32_t)~(uint32_t)(
               I2S_RCSR_RE_MASK |
               I2S_RCSR_STOPE_MASK |
               I2S_RCSR_DBGE_MASK |
               I2S_RCSR_BCE_MASK |
               I2S_RCSR_SR_MASK |
               I2S_RCSR_WSIE_MASK |
               I2S_RCSR_SEIE_MASK |
               I2S_RCSR_FEIE_MASK |
               I2S_RCSR_FWIE_MASK |
               I2S_RCSR_FRIE_MASK |
               I2S_RCSR_FWDE_MASK |
               I2S_RCSR_FRDE_MASK
              )) | (uint32_t)(
               I2S_RCSR_FR_MASK |
               I2S_RCSR_WSF_MASK |
               I2S_RCSR_SEF_MASK |
               I2S_RCSR_FEF_MASK
              ));                                                  
    while((I2S0->RCSR & I2S_RCSR_RE_MASK) != 0U){} /* Wait for receiver disable*/
  /* I2S0->TCR1: TFW=0 */
  I2S0->TCR1 &= (uint32_t)~(uint32_t)(I2S_TCR1_TFW(0x07));                                                   
  /* I2S0->RCR1: RFW=0 */
  I2S0->RCR1 &= (uint32_t)~(uint32_t)(I2S_RCR1_RFW(0x07));                                                   
  /* I2S0->TCR2: SYNC=0,BCS=0,BCI=0,MSEL=1,BCP=1,BCD=1,DIV=3 */
  I2S0->TCR2 = (uint32_t)((I2S0->TCR2 & (uint32_t)~(uint32_t)(
               I2S_TCR2_SYNC(0x03) |
               I2S_TCR2_BCS_MASK |
               I2S_TCR2_BCI_MASK |
               I2S_TCR2_MSEL(0x02) |
               I2S_TCR2_DIV(0xFC)
              )) | (uint32_t)(
               I2S_TCR2_MSEL(0x01) |
               I2S_TCR2_BCP_MASK |
               I2S_TCR2_BCD_MASK |
               I2S_TCR2_DIV(0x03)
              ));                                                  
  /* I2S0->RCR2: SYNC=0,BCS=0,BCI=0,MSEL=1,BCP=0,BCD=1,DIV=3 */
  I2S0->RCR2 = (uint32_t)((I2S0->RCR2 & (uint32_t)~(uint32_t)(
               I2S_RCR2_SYNC(0x03) |
               I2S_RCR2_BCS_MASK |
               I2S_RCR2_BCI_MASK |
               I2S_RCR2_MSEL(0x02) |
               I2S_RCR2_BCP_MASK |
               I2S_RCR2_DIV(0xFC)
              )) | (uint32_t)(
               I2S_RCR2_MSEL(0x01) |
               I2S_RCR2_BCD_MASK |
               I2S_RCR2_DIV(0x03)
              ));                                                  
  /* I2S0->TCR3: TCE=1,WDFL=0 */
  I2S0->TCR3 = (uint32_t)((I2S0->TCR3 & (uint32_t)~(uint32_t)(
               I2S_TCR3_TCE(0x02) |
               I2S_TCR3_WDFL(0x1F)
              )) | (uint32_t)(
               I2S_TCR3_TCE(0x01)
              ));                                                  
  /* I2S0->RCR3: RCE=0,WDFL=0 */
  I2S0->RCR3 &= (uint32_t)~(uint32_t)((I2S_RCR3_RCE(0x03) | I2S_RCR3_WDFL(0x1F)));                                                   
  /* I2S0->TCR4: FRSZ=1,SYWD=0x0F,MF=1,FSE=0,FSP=0,FSD=1 */
  I2S0->TCR4 = (uint32_t)((I2S0->TCR4 & (uint32_t)~(uint32_t)(
               I2S_TCR4_FRSZ(0x1E) |
               I2S_TCR4_SYWD(0x10) |
               I2S_TCR4_FSE_MASK |
               I2S_TCR4_FSP_MASK
              )) | (uint32_t)(
               I2S_TCR4_FRSZ(0x01) |
               I2S_TCR4_SYWD(0x0F) |
               I2S_TCR4_MF_MASK |
               I2S_TCR4_FSD_MASK
              ));                                                  
  /* I2S0->RCR4: FRSZ=1,SYWD=0,MF=0,FSE=0,FSP=0,FSD=0 */
  I2S0->RCR4 = (uint32_t)((I2S0->RCR4 & (uint32_t)~(uint32_t)(
               I2S_RCR4_FRSZ(0x1E) |
               I2S_RCR4_SYWD(0x1F) |
               I2S_RCR4_MF_MASK |
               I2S_RCR4_FSE_MASK |
               I2S_RCR4_FSP_MASK |
               I2S_RCR4_FSD_MASK
              )) | (uint32_t)(
               I2S_RCR4_FRSZ(0x01)
              ));                                                  
  /* I2S0->TCR5: WNW=0x0F,W0W=0x0F,FBT=0 */
  I2S0->TCR5 = (uint32_t)((I2S0->TCR5 & (uint32_t)~(uint32_t)(
               I2S_TCR5_WNW(0x10) |
               I2S_TCR5_W0W(0x10) |
               I2S_TCR5_FBT(0x1F)
              )) | (uint32_t)(
               I2S_TCR5_WNW(0x0F) |
               I2S_TCR5_W0W(0x0F)
              ));                                                  
  /* I2S0->RCR5: WNW=7,W0W=7,FBT=0 */
  I2S0->RCR5 = (uint32_t)((I2S0->RCR5 & (uint32_t)~(uint32_t)(
               I2S_RCR5_WNW(0x18) |
               I2S_RCR5_W0W(0x18) |
               I2S_RCR5_FBT(0x1F)
              )) | (uint32_t)(
               I2S_RCR5_WNW(0x07) |
               I2S_RCR5_W0W(0x07)
              ));                                                  
  /* I2S0->TMR: TWM=0 */
  I2S0->TMR = 0x00U;                                                   
  /* I2S0->RMR: RWM=0 */
  I2S0->RMR = 0x00U;                                                   
  /* I2S0->TCSR: TE=1,STOPE=0,DBGE=0,BCE=1,WSF=0,SEF=0,FEF=0,WSIE=0,SEIE=0,FEIE=0,FWIE=0,FRIE=0,FWDE=0,FRDE=0 */
  I2S0->TCSR = (uint32_t)((I2S0->TCSR & (uint32_t)~(uint32_t)(
               I2S_TCSR_STOPE_MASK |
               I2S_TCSR_DBGE_MASK |
               I2S_TCSR_WSF_MASK |
               I2S_TCSR_SEF_MASK |
               I2S_TCSR_FEF_MASK |
               I2S_TCSR_WSIE_MASK |
               I2S_TCSR_SEIE_MASK |
               I2S_TCSR_FEIE_MASK |
               I2S_TCSR_FWIE_MASK |
               I2S_TCSR_FRIE_MASK |
               I2S_TCSR_FWDE_MASK |
               I2S_TCSR_FRDE_MASK
              )) | (uint32_t)(
               I2S_TCSR_TE_MASK |
               I2S_TCSR_BCE_MASK
              ));                                                  
  /* I2S0->RCSR: RE=0,STOPE=0,DBGE=0,BCE=0,WSF=0,SEF=0,FEF=0,WSIE=0,SEIE=0,FEIE=0,FWIE=0,FRIE=0,FWDE=0,FRDE=0 */
  I2S0->RCSR &= (uint32_t)~(uint32_t)(
                I2S_RCSR_RE_MASK |
                I2S_RCSR_STOPE_MASK |
                I2S_RCSR_DBGE_MASK |
                I2S_RCSR_BCE_MASK |
                I2S_RCSR_WSF_MASK |
                I2S_RCSR_SEF_MASK |
                I2S_RCSR_FEF_MASK |
                I2S_RCSR_WSIE_MASK |
                I2S_RCSR_SEIE_MASK |
                I2S_RCSR_FEIE_MASK |
                I2S_RCSR_FWIE_MASK |
                I2S_RCSR_FRIE_MASK |
                I2S_RCSR_FWDE_MASK |
                I2S_RCSR_FRDE_MASK
               );                                                   
}

/*oled12864*/

static unsigned char sUpdate_mode = 1;

static unsigned char gui_disp_buf[GUI_LCM_YMAX/8][GUI_LCM_XMAX];

const unsigned char BIT_MASK[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
/* simple font: ' ', '0'~'9','a'~'z','A'~'Z' */
extern const unsigned char  FONTTYPE8_8[][8];

/*******************************************************************************
* Function Name  : LCD_Set_Update_Mode
* Description    : set mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void LCD_Set_Update_Mode(unsigned char mode)
{
	sUpdate_mode = mode;
}
/*******************************************************************************
* Function Name  : Delay
* Description    : Delay some time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Delay(void)
{
	unsigned char i=0x8;
	while(i--)
		__asm("nop");
}

/*******************************************************************************
* Function Name  : Reset_Delay
* Description    : Delay some time
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Reset_Delay(void)
{
	unsigned short i=0xff;
	while(i--)
		__asm("nop");
}

/*******************************************************************************
* Function Name  : Power_Delay
* Description    : Delay some time for power up.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Power_Delay(void)
{
	unsigned long i=0x4ffff;
	while(i--)
		__asm("nop");
}
/*******************************************************************************
* Function Name  : LCD_Init
* Description    : initialize lcd
* Input          : None.
* Return         : None.
*******************************************************************************/
void LCD_Initialize(void)
{
	Power_Delay();
	LCD_WriteCmd(Display_Off);
	Reset_Delay();
	// Resetting circuit
	LCD_WriteCmd(Reset_LCD);
	Reset_Delay();
	// LCD bias setting
	LCD_WriteCmd(Set_LCD_Bias_9);
	Reset_Delay();
	// ADC selection: display from left to right
	LCD_WriteCmd(Set_ADC_Normal);		
	Reset_Delay();
	// Common output state selection: display from up to down
	LCD_WriteCmd(COM_Scan_Dir_Reverse);
	Reset_Delay();
	
	LCD_WriteCmd(Power_Booster_On);
	Power_Delay(); // 50ms requried
	LCD_WriteCmd(Power_Regulator_On);
	Power_Delay(); // 50ms
	LCD_WriteCmd(Power_Follower_On);
	Power_Delay(); // 50ms
	
	// Setting the built-in resistance radio for regulation of the V0 voltage
	// Electronic volume control
	// Power control setting
	LCD_WriteCmd(Set_ElecVol_Reg|0x05);
	Delay();
	LCD_WriteCmd(Set_ElecVol_Mode);
	Delay();
	LCD_WriteCmd(Set_ElecVol_Reg);
	Delay();
	//  LCD_Clear();
	Delay();
	LCD_WriteCmd(Set_Page_Addr_0);
	Delay();
	LCD_WriteCmd(Set_ColH_Addr_0);
	Delay();
	LCD_WriteCmd(Set_ColL_Addr_0);
	Delay();
	LCD_WriteCmd(Display_On);
	Delay();
	
	LCD_WriteCmd(Display_All_On);
	Delay();
	LCD_WriteCmd(Display_Off);
	Delay();
	LCD_WriteCmd(Display_On);
	Delay();
	LCD_WriteCmd(Display_All_Normal);
	Delay();
	
	LCD_ClearSCR();
}


/*******************************************************************************
* Function Name  : LCD_PowerOn
* Description    : Power on the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Power_On(void)
{
	LCD_WriteCmd(Display_On);
	Delay();
	LCD_WriteCmd(Power_Booster_On);
	Delay();
	LCD_WriteCmd(Power_Regulator_On);
	Delay();
	LCD_WriteCmd(Power_Follower_On);
	Delay();
}

/*******************************************************************************
* Function Name  : LCD_PowerOff
* Description    : Power off the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Power_Off(void)
{	
	LCD_WriteCmd(Display_Off);		// Power saver
	Delay();
	LCD_WriteCmd(Display_All_On);	        // commands(compound)
	Delay();
}

/*******************************************************************************
* Function Name  : LCD_Sleep_Enter
* Description    : Enter the sleep mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Sleep_Enter(void)
{
	// Sleep Mode Setting (Sleep Mode)
	LCD_WriteCmd(Sleep_Mode_Enable);	// preceding Command
	Delay();
	LCD_WriteCmd(Sleep_Mode_Deliver);	// pollowing Command
	Delay();
	LCD_WriteCmd(Display_Off);				// power saver
	Delay();
}

/*******************************************************************************
* Function Name  : LCD_Sleep_Exit
* Description    : Exit the sleep mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Sleep_Exit(void)
{
	LCD_WriteCmd(Display_All_Normal);		// display all points OFF
	Delay();
	// Sleep Mode Setting (Normal Mode)
	LCD_WriteCmd(Sleep_Mode_Disable);		// preceding Command
	Delay();
	LCD_WriteCmd(Sleep_Mode_Deliver);		// pollowing Command
	Delay();
	LCD_WriteCmd(Display_On);
	Delay();
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : Enables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOn(void)
{
	LCD_WriteCmd(Display_On);	
}

/*******************************************************************************
* Function Name  : LCD_DisplayOff
* Description    : Disables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOff(void)
{
	LCD_WriteCmd(Display_Off);	
}
/*******************************************************************************
* Function Name  : LCD_FillAll
* Description    : Fill the whole LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_FillAll(unsigned char*	data)
{
	unsigned char i,j = GUI_LCM_XMAX;
	unsigned char* p = data;
	
	for(i=0; i<GUI_LCM_PAGE; i++)
	{
		LCD_WriteCmd(Set_Page_Addr_0|i);	
		LCD_WriteCmd(Set_ColH_Addr_0);		
		LCD_WriteCmd(Set_ColL_Addr_0);
		j = GUI_LCM_XMAX;
		while(j--)
		{
			LCD_WriteData(*p++);
			Delay();
		}
	}
}
/*******************************************************************************
* Function Name  : LCD_FillUpdate
* Description    : Fill the whole LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_FillUpdate(unsigned char* data)
{
	unsigned char i,j = GUI_LCM_XMAX;
	unsigned char* p = &gui_disp_buf[0][0];
	
	for (i=0; i<GUI_LCM_PAGE; i++)
	{
		j = GUI_LCM_XMAX;
		while (j--)
		{
			*p++ = *data++;
		}
	}
	LCD_FillAll((unsigned char*)gui_disp_buf);
}
/*******************************************************************************
* Function Name  : LCD_ClearSCR
* Description    : clean screen
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_ClearSCR(void)
{
	unsigned char i;
	unsigned char j;
	
	for(i=0; i<GUI_LCM_PAGE; i++)
	{
		for(j = 0; j < GUI_LCM_XMAX; j++)
			gui_disp_buf[i][j] = 0;
	}
	LCD_FillAll((unsigned char*)gui_disp_buf);
}
/*******************************************************************************
* Function Name  : LCD_Draw
* Description    : Draw LCD data from display buffer.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Draw(void)
{
	LCD_FillAll((unsigned char*)gui_disp_buf);
}
/****************************************************************************
* Function Name  : LCD_UpdatePoint
* Description    : refresh the point in screen
* Input          : x      X-coordinate
y      Y-coordinate
* Output         : None
* Return         : None
****************************************************************************/

void  LCD_UpdatePoint(unsigned int x, unsigned int y)
{
	unsigned char coll, colh, page;
	page = y / 8;
	coll = x & 0x0f;
	colh = x >> 4;
	
	LCD_WriteCmd(Set_Page_Addr_0 | page);	        // page no.
	LCD_WriteCmd(Set_ColH_Addr_0 | colh);		// fixed col first addr
	LCD_WriteCmd(Set_ColL_Addr_0 | coll);
	LCD_WriteData(gui_disp_buf[page][x]);
}

/****************************************************************************
* Function Name  : LCD_PutChar
* Description    : output a char to screen
(the char only can be ' ','0'~'9','A'~'Z','a'~'z')
* Input          : x      X-coordinate
y      Y-coordinate
ch     character
* Output         : None
* Return         : 1    Success
0    Fail
****************************************************************************/
unsigned char  LCD_PutChar(unsigned long x, unsigned long y, unsigned char ch)
{
	unsigned char data;
	unsigned char i;
	unsigned char j;
	
	if( x >(GUI_LCM_XMAX-6)) return(0);
	if( y >(GUI_LCM_YMAX-8)) return(0);
	
	if(ch >= 0x20)
		ch -= 0x20;
	else
		return(0);
	
	for(i = 0; i < FONT_VSIZE; i++)
	{
		data = FONTTYPE6_8[ch][i];
		
		for(j = 0; j < FONT_HSIZE; j++)
		{
			if((data&BIT_MASK[j]) == 0)
				gui_disp_buf[y / 8][x] &= (~(0x01 << ( y % 8)));
			else
				gui_disp_buf[y / 8][x] |= (0x01 <<( y % 8));
			if(sUpdate_mode != 0) 
			{
				LCD_UpdatePoint(x, y);
			}
			x ++;
		}
		x -= FONT_HSIZE;								
		y++;									
	}
	
 	return(1);
}

/****************************************************************************
* Function Name  : LCD_PutString
* Description    : output string to screen
* Input          : x      X-coordinate
y      Y-coordinate
str     pointer to string
* Output         : None
* Return         : None
****************************************************************************/
void  LCD_PutString(unsigned long x, unsigned long y, char* str)
{
	while(1)
	{
		if( (*str)=='\0' ) break;
		if( LCD_PutChar(x, y, *str++) == 0 ) break;
		x += 6;								
	}
}

/*******************************************************************************
* Function Name  : LCD_Write
* Description    : write data or command to lcd sdram
* Input          : command
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteCmd(unsigned char command)
{
	#ifdef SW_SPI
	unsigned char i;
	#endif
	LCD_CD_LOW();
	LCD_CS_LOW();
	#ifdef SW_SPI
	for(i=0; i<8; i++)
	{
		if(command & (0x80 >> i))
			LCD_DATA_HIGH();
		else
			LCD_DATA_LOW();
		
		LCD_CLK_LOW();
		Delay();
		LCD_CLK_HIGH();
		Delay();
	}
	#else
	LCD_SPI_SEND(command);
	#endif
	LCD_CS_HIGH();
}
/*******************************************************************************
* Function Name  : LCD_Write
* Description    : write data or command to lcd sdram
* Input          : data: data or command
attribute: data or command
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteData(unsigned char data)
{
	#ifdef SW_SPI
	unsigned char i;
	#endif
	LCD_CD_HIGH();
	LCD_CS_LOW();
	#ifdef SW_SPI
	for(i=0; i<8; i++)
	{
		if(data & (0x80 >> i))
			LCD_DATA_HIGH();
		else
			LCD_DATA_LOW();
		
		LCD_CLK_LOW();
		Delay();
		LCD_CLK_HIGH();
		Delay();
	}
	#else
	LCD_SPI_SEND(data);
	#endif
	LCD_CS_HIGH();
}

/*pit*/
BOOL PIT_ClkEn ( PIT_Type *PITx )
{
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PITx->MCR &= ~PIT_MCR_MDIS_MASK;
	PITx->MCR |= PIT_MCR_FRZ_MASK;
	return (TRUE);
}

BOOL PIT_ClkDis ( PIT_Type *PITx )
{
	SIM->SCGC6 &= ~SIM_SCGC6_PIT_MASK;
	return (TRUE);
}

BOOL PIT_ChSetup ( PIT_Type *PITx, INT8U ch, INT32U value )
{
	PITx->CHANNEL[ch].LDVAL = (INT32U)value;
	PITx->CHANNEL[ch].TFLG  |= PIT_TFLG_TIF_MASK;
	PITx->CHANNEL[ch].TCTRL |= PIT_TCTRL_TEN_MASK
	                         |PIT_TCTRL_TIE_MASK;
	return (TRUE);
}

void PIT0_IRQHandler(void)
{
	extern INT32U CounterPIT;
	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;	
	CounterPIT++;
}

void PIT1_IRQHandler(void)
{ 
	PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
}

void PIT2_IRQHandler(void)
{
	PIT->CHANNEL[2].TFLG |= PIT_TFLG_TIF_MASK;
}

void PIT3_IRQHandler(void)
{
	PIT->CHANNEL[3].TFLG |= PIT_TFLG_TIF_MASK;
}

/*port*/
BOOL PORT_ClkEn ( PORT_Type *PORTx)
{
	switch ((INT32U)PORTx) {
		case (INT32U)PORTA:
			SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
			break;
		case (INT32U)PORTB:
			SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
			break;
		case (INT32U)PORTC:
			SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
			break;
		case (INT32U)PORTD:
			SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
			break;
		case (INT32U)PORTE:
			SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
			break;
		default:
			break;
	}
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n时钟禁止
BOOL PORT_ClkDis ( PORT_Type *PORTx)
{
	switch ((INT32U)PORTx) {
		case (INT32U)PORTA:
			SIM->SCGC5 &= ~SIM_SCGC5_PORTA_MASK;
			break;
		case (INT32U)PORTB:
			SIM->SCGC5 &= ~SIM_SCGC5_PORTB_MASK;
			break;
		case (INT32U)PORTC:
			SIM->SCGC5 &= ~SIM_SCGC5_PORTC_MASK;
			break;
		case (INT32U)PORTD:
			SIM->SCGC5 &= ~SIM_SCGC5_PORTD_MASK;
			break;
		case (INT32U)PORTE:
			SIM->SCGC5 &= ~SIM_SCGC5_PORTE_MASK;
			break;
		default:
			break;
	}
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//设置端口n第n位的中断方式
//mode	0000 Interrupt/DMA Request disabled
//		0001 DMA Request on rising edge
//		0010 DMA Request on falling edge
//		0011 DMA Request on either edge
//		0100 Reserved
//		1000 Interrupt when logic zero
//		1001 Interrupt on rising edge
//		1010 Interrupt on falling edge
//		1011 Interrupt on either edge
//		1100 Interrupt when logic one
BOOL PORT_IntCfg ( PORT_Type *PORTx, INT32U pin_n, INT32U mode_n)
{
	PORTx->PCR[pin_n] &= ~PORT_PCR_IRQC_MASK;
	PORTx->PCR[pin_n] |= PORT_PCR_IRQC(mode_n);
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n第n位控制寄存器锁定（如果锁定，该寄存器在下一次复位时解锁）
BOOL PORT_BitLock ( PORT_Type *PORTx, INT32U pin_n)
{
	PORTx->PCR[pin_n] |= PORT_PCR_LK_MASK;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n第n位引脚功能选择
BOOL PORT_BitFn ( PORT_Type *PORTx, INT32U pin_n, INT32U fn_n)
{
	PORTx->PCR[pin_n] &= ~PORT_PCR_MUX_MASK;
	PORTx->PCR[pin_n] |= PORT_PCR_MUX(fn_n);
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n第n位引脚驱动能力设置
//dse: drive strength enable			ode: open drain enable
//pfe: passive filter enable			sre: slew rate enable
//pe : pull enable						ps : pull select(0:pull-down 1:pull-up)
BOOL PORT_BitDrive ( PORT_Type *PORTx, INT32U pin_n, INT32U dse, INT32U ode,
					INT32U pfe, INT32U sre, INT32U pe, INT32U ps)
{
	INT32U tmp = 0;
	
	PORTx->PCR[pin_n] &= ~0xFF;
	
	if (dse != 0) {tmp |= PORT_PCR_DSE_MASK;}
	if (ode != 0) {tmp |= PORT_PCR_ODE_MASK;}
	if (pfe != 0) {tmp |= PORT_PCR_PFE_MASK;}
	if (sre != 0) {tmp |= PORT_PCR_SRE_MASK;}
	if (pe  != 0) {tmp |= PORT_PCR_PE_MASK;}
	if (ps  != 0) {tmp |= PORT_PCR_PS_MASK;}
	
	PORTx->PCR[pin_n] |= tmp;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n所有位控制寄存器锁定
BOOL PORT_GlobalLock ( PORT_Type *PORTx)
{
	PORTx->GPCLR = PORT_PCR_LK_MASK|
				PORT_PCR_LK_MASK<<16;
	PORTx->GPCHR = PORT_PCR_LK_MASK|
				PORT_PCR_LK_MASK<<16;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n所有位功能选择
BOOL PORT_GlobalFn ( PORT_Type *PORTx, INT32U fn_n)
{
	PORTx->GPCLR = PORT_PCR_MUX(fn_n)|
				PORT_PCR_MUX_MASK<<16;
	PORTx->GPCHR = PORT_PCR_MUX(fn_n)|
				PORT_PCR_MUX_MASK<<16;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n所有位引脚驱动能力设置
BOOL PORT_GlobalDrive ( PORT_Type *PORTx, INT32U dse, INT32U ode,
					INT32U pfe, INT32U sre, INT32U pe, INT32U ps)
{
	INT32U tmp = 0;	
	if (dse != 0) {tmp |= PORT_PCR_DSE_MASK;}
	if (ode != 0) {tmp |= PORT_PCR_ODE_MASK;}
	if (pfe != 0) {tmp |= PORT_PCR_PFE_MASK;}
	if (sre != 0) {tmp |= PORT_PCR_SRE_MASK;}
	if (pe  != 0) {tmp |= PORT_PCR_PE_MASK;}
	if (ps  != 0) {tmp |= PORT_PCR_PS_MASK;}

	PORTx->GPCLR = tmp | 0xFF<<16;
	PORTx->GPCHR = tmp | 0xFF<<16;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n第n位的滤波器允许
BOOL PORT_BitFilterEn ( PORT_Type *PORTx, INT32U bit_n)
{
	PORTx->DFER |= 1<<bit_n;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n第n位的滤波器禁止
BOOL PORT_BitFilterDis ( PORT_Type *PORTx, INT32U bit_n)
{
	PORTx->DFER &= ~(1<<bit_n);
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n所有位的滤波器允许
BOOL PORT_GlobalFilterEn ( PORT_Type *PORTx)
{
	PORTx->DFER = (INT32U)0xFFFFFFFF;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n所有位的滤波器禁止
BOOL PORT_GlobalFilterDis ( PORT_Type *PORTx)
{
	PORTx->DFER = (INT32U)0x0;
	return (TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n滤波器设置
//cs : clock source						flen: filter length
BOOL PORT_FilterCfg ( PORT_Type *PORTx, INT32U cs, INT32U flen)
{
	(cs == 0) ? (PORTx->DFCR = (INT32U)0x0) : (PORTx->DFCR = (INT32U)0x1);
	PORTx->DFWR = PORT_DFWR_FILT(flen);
	return (TRUE);
}

/*spi*/
BOOL SPI_ClkEn ( SPI_Type *SPIx )
{
	switch ((INT32U)SPIx) {
		case (INT32U)SPI0:
			SIM->SCGC6 |= SIM_SCGC6_SPI0_MASK;
			break;
		case (INT32U)SPI1:
			SIM->SCGC6 |= SIM_SCGC6_SPI1_MASK;
			break;
//		case (INT32U)SPI2:
//			SIM->SCGC3 |= SIM_SCGC3_SPI2_MASK;
//			break;
		default:
			;
	}
	
	return (TRUE);
}

BOOL SPI_ClkDis ( SPI_Type *SPIx )
{
	switch ((INT32U)SPIx) {
		case (INT32U)SPI0:
			SIM->SCGC6 &= ~SIM_SCGC6_SPI0_MASK;
			break;
		case (INT32U)SPI1:
			SIM->SCGC6 &= ~SIM_SCGC6_SPI1_MASK;
			break;
//		case (INT32U)SPI2:
//			SIM->SCGC3 &= ~SIM_SCGC3_SPI2_MASK;
//			break;
		default:
			;
	}
	
	return (TRUE);
}

BOOL SPI_Init ( SPI_Type *SPIx, INT32U baud )
{
	SPIx->MCR	 = (SPI_MCR_CLR_RXF_MASK|
					SPI_MCR_CLR_TXF_MASK|
					SPI_MCR_PCSIS_MASK	|
					SPI_MCR_HALT_MASK	);	
	SPIx->MCR	|= 	SPI_MCR_MSTR_MASK;
	
	SPIx->CTAR[0] = (//SPI_CTAR_DBR_MASK	|
					SPI_CTAR_BR(7)		|
					SPI_CTAR_FMSZ(7)	|
					SPI_CTAR_CSSCK(4)	|
					SPI_CTAR_ASC(4)		|
					SPI_CTAR_DT(6)		|
					SPI_CTAR_PDT_MASK	//|
// 					SPI_CTAR_CPOL_MASK	|
// 					SPI_CTAR_CPHA_MASK	
					);
						
	SPIx->SR	    |= (SPI_SR_EOQF_MASK	|
					SPI_SR_TFFF_MASK	|
					SPI_SR_TFUF_MASK	|
					SPI_SR_RFDF_MASK	|
					SPI_SR_RFOF_MASK	);

	SPIx->MCR	&=  ~SPI_MCR_HALT_MASK;
	return (TRUE);
}

INT8U SPI_SendReceive ( SPI_Type *SPIx, INT8U tx_dat )
{
	SPIx->SR	 = (SPI_SR_EOQF_MASK	|
				SPI_SR_TFFF_MASK	|
				SPI_SR_TFUF_MASK	|
				SPI_SR_RFDF_MASK	|
				SPI_SR_RFOF_MASK	);
	
	SPIx->MCR |= (SPI_MCR_CLR_RXF_MASK|
				SPI_MCR_CLR_TXF_MASK);	
		
	SPIx->PUSHR = (SPI_PUSHR_CTAS(0)	|
				SPI_PUSHR_EOQ_MASK	|
				SPI_PUSHR_PCS(1)	|	/*SPI Flash cs Conflict with lcd_en(pcs1) spiflash need comment this statement*/
				SPI_PUSHR_TXDATA(tx_dat));
		
 	while((SPIx->SR & SPI_SR_TCF_MASK) == 0);
	SPIx->SR |= SPI_SR_TCF_MASK;
	
	return (SPIx->POPR);
}

INT32U SPI_Send ( SPI_Type *SPIx, INT8U *buff, INT32U len )
{
	while(len-- != 0) 
		SPI_SendReceive ( SPIx, *buff++);
	return (len);
}

INT32U SPI_Receive ( SPI_Type *SPIx, INT8U *buff, INT32U len )
{
	while(len-- != 0)
		*buff++ = SPI_SendReceive ( SPIx, 0xFF);
	return (len);
}

void SPI0_IRQHandler(void)
{
	;
}

void SPI1_IRQHandler(void)
{
	;
}

void SPI2_IRQHandler(void)
{
	;
}
unsigned char SPI_Flash_ReadByte(unsigned int addr)
{
	unsigned char *p;
	unsigned char txdbuff[6];
	txdbuff[0] = 0x03;
	p = (unsigned char *)&addr;
	txdbuff[3] = *p++;
	txdbuff[2] = *p++;
	txdbuff[1] = *p;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 4 );
	SPI_FLASH_RECEIVE(txdbuff, 1 );
	SPI_FLASH_DIS();
	return txdbuff[0];
}

void SPI_Flash_Read(unsigned int addr, unsigned char *dat, unsigned int n)
{
	unsigned char *p;
	unsigned char txdbuff[6];
	txdbuff[0] = 0x0B;
	p = (unsigned char *)&addr;
	txdbuff[3] = *p++;
	txdbuff[2] = *p++;
	txdbuff[1] = *p;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 5 );
	SPI_FLASH_RECEIVE(dat, n );
	SPI_FLASH_DIS();
}

void SPI_Flash_WriteEnable(void)
{
	unsigned char txdbuff = 0x06;
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1 );	
	SPI_FLASH_DIS();	
}

void SPI_Flash_WriteDisable(void)
{
	unsigned char txdbuff = 0x04;
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1 );	
	SPI_FLASH_DIS();	
}

unsigned char SPI_Flash_ReadStaReg(void)
{
	unsigned char txdbuff = 0x05;
	SPI_FLASH_EN();
	SPI_FLASH_SEND(&txdbuff, 1 );	
	SPI_FLASH_RECEIVE(&txdbuff, 1 );
	SPI_FLASH_DIS();
	return txdbuff;
}

void SPI_Flash_WriteStaReg(unsigned char sta)
{
	unsigned char txdbuff = 0x01;
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1 );	
	SPI_FLASH_SEND((unsigned char *)&sta, 1 );
	SPI_FLASH_DIS();	
}


void SPI_Flash_PageWrite(unsigned int addr, unsigned char *dat, unsigned int n)
{
	unsigned char txdbuff[6];

	SPI_Flash_WriteEnable();
	
	txdbuff[0] = 0x02;
	txdbuff[1] = addr >> 16;
	txdbuff[2] = addr >> 8;
	txdbuff[3] = addr;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 4 );
	SPI_FLASH_SEND(dat, n );
	SPI_FLASH_DIS();
	while (SPI_Flash_Busy());
}

void SPI_Flash_Write(unsigned int addr, unsigned char *dat, unsigned int n)
{
	unsigned int m;
	if (((addr & 0xff) + n) > 0x100) {
		m = 0x100 - (addr & 0xff);
		SPI_Flash_PageWrite(addr, dat, m);
		SPI_Flash_PageWrite((addr + m), dat + m, n - m);
	}
	else {
		SPI_Flash_PageWrite(addr, dat, n);
	}
}

void SPI_Flash_ChipErase(void)
{
	unsigned char txdbuff = 0x60;
	SPI_Flash_WriteEnable();	
	SPI_FLASH_EN();
	SPI_FLASH_SEND((unsigned char *)&txdbuff, 1);	
	SPI_FLASH_DIS();
	while (SPI_Flash_Busy());
}


void SPI_Flash_Init(void)
{
	SPI_Flash_WriteStaReg(0x00);
}

unsigned char SPI_Flash_Busy(void)
{
	return (SPI_Flash_ReadStaReg() & 0x01);
}

unsigned char SPI_Flash_DevID(void)
{
	unsigned char txdbuff[6];
	txdbuff[0] = 0x90;										//fast read
	txdbuff[3] = 0x00;
	
	SPI_FLASH_EN();
	SPI_FLASH_SEND(txdbuff, 4 );
	SPI_FLASH_RECEIVE(txdbuff, 2 );
	SPI_FLASH_DIS();
	return txdbuff[1];
}
/*tsi*/

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

