#ifndef __TYPE_H
#define __TYPE_H

typedef unsigned char     BOOL;

#ifndef FALSE
#define FALSE             ((BOOL)0x00)
#endif

#ifndef TRUE
#define TRUE              ((BOOL)0x01)
#endif

#ifndef NULL
#define NULL 	         ((void *)0)
#endif

typedef unsigned char     INT8U;
typedef signed   char     INT8S;
typedef          char     INT8;

typedef unsigned short    INT16U;
typedef signed   short    INT16S;
typedef          short    INT16;

typedef unsigned int      INT32U;
typedef signed   int      INT32S;
typedef          int      INT32;

typedef float             FP32;
typedef double            FP64;

#endif
