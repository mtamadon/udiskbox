//ͷ�ļ�����
#include "usually.h"
#include "usart.h"
#include "PICC.h"

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus���õ�����״̬λ
														  //USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART1,Data);						  //����һ���ַ�
	   
	return Data;										  //����һ��ֵ
}
#endif 

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_Send_Byte
** ��������: ���ڷ���һ���ַ���
** ����������Data Ҫ���͵�����
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_Send_Byte(uint16_t Data)
{ 
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus���õ�����״̬λ
														  //USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART1,Data);						  //����һ���ַ�
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_Send_Byte
** ��������: ���ڷ���һ���ַ���
** ����������Data Ҫ���͵�����
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
uint8_t USART1_Receive_Byte(void)
{ 
   	while(!(USART_GetFlagStatus(USART1,USART_FLAG_RXNE))); //USART_GetFlagStatus���õ�����״̬λ
														   //USART_FLAG_RXNE:�������ݼĴ����ǿձ�־λ 
														   //1��æ״̬  0������(û�յ����ݣ��ȴ�������)
	return USART_ReceiveData(USART1);					   //����һ���ַ�
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Usart_Init
** ��������: �������ų�ʼ������ʼ��USART1��USART2��USART3
** ��������: ��
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_Usart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;					//����һ��GPIO�ṹ�����

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); //USART1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //USART2
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE); //USART3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	
	
															//ʹ�ܸ����˿�ʱ�ӣ���Ҫ������

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 				//���ô��ڽ��ն˿ڹҽӵ�2�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//���ù��������©
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//���ô��ڽ��ն˿ڹҽӵ�9�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//���ù��������©
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);	   				
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 				//���ô��ڽ��ն˿ڹҽӵ�10�˿� USART3
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//���ù��������©
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);	   				
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
		
		
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Usart_Configuration
** ��������: �������ú���
** ��������: BUSART_X��ʾ���ںţ�baudRate���ò����� 
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Usart_Configuration(USART_TypeDef * USART_X, uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//����һ�����ڽṹ��

	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//������115200
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//���������ʹ��8λ����
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//��֡��β����1λֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ����ʧ��
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	USART_Init(USART_X, &USART_InitStructure);								//���ݲ�����ʼ�����ڼĴ���
	USART_ITConfig(USART_X,USART_IT_RXNE,ENABLE);							//ʹ�ܴ����жϽ���
	USART_Cmd(USART_X, ENABLE);     											//ʹ�ܴ�������
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART1_IRQHandler
** ��������: �����жϺ���
** ��������: �� 
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void USART1_IRQHandler()
{
	//uint8_t ReceiveDate;								//����һ��������Ž��յ�����
	if(!(USART_GetITStatus(USART1,USART_IT_RXNE))); 	//��ȡ�����жϱ�־λUSART_IT_RXNE 
														//USART_FLAG_RXNE:�������ݼĴ����ǿձ�־λ 
														//1��æ״̬  0������(û�յ����ݣ��ȴ�������)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//����жϱ�־λ
// 		ReceiveDate=USART_ReceiveData(USART1);			//�����ַ���������
// 		printf("%d",(char*)ReceiveDate);				//��ʮ������������ֵ,���������ǿ��Կ������������еļ�ֵ��Ӧ��ʮ������
// 														//��������"1"��Ӧ�����49 �� ����"A" ��Ӧ��ֵΪ65
// 		printf("\n\r");									//�����ö�
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
	
	/* ���ڽ��� */
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
	{
		/* �崮���жϱ�� */
		bTemp = USART_ReceiveData(USART2);
		/* �����������ɲŽ��� */
		if(Cmd.ReceiveFlag == 0)
		{
			/* 7F ��ǣ�Ϊ 0 ��ʾ��һ�����ݲ���7F */
			if(flag == 0)
			{
				/* ��һ����ǲ���7F������ǣ����ϱ�� */
				if(bTemp == 0x7F){flag = 1;}
				/* ��ֵ������ջ��� */
				Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
			}
			else
			{
				flag = 0;
				/* ��һ����7F����һ�����Ǳ�ʾ�յ�������ͷ�� */
				if(bTemp != 0x7F)
				{
					Cmd.ReceivePoint = 0;
					Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
				}
				/* ��һ����7F����һ��Ҳ�ǣ�����Ե�ǰ��7F */
			}
			if(Cmd.ReceivePoint >= 32){Cmd.ReceivePoint = 0;}
			/* ����ָ����� 2 ����ʼ�Ա�����Ⱥͽ���ָ�룬һ�±�ʾ�յ��������� */
			if(Cmd.ReceivePoint > 2){if(Cmd.ReceivePoint == Cmd.ReceiveBuffer[0]+1){Cmd.ReceiveFlag = 1;}}
		}
	}
	/* ���� */
	if(USART_GetFlagStatus(USART2, USART_FLAG_TC) == SET)
	{
		USART_ClearFlag(USART2, USART_FLAG_TC);
		/* ����ָ�벻Ϊ0ʱ�������� */
		if(Cmd.SendPoint != 0)
		{
			/* 7F�ж� */
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
		/* ����ָ��Ϊ0ʱ���Ϸ��ͱ�Ǳ�ʾ������� */
		else
		{
			Cmd.SendFlag = 0;
		}
	}
	USART2->SR = 0;
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Lcd_Display
** ��������: LCD��ʾ����
** ��������: �� 
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void Lcd_Display(char * buf1)
{	u8 i=0;
	while (1)
	{	if (buf1[i]!=0)
		{	USART_SendData(USART3, buf1[i]);
			while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET){}; //�ж��Ƿ������
			i++;
		}
		else return;
	}
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: USART2_SendString
** ��������: USART2�����ַ���
** ��������: �� 
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

void USART2_SendString(char * buf1)
{	u8 i=0;
	while (1)
	{	if (buf1[i]!=0)
		{	USART_SendData(USART2, buf1[i]);
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){}; //�ж��Ƿ������
			i++;
		}
		else return;
	}
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
