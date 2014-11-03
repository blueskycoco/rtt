#ifndef	__I2S_H
#define	__I2S_H

#include "kinetis.h"
#include "type.h"

void I2S_Init(void);
BOOL CS43L22_Init (void);
BOOL I2S_ClkEn ( I2S_Type *I2Sx );

extern const	unsigned char GucWaveData[];

#endif
