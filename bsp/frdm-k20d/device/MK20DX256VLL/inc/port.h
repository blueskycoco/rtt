/**************************************************************************************************
*			file: port.h
*			
*			date: 2013.2.28 Manley
**************************************************************************************************/
#ifndef __PORT_H
#define __PORT_H

#include "kinetis.h"
#include "type.h"

#define PIN_0 	((INT8U)0x00)
#define PIN_1 	((INT8U)0x01)
#define PIN_2 	((INT8U)0x02)
#define PIN_3 	((INT8U)0x03)
#define PIN_4 	((INT8U)0x04)
#define PIN_5 	((INT8U)0x05)
#define PIN_6 	((INT8U)0x06)
#define PIN_7 	((INT8U)0x07)
#define PIN_8 	((INT8U)0x08)
#define PIN_9 	((INT8U)0x09)
#define PIN_10 	((INT8U)0x0A)
#define PIN_11 	((INT8U)0x0B)
#define PIN_12 	((INT8U)0x0C)
#define PIN_13 	((INT8U)0x0D)
#define PIN_14 	((INT8U)0x0E)
#define PIN_15 	((INT8U)0x0F)
#define PIN_16 	((INT8U)0x10)
#define PIN_17 	((INT8U)0x11)
#define PIN_18 	((INT8U)0x12)
#define PIN_19 	((INT8U)0x13)
#define PIN_20 	((INT8U)0x14)
#define PIN_21 	((INT8U)0x15)
#define PIN_22 	((INT8U)0x16)
#define PIN_23 	((INT8U)0x17)
#define PIN_24 	((INT8U)0x18)
#define PIN_25 	((INT8U)0x19)
#define PIN_26 	((INT8U)0x1A)
#define PIN_27 	((INT8U)0x1B)
#define PIN_28 	((INT8U)0x1C)
#define PIN_29 	((INT8U)0x1D)
#define PIN_30 	((INT8U)0x1E)
#define PIN_31 	((INT8U)0x1F)

#define FN_0 	((INT8U)0x00)
#define FN_1 	((INT8U)0x01)
#define FN_2 	((INT8U)0x02)
#define FN_3 	((INT8U)0x03)
#define FN_4 	((INT8U)0x04)
#define FN_5 	((INT8U)0x05)
#define FN_6 	((INT8U)0x06)
#define FN_7 	((INT8U)0x07)

#define DS_EN 	((INT8U)0x01)
#define DS_DIS 	((INT8U)0x00)
#define OD_EN 	((INT8U)0x01)
#define OD_DIS 	((INT8U)0x00)
#define PF_EN 	((INT8U)0x01)
#define PF_DIS 	((INT8U)0x00)
#define SR_EN 	((INT8U)0x01)
#define SR_DIS 	((INT8U)0x00)
#define P_EN 	((INT8U)0x01)
#define P_DIS 	((INT8U)0x00)
#define P_DOWN 	((INT8U)0x00)
#define P_UP 	((INT8U)0x01)

BOOL PORT_ClkEn ( PORT_Type *PORTx);
BOOL PORT_ClkDis ( PORT_Type *PORTx);
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
BOOL PORT_IntCfg ( PORT_Type *PORTx, INT32U pin_n, INT32U mode_n);

BOOL PORT_BitLock ( PORT_Type *PORTx, INT32U pin_n);
BOOL PORT_GlobalLock ( PORT_Type *PORTx);

BOOL PORT_BitFn ( PORT_Type *PORTx, INT32U pin_n, INT32U fn_n);
BOOL PORT_GlobalFn ( PORT_Type *PORTx, INT32U fn_n);
///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n第n位引脚驱动能力设置
//dse: drive strength enable			ode: open drain enable
//pfe: passive filter enable			sre: slew rate enable
//pe : pull enable						ps : pull select(0:pull-down 1:pull-up)
BOOL PORT_BitDrive ( PORT_Type *PORTx, INT32U bit_n, INT32U dse, INT32U ode,
					INT32U pfe, INT32U sre, INT32U pe, INT32U ps);
BOOL PORT_GlobalDrive ( PORT_Type *PORTx, INT32U dse, INT32U ode,
					INT32U pfe, INT32U sre, INT32U pe, INT32U ps);
					
BOOL PORT_BitFilterEn ( PORT_Type *PORTx, INT32U bit_n);
BOOL PORT_BitFilterDis ( PORT_Type *PORTx, INT32U bit_n);
BOOL PORT_GlobalFilterEn ( PORT_Type *PORTx);
BOOL PORT_GlobalFilterDis ( PORT_Type *PORTx);
///////////////////////////////////////////////////////////////////////////////////////////////////
//端口n滤波器设置
//cs : clock source						flen: filter length
BOOL PORT_FilterCfg ( PORT_Type *PORTx, INT32U cs, INT32U flen);
#endif
