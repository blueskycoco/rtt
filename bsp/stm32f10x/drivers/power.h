#ifndef __POWER_H
#define __POWER_H
#include "stm32f10x.h"
#ifdef __cplusplus
extern "C" {
#endif
#define BATS_SEL_C_PIN              (GPIO_Pin_2)
#define BATS_SEL_C_PORT             (GPIOE)
#define BATS_SEL_B_PIN              (GPIO_Pin_3)
#define BATS_SEL_B_PORT             (GPIOE)
#define BATS_SEL_A_PIN              (GPIO_Pin_4)
#define BATS_SEL_A_PORT             (GPIOE)
#define BATS_SEL_STAC_PIN           (GPIO_Pin_5)
#define BATS_SEL_STAC_PORT          (GPIOE)
#define BATS_SEL_STAB_PIN           (GPIO_Pin_6)
#define BATS_SEL_STAB_PORT          (GPIOE)
#define BATS_SEL_STAA_PIN           (GPIO_Pin_0)
#define BATS_SEL_STAA_PORT          (GPIOF)
#define BATS_ABC_CHARGE_CTL_PIN     (GPIO_Pin_1)
#define BATS_ABC_CHARGE_CTL_PORT    (GPIOF)
#define BATS_ABC_CHARGE_FAULT_PIN   (GPIO_Pin_2)
#define BATS_ABC_CHARGE_FAULT_PORT  (GPIOF)
#define BATS_ABC_CHARGE_CHRG_PIN    (GPIO_Pin_3)
#define BATS_ABC_CHARGE_CHRG_PORT   (GPIOF)
#define PG_3V3_PIN                  (GPIO_Pin_5)
#define PG_3V3_PORT                 (GPIOF)
#define BATS_A_CHARGE_STAT_PIN      (GPIO_Pin_2)
#define BATS_A_CHARGE_STAT_PORT     (GPIOG)
#define BATS_A_CHARGE_CTL_PIN       (GPIO_Pin_3)
#define BATS_A_CHARGE_CTL_PORT      (GPIOG)
#define BATS_B_CHARGE_STAT_PIN      (GPIO_Pin_4)
#define BATS_B_CHARGE_STAT_PORT     (GPIOG)
#define BATS_B_CHARGE_CTL_PIN       (GPIO_Pin_5)
#define BATS_B_CHARGE_CTL_PORT      (GPIOG)
#define BATS_C_CHARGE_STAT_PIN      (GPIO_Pin_6)
#define BATS_C_CHARGE_STAT_PORT     (GPIOG)
#define BATS_C_CHARGE_CTL_PIN       (GPIO_Pin_7)
#define BATS_C_CHARGE_CTL_PORT      (GPIOG)
#define BATS_I2C_SCL_PIN            (GPIO_Pin_10)
#define BATS_I2C_SCL_PORT           (GPIOB)
#define BATS_I2C_SDA_PIN            (GPIO_Pin_11)
#define BATS_I2C_SDA_PORT           (GPIOB)
#define BATS_I2C_SEL_PIN            (GPIO_Pin_15)
#define BATS_I2C_SEL_PORT						(GPIOA)
#define BATS_AB_MON_I_PIN           (GPIO_Pin_6)
#define BATS_AB_MON_I_PORT          (GPIOF)
#define V3P3_MON_V_PIN              (GPIO_Pin_7)
#define V3P3_MON_V_PORT             (GPIOF)
#define BATS_A_V_MON_PIN            (GPIO_Pin_8)
#define BATS_A_V_MON_PORT           (GPIOF)
#define BATS_B_V_MON_PIN            (GPIO_Pin_9)
#define BATS_B_V_MON_PORT           (GPIOF)
#define BATS_C_V_MON_PIN            (GPIO_Pin_10)
#define BATS_C_V_MON_PORT           (GPIOF)
#define ARP_ADDRESS                 0x61
#define ARP_PREPARE                 0x01
#define ARP_RESET_DEV               0x02
#define ARP_GET_UDID_GEN            0x03
#define ARP_ASSIGN_ADDR             0x04
#define UDID_LENGTH                 0x11
#define SMBUS_ADDRESS_SIZE          0x80
#define ARP_FREE                    0       
#define ARP_RESERVED                1
#define ARP_BUSY                    2
#define I2C_SMBUS_BLOCK_MAX         32
typedef enum PowerType {
    POWER_NOT_DEFINED = 0,                 // 未定义
    POWER_ADAPTER    = 1,                 // 适配器
    POWER_BATTERY_LI  = 2,                 // 锂电池
    POWER_BATTERY_DRY = 3,                 // 干电池
    POWER_UNKNOWN    = 4,                 // 未知类型
}PowerType_t;
typedef struct PowerSource PowerSource_t;
struct PowerSource{
    unsigned long   ID;             //电源的序列号
    PowerType_t type;               //电源类型
    int16_t     voltage;            //电源当前电压，单位V，前8个bit是整数部分 
    int16_t     current;            //电源当前电流，单位A，前8个bit是整数部分 
    int16_t     power;              //电源当前功率，单位W，前8个bit是整数部分 
    int16_t     available;          //电源是否有效 1 有效 0 无效
    int16_t     energyAll;          //电池总容量(单位：瓦时)，前12个bit是整数部分 
    int16_t     energyNow;          //电池当前电量（单位：瓦时），前12个bit是整数部分
    int16_t     cycleNum;           //电池已经充放电循环了多少次，16个bit都是整数部分
    int16_t     thisWorkTime;       //在该次放电中的累积时间，单位分钟， 16个bit都是整数部分，整数饱和不能溢出
    //...
    //其它电池中有的数据
}; 
#define PS_NUM 3
struct PowerMan{
    PowerSource_t ps[PS_NUM];
    int16_t currentPs;              //当前使用那个电源。   范围：0,1,...,PS_NUM-1.
    int16_t power;                  //电源当前功率
    int16_t minVoltage;             //最小电压
    int16_t maxVoltage;             //最大电压
};
typedef struct PowerMan PowerMan_t;
/*power系统的初始化*/
uint8_t power_man_init(int16_t min_vol,int16_t max_vol);
void power_man_timer_interrupt(void);
PowerMan_t power_man_timer_poll(int16_t min_vol,int16_t max_vol);
#ifdef __cplusplus
}
#endif

#endif
