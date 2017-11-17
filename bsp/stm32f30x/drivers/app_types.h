/*****************************************************************************************
��Ȩ����:        ---------------
�汾��:          1.0.0
��������:        2010.05.11
�ļ���:          app_compile.h
����:            Ӱ����
����˵��:        �������������ļ�
����˵��:        Evaluation Board
�����ļ���ϵ:    ���ļ�Ϊ���̹�Լ�����ļ�

�޸ļ�¼:
��¼1:
�޸���:
�޸�����:
�޸�����:
�޸�ԭ��:
*****************************************************************************************/

#ifndef APP_COMPILE_PRESENT
#define APP_COMPILE_PRESENT




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2009.09.08
����˵��:   ���ÿ���
����˵��:   ��
*****************************************************************************************/
#define WDT_DELAY                       (200)                       // WDT ʱ����

#define HD_VERSION                      (100)                       // Ӳ���汾 220 == H2.20
#define SF_VERSION                      (9774)                      // ����汾 SVNxxxx
#define DT_DEVLOPER                     "2017-02-21"                // "DOTT: 2014 -11 -12"




/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICKRATE_HZ                     (2000)                      /* 2000 ticks per second */
#define ADCRATE_HZ                      (1000000)                   /* 1000000 HZ */




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2009.09.08
����˵��:   �ͺ�ȷ��
����˵��:   ��
    sale    P1  ֧���ݴ�/���ϴ�ӡ
    sale    P2  ����
*****************************************************************************************/
#ifdef  MONITOR_STM32F1_A1
    #include "app_p1.h"

    #define PRODUCT_NAME                  "MonitorA1"
    #define APP_NAME                      "MonitorA1.bin"
#endif

#ifdef  MONITOR_STM32F1_A2
    #include "app_p2.h"

    #define PRODUCT_NAME                  "MonitorA2"
    #define APP_NAME                      "MonitorA2.bin"
#endif




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:        1.0.0
��������:    2009.09.08
����˵��:   ���ÿ���
����˵��:   ��
*****************************************************************************************/
#define LED_SUPPORT                         1                           // �Ƿ����LEDָʾ��
#define KEY_SUPPORT                         1                           // �Ƿ����KEY֧��
#define WDT_SUPPORT                         1                           // �Ƿ����WDT֧��
#define IIC_SUPPORT                         1                           // �Ƿ����IIC֧��
#define ADC_SUPPORT                         1                           // �Ƿ����ADC֧��
#define DAC_SUPPORT                         1                           // �Ƿ����DAC֧��
#define WAVE_SUPPORT                        1                           // �Ƿ����WAVE֧��
#define SPI_SUPPORT                         1                           // �Ƿ����SPI֧��
#define TIMER_SUPPORT                       1                           // �Ƿ����TIMER֧��
#define TIMER1_SUPPORT                      1                           // �Ƿ����TIMER1֧��
#define TIMER2_SUPPORT                      1                           // �Ƿ����TIMER2֧��
#define PCF8563_SUPPORT                     1                           // �Ƿ����PCF8563֧��
#define MX25L1602_SUPORT                    1                           // �Ƿ����MX25L1602֧��
#define PWM_SUPPORT                         1                           // �Ƿ����PWM֧��
#define FLASH_SUPPORT                       1                           // �Ƿ����FLASH����
#define POWER_SUPPORT                       1                           // �Ƿ����POWER����
#define LCD_SUPPORT                         1                           // �Ƿ����LCD֧��
#define MONITOR_SUPPORT                     0                           // �Ƿ����Ӧ�ù���




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2010.05.11
����˵��:   ��������������
����˵��:   ��
*****************************************************************************************/
#define    uint8        unsigned   char                                 // �޷��� 8λ���ͱ���
#define    int8         signed     char                                 // �з��� 8λ���ͱ���
#define    uint16       unsigned   short                                // �޷���16λ���ͱ���
#define    int16        signed     short                                // �з���16λ���ͱ��� 
#define    uint32       unsigned   int                                  // �޷���32λ���ͱ���
#define    int32        signed     int                                  // �з���32λ���ͱ���
#define    uint64       unsigned   long long                            // �޷���64λ���ͱ���
#define    int64        signed     long long                            // �з���64λ���ͱ���
#define    float32      float                                           // �����ȸ�����(32λ����)
#define    float64      double                                          // ˫���ȸ�����(64λ����)

#define    FALSE        0
#define    TRUE         1

#define    false        0
#define    true         1

#define    DEF_OFF      0
#define    DEF_ON       1

#define    INT08U       unsigned    char                                      // �޷��� 8λ���ͱ���
#define    INT08S       signed      char                                      // �з��� 8λ���ͱ���
#define    INT16U       unsigned    short                                     // �޷���16λ���ͱ���
#define    INT16S       signed      short                                     // �з���16λ���ͱ��� 
#define    INT32U       unsigned    int                                       // �޷���32λ���ͱ���
#define    INT32S       signed      int                                       // �з���32λ���ͱ���
#define    INT64U       unsigned    long long                                 // �޷���64λ���ͱ���
#define    INT64S       signed      long long                                 // �з���64λ���ͱ���
#define    FLOAT32      float                                                 // �����ȸ�����(32λ����)
#define    FLOAT64      double                                                // ˫���ȸ�����(64λ����)

#define    INT8U        uint8_t
#define    BOOL         int
#define    BYTE         unsigned   char 


// ������ɫ�ĺ�
#define WHITE                   0xFFFF
#define BLACK                   0x0000
#define BLUE                    0x001F
#define RED                     0xF800
#define MAGENTA                 0xF81F
#define GREEN                   0x07E0
#define CYAN                    0x7FFF
#define YELLOW                  0xFFE0                      //������ɫ�ĺ�
#define GREY                    0xc618   //��
#define lan                     0x051D


