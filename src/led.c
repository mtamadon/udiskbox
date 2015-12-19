/******************** (C) COPYRIGHT 2011 野火嵌入式开发工作室 ********************
 * 文件名  ：led.c
 * 描述    ：led 应用函数库
 *
 * 实验平台：野火STM32开发板
 * 硬件连接：-----------------
 *          |   PC3 - LED1     |
 *          |   PC4 - LED2     |
 *          |   PC5 - LED3     |
 *           -----------------
 * 库版本  ：ST3.0.0
 *
 * 作者    ：fire  QQ: 313303034
 * 博客    ：firestm32.blog.chinaunix.net
 **********************************************************************************/

#include "led.h"

void LED_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /*RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);*/
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);	 // turn off all led
}



