// led�ӿ�
// ʹ�õ͵�ƽ����led���ߵ�ƽϨ��led


#include "ls1c_gpio.h"


// ��ʼ��led
// @led_gpio led����gpio����
void led_init(unsigned int led_gpio)
{
    gpio_init(led_gpio, gpio_mode_output);
    gpio_set(led_gpio, gpio_level_high);        // ָʾ��Ĭ��Ϩ��

    return ;
}


// ����led
// @led_gpio led����gpio����
void led_on(unsigned int led_gpio)
{
    gpio_set(led_gpio, gpio_level_low);

    return ;
}


// Ϩ��led
// @led_gpio
void led_off(unsigned int led_gpio)
{
    gpio_set(led_gpio, gpio_level_high);

    return ;
}




