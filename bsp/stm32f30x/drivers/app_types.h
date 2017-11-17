/*****************************************************************************************
版权所有:        ---------------
版本号:          1.0.0
生成日期:        2010.05.11
文件名:          app_compile.h
作者:            影舞者
功能说明:        变量类型配置文件
其它说明:        Evaluation Board
所属文件关系:    本文件为工程规约代码文件

修改记录:
记录1:
修改者:
修改日期:
修改内容:
修改原因:
*****************************************************************************************/

#ifndef APP_COMPILE_PRESENT
#define APP_COMPILE_PRESENT




/*****************************************************************************************
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2009.09.08
功能说明:   配置开关
其它说明:   无
*****************************************************************************************/
#define WDT_DELAY                       (200)                       // WDT 时间间隔

#define HD_VERSION                      (100)                       // 硬件版本 220 == H2.20
#define SF_VERSION                      (9774)                      // 软件版本 SVNxxxx
#define DT_DEVLOPER                     "2017-02-21"                // "DOTT: 2014 -11 -12"




/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define TICKRATE_HZ                     (2000)                      /* 2000 ticks per second */
#define ADCRATE_HZ                      (1000000)                   /* 1000000 HZ */




/*****************************************************************************************
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2009.09.08
功能说明:   型号确认
其它说明:   无
    sale    P1  支持容大/开聪打印
    sale    P2  测试
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
版权所有:   影舞者
版本号:        1.0.0
生成日期:    2009.09.08
功能说明:   配置开关
其它说明:   无
*****************************************************************************************/
#define LED_SUPPORT                         1                           // 是否加入LED指示灯
#define KEY_SUPPORT                         1                           // 是否加入KEY支持
#define WDT_SUPPORT                         1                           // 是否加入WDT支持
#define IIC_SUPPORT                         1                           // 是否加入IIC支持
#define ADC_SUPPORT                         1                           // 是否加入ADC支持
#define DAC_SUPPORT                         1                           // 是否加入DAC支持
#define WAVE_SUPPORT                        1                           // 是否加入WAVE支持
#define SPI_SUPPORT                         1                           // 是否加入SPI支持
#define TIMER_SUPPORT                       1                           // 是否加入TIMER支持
#define TIMER1_SUPPORT                      1                           // 是否加入TIMER1支持
#define TIMER2_SUPPORT                      1                           // 是否加入TIMER2支持
#define PCF8563_SUPPORT                     1                           // 是否加入PCF8563支持
#define MX25L1602_SUPORT                    1                           // 是否加入MX25L1602支持
#define PWM_SUPPORT                         1                           // 是否加入PWM支持
#define FLASH_SUPPORT                       1                           // 是否加入FLASH功能
#define POWER_SUPPORT                       1                           // 是否加入POWER功能
#define LCD_SUPPORT                         1                           // 是否加入LCD支持
#define MONITOR_SUPPORT                     0                           // 是否加入应用功能




/*****************************************************************************************
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2010.05.11
功能说明:   编译器变量类型
其它说明:   无
*****************************************************************************************/
#define    uint8        unsigned   char                                 // 无符号 8位整型变量
#define    int8         signed     char                                 // 有符号 8位整型变量
#define    uint16       unsigned   short                                // 无符号16位整型变量
#define    int16        signed     short                                // 有符号16位整型变量 
#define    uint32       unsigned   int                                  // 无符号32位整型变量
#define    int32        signed     int                                  // 有符号32位整型变量
#define    uint64       unsigned   long long                            // 无符号64位整型变量
#define    int64        signed     long long                            // 有符号64位整型变量
#define    float32      float                                           // 单精度浮点数(32位长度)
#define    float64      double                                          // 双精度浮点数(64位长度)

#define    FALSE        0
#define    TRUE         1

#define    false        0
#define    true         1

#define    DEF_OFF      0
#define    DEF_ON       1

#define    INT08U       unsigned    char                                      // 无符号 8位整型变量
#define    INT08S       signed      char                                      // 有符号 8位整型变量
#define    INT16U       unsigned    short                                     // 无符号16位整型变量
#define    INT16S       signed      short                                     // 有符号16位整型变量 
#define    INT32U       unsigned    int                                       // 无符号32位整型变量
#define    INT32S       signed      int                                       // 有符号32位整型变量
#define    INT64U       unsigned    long long                                 // 无符号64位整型变量
#define    INT64S       signed      long long                                 // 有符号64位整型变量
#define    FLOAT32      float                                                 // 单精度浮点数(32位长度)
#define    FLOAT64      double                                                // 双精度浮点数(64位长度)

#define    INT8U        uint8_t
#define    BOOL         int
#define    BYTE         unsigned   char 


// 定义颜色的宏
#define WHITE                   0xFFFF
#define BLACK                   0x0000
#define BLUE                    0x001F
#define RED                     0xF800
#define MAGENTA                 0xF81F
#define GREEN                   0x07E0
#define CYAN                    0x7FFF
#define YELLOW                  0xFFE0                      //定义颜色的宏
#define GREY                    0xc618   //灰
#define lan                     0x051D


