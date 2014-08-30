#include "glcd.h"

#ifdef HX8347
	#include "hx8347.c"
#else
	#include "ili9341.c"
#endif