#define BOARD_LCD_WIDTH             (320 - 1)
#define BOARD_LCD_HEIGHT            (480 - 1)    




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2009.09.08
����˵��:   ���ÿ���
����˵��:   ��
*****************************************************************************************/
#define UART1_BPS                       115200                          // �û��ٶ� 
#define UART2_BPS                       115200                          // �����ٶ�  
#define UART3_BPS                       115200                          // �����ٶ�  
#define UART4_BPS                       115200                          // �����ٶ�  
#define UART5_BPS                       115200                          // �����ٶ�  




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:        1.0.0
��������:    2009.09.08
����˵��:   ���ÿ���
����˵��:   ��
*****************************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"

#include "STM32F30x.h"

#include "ILI93XX.h"

#include "gui_font.h"
#include "gui_hzk14.h"
#include "gui_hzk16.h"
#include "gui_hzk24.h"
#include "gui_hzk35.h"
#include "gui_hzk64.h"
#include "gui_basic.h"
#include "gui_loadbmp.h"



/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2009.10.13
����˵��:   �ṹ����
����˵��:   ��
*****************************************************************************************/
#define __APP_PACKED(n)        __attribute__ ((packed, aligned(n)))            // �ṹ���� 

extern  void  TICK_DisableIRQ(void);
extern  void  TICK_EnableIRQ(void);

extern  void  Delay_Ms(uint32 nCount);
extern  void  Delay_Us(uint32 nCount);
extern  void  Delay(uint32 nCount);

extern  u16   crc16(u16 crc, u8 const *buffer, size_t len);
extern  void  DebugMessage(uint8 * message, uint32 number);




/*****************************************************************************************
��Ȩ����:   Ӱ����
�汾��:     1.0.0
��������:   2009.10.04
����˵��:   ϵͳ����
����˵��:   ��
*****************************************************************************************/
#if (DEBUG_SUPPORT == 1)
    #define dprintf(fmt, args...)       printf(fmt, ##args)                // ��Ϣ��ӡ
#endif
#if (DEBUG_SUPPORT == 0)
    #define dprintf(fmt, args...)
#endif

#define ENTER_CRITICAL()               // TICK_DisableIRQ()                  // �����ж�
#define EXIT_CRITICAL()                // TICK_EnableIRQ()                   // �˳��ж�

#define APP_crc16(a,b,c)                crc16(a,b,c)                       // CRC 16

#define qprintf(fmt, args...)           dprintf(fmt, ##args)               // ��ӡ
#define mprintf(fmt, args...)           dprintf(fmt, ##args)               // ��ӡ
#define pprintf(fmt, args...)           dprintf(fmt, ##args)               // ��ӡ
#define bprintf(fmt, args...)           dprintf(fmt, ##args)               // ��ӡ
#define DBG_printf(fmt, args...)        dprintf(fmt, ##args)               // ��ӡ
#define APP_printf(fmt, args...)        dprintf(fmt, ##args)               // ��ӡ

#define APP_sprintf                     sprintf                            // ��ӡ
#define APP_malloc(a)                   malloc(a)                          // �����ڴ�
#define APP_free(a)                     free(a)                            // �ͷ��ڴ�
#define APP_strlen(a)                   strlen(a)                          // ����
#define APP_sizeof(a)                   sizeof(a)                          // ��С
#define APP_strcat(a, b)                strcat(a, b)                       // ����
#define APP_strstr(a, b)                strstr(a, b)                       // ��������
#define APP_strchr(a, b)                strchr(a, b)                       // ��������
#define APP_strcmp(a, b)                strcmp(a, b)                       // �Ƚ�
#define APP_strncmp(a, b, n)            strncmp(a, b, n)                   // �Ƚ�
#define APP_strcpy(a, b)                strcpy(a, b)                       // ����
#define APP_strncpy(a, b, n)            strncpy(a, b, n)                   // ����
#define APP_abs(a)                      abs(a)                             // ����ֵ
#define APP_fabs(a)                     fabs(a)                            // ����ֵ
#define APP_atoi(a)                     atoi(a)                            // �ַ���ת����
#define APP_atol(a)                     atol(a)                            // �ַ���ת����
#define APP_atof(a)                     atof(a)                            // �ַ���ת����
#define APP_memset(a, b, n)             memset(a, b, n)                    // �ڴ�����
#define APP_memcpy(a, b, n)             memcpy(a, b, n)                    // �ڴ濽��
#define APP_memcmp(a, b, n)             memcmp(a, b, n)                    // �ڴ�Ƚ�
#define APP_memmove(a, b, n)            memmove(a, b, n)                   // �ڴ��ƶ�

#define GUI_sprintf                     sprintf                            // ��ӡ
#define GUI_strlen(a)                   strlen(a)                          // ����
#define GUI_sizeof(a)                   sizeof(a)                          // ��С
#define GUI_strcmp(a, b)                strcmp(a, b)                       // �Ƚ�
#define GUI_strncmp(a, b, n)            strncmp(a, b, n)                   // �Ƚ�
#define GUI_strcpy(a, b)                strcpy(a, b)                       // ����
#define GUI_strncpy(a, b, n)            strncpy(a, b, n)                   // ����
#define GUI_atol(a)                     atol(a)                            // �ַ���ת����
#define GUI_memset(a, b, n)             memset(a, b, n)                    // �ڴ�����
#define GUI_memcpy(a, b, n)             memcpy(a, b, n)                    // �ڴ濽��




#endif


