/**************************************************************************************************
*			file: port.c
*			
*			date: 2013.2.28 Manley
**************************************************************************************************/
#include "port.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n时钟允许
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
