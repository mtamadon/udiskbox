#ifndef __LED_H
#define	__LED_H

#include <stm32f1xx_hal_conf.h>

#define ON	0
#define OFF	1

#define LED1(a)	if (a)	\
					GPIO_SetBits(GPIOD,GPIO_Pin_13);\
					else		\
					GPIO_ResetBits(GPIOD,GPIO_Pin_13)

#define LED2(a)	if (a)	\
					GPIO_SetBits(GPIOG,GPIO_Pin_14);\
					else		\
					GPIO_ResetBits(GPIOG,GPIO_Pin_14)


void LED_GPIO_Config(void);

#endif /* __LED_H */
