/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���  ��led.c
 * ����    ��led Ӧ�ú�����
 *          
 * ʵ��ƽ̨��Ұ��STM32������
 * Ӳ�����ӣ�-----------------
 *          |   PC3 - LED1     |
 *          |   PC4 - LED2     |
 *          |   PC5 - LED3     |
 *           ----------------- 
 * ��汾  ��ST3.0.0
 *
 * ����    ��fire  QQ: 313303034
 * ����    ��firestm32.blog.chinaunix.net 
**********************************************************************************/

#include "led.h"

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOD, GPIO_Pin_13 );	 // turn off all led
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOG, GPIO_Pin_14 );	 // turn off all led
}



