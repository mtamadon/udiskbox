#ifndef __USART_H
#define __USART_H

//头文件包含
#include "stdio.h"
#include <string.h>

//函数声明
void USART1_Send_Byte(uint16_t dat);
uint8_t USART1_Receive_Byte(void);
void Init_Usart(void);
void Usart_Configuration(USART_TypeDef * USART_X, uint32_t BaudRate);
void Lcd_Display(char * buf1);
void USART2_SendString(char * buf1);

#endif
