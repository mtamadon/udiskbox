#ifndef __STM32F10_DELAY
#define __STM32F10_DELAY
#include <stm32f1xx_hal_conf.h>
void SysTick_Configuration(void);
void Delay(u32 nms);
#endif
