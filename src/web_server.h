#ifndef _TCPIP_H
#define _TCPIP_H

int Web_Server(void);
void SetIpMac(void);

#define PSTR(s) s

/* LED1 亮灭宏定义 */
//#define LED1_ON()  GPIO_SetBits(GPIOC, GPIO_PIN_3);
//#define LED1_ON()  GPIO_SetBits(GPIOC, GPIO_PIN_3);
//#define LED1_OFF()  GPIO_ResetBits(GPIOC, GPIO_PIN_3);
//#define LED1_OFF()  GPIO_ResetBits(GPIOC, GPIO_PIN_3);

#endif


