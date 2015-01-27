#ifndef	__FLEXCAN_H
#define __FLEXCAN_H

#include "kinetis.h"
#include "type.h"

#define CAN_PCLK          (SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT)+1))	
#define STANDARD_FORMAT   (0)
#define EXTENDED_FORMAT   (1)
#define DATA_FRAME        (0)
#define REMOTE_FRAME      (1)

/* CAN message object structure                                              */
typedef struct {
	INT32U id;                 /* 29 bit identifier                               */
	INT8U  format;             /* 0 - STANDARD,   1 - EXTENDED IDENTIFIER         */
	INT8U  type;               /* 0 - DATA FRAME, 1 - REMOTE FRAME                */
	INT8U  len;                /* Length of data field in bytes                   */
	INT8U  reserve;            /* Object channel                                  */
	INT8U  data[8];            /* Data field                                      */
} CAN_msg_t;

extern INT32U CAN_MBRxflag[2];

extern CAN_msg_t CAN_MsgBox[32];

BOOL CAN_ClkEn (CAN_Type *CANx);
BOOL CAN_ClkDis (CAN_Type *CANx);
BOOL CAN_Init (CAN_Type *CANx, INT32U baud);
BOOL CAN_DeInit (CAN_Type *CANx);
BOOL CAN_Loopback (CAN_Type *CANx, INT8U val);
BOOL CAN_EnChanelInterrupt(CAN_Type *CANx, INT8U ch);
BOOL CAN_DisChanelInterrupt(CAN_Type *CANx, INT8U ch);
BOOL CAN_Wr (CAN_Type *CANx, INT8U ch, CAN_msg_t *msg);
BOOL CAN_Rd (CAN_Type *CANx, INT8U ch, CAN_msg_t *msg);
BOOL CAN_Set (CAN_Type *CANx, CAN_msg_t *msg);
BOOL CAN_RxObject (CAN_Type *CANx, INT8U ch, INT32U id, INT32U object_para);

#endif
