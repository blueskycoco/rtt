// led�ӿ�
// ʹ�õ͵�ƽ����led���ߵ�ƽϨ��led

#ifndef __OPENLOONGSON_LED_H
#define __OPENLOONGSON_LED_H


// ��ʼ��led
// @led_gpio led����gpio����
void led_init(unsigned int led_gpio);


// ����led
// @led_gpio led����gpio����
void led_on(unsigned int led_gpio);


// Ϩ��led
// @led_gpio
void led_off(unsigned int led_gpio);





#endif

