#ifndef __PIT_H
#define __PIT_H

#include "kinetis.h"
#include "type.h"

#define PIT_CH0   (0)
#define PIT_CH1   (1)
#define	PIT_CH2   (2)
#define PIT_CH3   (3)

BOOL PIT_ClkEn ( PIT_Type *PITx );
BOOL PIT_ClkDis ( PIT_Type *PITx );
BOOL PIT_ChSetup ( PIT_Type *PITx, INT8U ch, INT32U value );

void PIT0_IRQHandler(void);
void PIT1_IRQHandler(void);
void PIT2_IRQHandler(void);
void PIT3_IRQHandler(void);

#endif
