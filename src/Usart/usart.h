#ifndef __USART_H
#define __USART_H

//头文件包含
#include "stm32f1xx_hal.h"
#include "stdio.h"
#include <string.h>

//函数声明
void USART1_Send_Byte(uint16_t dat);
uint8_t USART1_Receive_Byte(void);
void Init_UsartGpio(void);
void DebugLogConsoleConfig(USART_TypeDef * USART_X);
void RFIDUSARTConfig(USART_TypeDef * USART_X);
void LCDUSARTConfig(USART_TypeDef * USART_X);
void Usart_Configuration(USART_TypeDef * USART_X, uint32_t BaudRate);
void Lcd_Display(char * buf1);
void USART2_SendString(char * buf1);

HAL_StatusTypeDef USART_SendByte(USART_HandleTypeDef *p_USARTHandle, uint8_t DATA);

#endif
