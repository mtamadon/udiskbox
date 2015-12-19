#ifndef __LED_H
#define	__LED_H

#include <stm32f1xx_hal_conf.h>

#define ON	0
#define OFF	1

#define LED1(a) do {if (a) GPIO_SetBits(GPIOD,GPIO_PIN_13);\
    else GPIO_ResetBits(GPIOD,GPIO_PIN_13);} while(0);

#define LED2(a) do {if (a) GPIO_SetBits(GPIOG,GPIO_PIN_14);\
    else GPIO_ResetBits(GPIOG,GPIO_PIN_14);} while(0);

void LED_GPIO_Config(void);

#endif /* __LED_H */
