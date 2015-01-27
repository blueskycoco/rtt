#include "flexcan.h"

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
