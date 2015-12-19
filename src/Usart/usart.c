//头文件调用
#include "usually.h"
#include "usart.h"
#include "PICC.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus：得到发送状态位
														  //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USART1,Data);						  //发送一个字符
	   
	return Data;										  //返回一个值
}
#endif 

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Send_Byte(uint16_t Data)
{ 
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus：得到发送状态位
														  //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USART1,Data);						  //发送一个字符
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_Send_Byte
** 功能描述: 串口发送一个字符串
** 参数描述：Data 要发送的数据
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
uint8_t USART1_Receive_Byte(void)
{ 
   	while(!(USART_GetFlagStatus(USART1,USART_FLAG_RXNE))); //USART_GetFlagStatus：得到接收状态位
														   //USART_FLAG_RXNE:接收数据寄存器非空标志位 
														   //1：忙状态  0：空闲(没收到数据，等待。。。)
	return USART_ReceiveData(USART1);					   //接收一个字符
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Usart_Init
** 功能描述: 串口引脚初始化，初始化USART1、USART2、USART3
** 参数描述: 无
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_Usart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//定义一个GPIO结构体变量

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //USART1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //USART2
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE); //USART3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	
	
															//使能各个端口时钟，重要！！！

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 				//配置串口接收端口挂接到2端口
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//配置串口接收端口挂接到9端口
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 				//配置串口接收端口挂接到10端口 USART3
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用功能输出开漏
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);	   				
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
		
		
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Usart_Configuration
** 功能描述: 串口配置函数
** 参数描述: BUSART_X表示串口号，baudRate设置波特率 
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Usart_Configuration(USART_TypeDef * USART_X, uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//定义一个串口结构体

	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//波特率115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_Init(USART_X, &USART_InitStructure);								//根据参数初始化串口寄存器
	USART_ITConfig(USART_X,USART_IT_RXNE,ENABLE);							//使能串口中断接收
	USART_Cmd(USART_X, ENABLE);     											//使能串口外设
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART1_IRQHandler
** 功能描述: 串口中断函数
** 参数描述: 无 
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_IRQHandler()
{
	//uint8_t ReceiveDate;								//定义一个变量存放接收的数据
	if(!(USART_GetITStatus(USART1,USART_IT_RXNE))); 	//读取接收中断标志位USART_IT_RXNE 
														//USART_FLAG_RXNE:接收数据寄存器非空标志位 
														//1：忙状态  0：空闲(没收到数据，等待。。。)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//清楚中断标志位
// 		ReceiveDate=USART_ReceiveData(USART1);			//接收字符存入数组
// 		printf("%d",(char*)ReceiveDate);				//以十进制输出输入的值,从这里我们可以看到键盘上所有的键值对应的十进制数
// 														//比如输入"1"对应的输出49 、 输入"A" 对应的值为65
// 		printf("\n\r");									//换行置顶
	}  
}
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
	static unsigned char bTemp, flag=0;
	static unsigned char sflag = 0;
	
	/* 串口接收 */
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
	{
		/* 清串口中断标记 */
		bTemp = USART_ReceiveData(USART2);
		/* 上条命令处理完成才接收 */
		if(Cmd.ReceiveFlag == 0)
		{
			/* 7F 标记，为 0 表示上一个数据不是7F */
			if(flag == 0)
			{
				/* 上一个标记不是7F，这个是，打上标记 */
				if(bTemp == 0x7F){flag = 1;}
				/* 把值存进接收缓存 */
				Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
			}
			else
			{
				flag = 0;
				/* 上一个是7F，这一个不是表示收到【命令头】 */
				if(bTemp != 0x7F)
				{
					Cmd.ReceivePoint = 0;
					Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
				}
				/* 上一个是7F，这一个也是，则忽略当前的7F */
			}
			if(Cmd.ReceivePoint >= 32){Cmd.ReceivePoint = 0;}
			/* 接收指针大于 2 个开始对比命令长度和接收指针，一致表示收到完整命令 */
			if(Cmd.ReceivePoint > 2){if(Cmd.ReceivePoint == Cmd.ReceiveBuffer[0]+1){Cmd.ReceiveFlag = 1;}}
		}
	}
	/* 发送 */
	if(USART_GetFlagStatus(USART2, USART_FLAG_TC) == SET)
	{
		USART_ClearFlag(USART2, USART_FLAG_TC);
		/* 发送指针不为0时继续发送 */
		if(Cmd.SendPoint != 0)
		{
			/* 7F判断 */
			if(sflag == 0)
			{
				Cmd.SendPoint--;
				USART_SendData(USART2, Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint]);
				if(Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint] == 0x7F){sflag = 1;}
			}
			else
			{
				sflag = 0;USART_SendData(USART2, 0x7F);
			}
		}
		/* 发送指针为0时打上发送标记表示发送完成 */
		else
		{
			Cmd.SendFlag = 0;
		}
	}
	USART2->SR = 0;
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: Lcd_Display
** 功能描述: LCD显示成像
** 参数描述: 无 
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void Lcd_Display(char * buf1)
{	u8 i=0;
	while (1)
	{	if (buf1[i]!=0)
		{	USART_SendData(USART3, buf1[i]);
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}; //判断是否发送完毕
			i++;
		}
		else return;
	}
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** 函数名称: USART2_SendString
** 功能描述: USART2发送字符串
** 参数描述: 无 
** 作  　者: Dream
** 日　  期: 2011年6月20日
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void USART2_SendString(char * buf1)
{	u8 i=0;
	while (1)
	{	if (buf1[i]!=0)
		{	USART_SendData(USART2, buf1[i]);
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}; //判断是否发送完毕
			i++;
		}
		else return;
	}
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
