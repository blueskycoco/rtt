#ifndef	__CMP_H
#define	__CMP_H

#include "kinetis.h"
#include "type.h"

BOOL CMP_ClkEn ( CMP_Type *CMPx );
BOOL CMP_ClkDis ( CMP_Type *CMPx );
BOOL CMP_Init ( CMP_Type *CMPx, INT8U Vref, INT8U inp_n, INT8U inm_n );
BOOL CMP_DacSetVal ( CMP_Type *CMPx, INT8U stage );
BOOL CMP_IntEn ( CMP_Type *CMPx );
BOOL CMP_IntDis ( CMP_Type *CMPx );
BOOL CMP_GetOutVal ( CMP_Type *CMPx );
#endif
