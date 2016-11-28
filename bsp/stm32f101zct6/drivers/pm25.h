#ifndef __PM25_H
#define __PM25_H

#ifdef __cplusplus
 extern "C" {
#endif
#define CAP_DIST_TIME 10
typedef enum _STATE{
	STATE_INITIAL = 0,
	STATE_BEGIN,
	STATE_LEN,
	STATE_DATA,
	STATE_CRC
}STATE;
void pm25_init(void);
void ask_pm25();
#ifdef __cplusplus
}
#endif

#endif