#define BOARD_LCD_WIDTH             (320 - 1)
#define BOARD_LCD_HEIGHT            (480 - 1)    




/*****************************************************************************************
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2009.09.08
功能说明:   配置开关
其它说明:   无
*****************************************************************************************/
#define UART1_BPS                       115200                          // 用户速度 
#define UART2_BPS                       115200                          // 蓝牙速度  
#define UART3_BPS                       115200                          // 蓝牙速度  
#define UART4_BPS                       115200                          // 蓝牙速度  
#define UART5_BPS                       115200                          // 蓝牙速度  




/*****************************************************************************************
版权所有:   影舞者
版本号:        1.0.0
生成日期:    2009.09.08
功能说明:   配置开关
其它说明:   无
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
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2009.10.13
功能说明:   结构对齐
其它说明:   无
*****************************************************************************************/
#define __APP_PACKED(n)        __attribute__ ((packed, aligned(n)))            // 结构对齐 

extern  void  TICK_DisableIRQ(void);
extern  void  TICK_EnableIRQ(void);

extern  void  Delay_Ms(uint32 nCount);
extern  void  Delay_Us(uint32 nCount);
extern  void  Delay(uint32 nCount);

extern  u16   crc16(u16 crc, u8 const *buffer, size_t len);
extern  void  DebugMessage(uint8 * message, uint32 number);




/*****************************************************************************************
版权所有:   影舞者
版本号:     1.0.0
生成日期:   2009.10.04
功能说明:   系统函数
其它说明:   无
*****************************************************************************************/
#if (DEBUG_SUPPORT == 1)
    #define dprintf(fmt, args...)       printf(fmt, ##args)                // 信息打印
#endif
#if (DEBUG_SUPPORT == 0)
    #define dprintf(fmt, args...)
#endif

#define ENTER_CRITICAL()               // TICK_DisableIRQ()                  // 进入中断
#define EXIT_CRITICAL()                // TICK_EnableIRQ()                   // 退出中断

#define APP_crc16(a,b,c)                crc16(a,b,c)                       // CRC 16

#define qprintf(fmt, args...)           dprintf(fmt, ##args)               // 打印
#define mprintf(fmt, args...)           dprintf(fmt, ##args)               // 打印
#define pprintf(fmt, args...)           dprintf(fmt, ##args)               // 打印
#define bprintf(fmt, args...)           dprintf(fmt, ##args)               // 打印
#define DBG_printf(fmt, args...)        dprintf(fmt, ##args)               // 打印
#define APP_printf(fmt, args...)        dprintf(fmt, ##args)               // 打印

#define APP_sprintf                     sprintf                            // 打印
#define APP_malloc(a)                   malloc(a)                          // 申请内存
#define APP_free(a)                     free(a)                            // 释放内存
#define APP_strlen(a)                   strlen(a)                          // 长度
#define APP_sizeof(a)                   sizeof(a)                          // 大小
#define APP_strcat(a, b)                strcat(a, b)                       // 连接
#define APP_strstr(a, b)                strstr(a, b)                       // 查找字条
#define APP_strchr(a, b)                strchr(a, b)                       // 查找字条
#define APP_strcmp(a, b)                strcmp(a, b)                       // 比较
#define APP_strncmp(a, b, n)            strncmp(a, b, n)                   // 比较
#define APP_strcpy(a, b)                strcpy(a, b)                       // 拷贝
#define APP_strncpy(a, b, n)            strncpy(a, b, n)                   // 拷贝
#define APP_abs(a)                      abs(a)                             // 绝对值
#define APP_fabs(a)                     fabs(a)                            // 绝对值
#define APP_atoi(a)                     atoi(a)                            // 字符串转整数
#define APP_atol(a)                     atol(a)                            // 字符串转整数
#define APP_atof(a)                     atof(a)                            // 字符串转整数
#define APP_memset(a, b, n)             memset(a, b, n)                    // 内存设置
#define APP_memcpy(a, b, n)             memcpy(a, b, n)                    // 内存拷贝
#define APP_memcmp(a, b, n)             memcmp(a, b, n)                    // 内存比较
#define APP_memmove(a, b, n)            memmove(a, b, n)                   // 内存移动

#define GUI_sprintf                     sprintf                            // 打印
#define GUI_strlen(a)                   strlen(a)                          // 长度
#define GUI_sizeof(a)                   sizeof(a)                          // 大小
#define GUI_strcmp(a, b)                strcmp(a, b)                       // 比较
#define GUI_strncmp(a, b, n)            strncmp(a, b, n)                   // 比较
#define GUI_strcpy(a, b)                strcpy(a, b)                       // 拷贝
#define GUI_strncpy(a, b, n)            strncpy(a, b, n)                   // 拷贝
#define GUI_atol(a)                     atol(a)                            // 字符串转整数
#define GUI_memset(a, b, n)             memset(a, b, n)                    // 内存设置
#define GUI_memcpy(a, b, n)             memcpy(a, b, n)                    // 内存拷贝




#endif


