#include <stm32f1xx_hal.h>

#define NOP() __NOP

#define DS1302_CLK_H()	(GPIOE->BSRR=GPIO_PIN_4)
#define DS1302_CLK_L()	(GPIOE->BRR=GPIO_PIN_4)

#define DS1302_RST_H()	(GPIOE->BSRR=GPIO_PIN_6)
#define DS1302_RST_L()	(GPIOE->BRR=GPIO_PIN_6)

#define DS1302_OUT_H()	(GPIOE->BSRR=GPIO_PIN_5)
#define DS1302_OUT_L()	(GPIOE->BRR=GPIO_PIN_5)

#define DS1302_IN_X		(GPIOE->IDR&GPIO_PIN_5)

#define Time_24_Hour	0x00	//24ʱ�ƿ���
#define Time_Start		0x00	//��ʼ��ʱ

#define DS1302_SECOND	0x80	//DS1302���Ĵ������������
#define DS1302_MINUTE	0x82
#define DS1302_HOUR		0x84
#define DS1302_DAY		0x86
#define DS1302_MONTH	0x88
#define DS1302_WEEK		0x8A
#define DS1302_YEAR		0x8C
#define DS1302_WRITE	0x8E
#define DS1302_POWER	0x90

static GPIO_InitTypeDef GPIO_InitStructure;

void DS1302_Configuration(void)
{
	/*RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);*/
    __HAL_RCC_GPIOE_CLK_ENABLE();

	GPIO_InitStructure.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	//�������
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;	//50Mʱ���ٶ�
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* PE4,5,6��� */
	GPIO_ResetBits(GPIOE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
}


static void DelayNOP(uint32_t count)
{
	while(count--) NOP();
}

static void DS1302_OUT(void)
{
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}

static void DS1302_IN(void)
{
	GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void DS1302SendByte(uint8_t byte)
{
	uint8_t	i;

	for(i=0x01;i;i<<=1)
	{
		if(byte&i)
            DS1302_OUT_H();
		else
            DS1302_OUT_L();
		DS1302_CLK_H();
		DelayNOP(50);		//����ʱ
		DS1302_CLK_L();
	}
}

uint8_t DS1302ReceiveByte(void)
{
	uint8_t	i,byte=0;

	for(i=0x01;i;i<<=1)
	{
		if(DS1302_IN_X)
            byte |= i;
		DS1302_CLK_H();
		DelayNOP(50);		//����ʱ
		DS1302_CLK_L();
	}
	return(byte);
}

void Write1302(uint8_t addr,uint8_t data)
{
    DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(addr);
	DS1302SendByte(data);
	DelayNOP(100);
	DS1302_RST_L();
}

uint8_t Read1302(uint8_t addr)
{
    uint8_t data=0;

    DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(addr|0x01);
	DS1302_IN();
	data = DS1302ReceiveByte();
	DelayNOP(100);
	DS1302_RST_L();
	return(data);
}

//��ȡʱ�亯��
void DS1302_GetTime(uint8_t *time)
{
	//uint8_t tmp;

	time[0] = Read1302(DS1302_YEAR);
	time[1] = Read1302(DS1302_WEEK);
	time[2] = Read1302(DS1302_MONTH);
	time[3] = Read1302(DS1302_DAY);
	time[4] = Read1302(DS1302_HOUR);
	time[5] = Read1302(DS1302_MINUTE);
	time[6] = Read1302(DS1302_SECOND);
}

/*
��ȡDS1302�е�RAM
addr:��ַ,��0��30,��31���ֽڵĿռ�
����Ϊ����ȡ������
*/
uint8_t ReadDS1302Ram(uint8_t addr)
{
	uint8_t	tmp,res;

	tmp = (addr<<1)|0xc0;
	res = Read1302(tmp);
	return(res);
}

/*
дDS1302�е�RAM
addr:��ַ,��0��30,��31���ֽڵĿռ�
data:Ҫд������
*/
void WriteDS1302Ram(uint8_t addr,uint8_t data)
{
	uint8_t	tmp;

	Write1302(DS1302_WRITE,0x00);		//�ر�д����
	tmp = (addr<<1)|0xc0;
	Write1302(tmp,data);
	Write1302(DS1302_WRITE,0x80);		//��д����
}

void ReadDSRam(uint8_t *p,uint8_t add,uint8_t cnt)
{
	uint8_t i;

	if(cnt>30) return;
	for(i=0;i<cnt;i++)
	{
		*p = ReadDS1302Ram(add+1+i);
		p++;
	}
}

void WriteDSRam(uint8_t *p,uint8_t add,uint8_t cnt)
{
	uint8_t i;

	if(cnt>30) return;
	for(i=0;i<cnt;i++)
	{
		WriteDS1302Ram(add+1+i,*p++);
	}
}

/*
��ʱ�亯��,˳��Ϊ:��������ʱ����
*/
void ReadDS1302Clock(uint8_t *p)
{
	DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(0xbf);			//ͻ��ģʽ
	DS1302_IN();
	p[5] = DS1302ReceiveByte();		//��
	p[4] = DS1302ReceiveByte();		//��
	p[3] = DS1302ReceiveByte();		//ʱ
	p[2] = DS1302ReceiveByte();		//��
	p[1] = DS1302ReceiveByte();		//��
	DS1302ReceiveByte();			//��
	p[0] = DS1302ReceiveByte();		//��
	DS1302ReceiveByte();			//������־�ֽ�
	DelayNOP(100);
	DS1302_RST_L();
}

/*
дʱ�亯��,˳��Ϊ:��������ʱ����
*/
void WriteDS1302Clock(uint8_t *p)
{
	Write1302(DS1302_WRITE,0x00);		//�ر�д����
	DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(0xbe);				//ͻ��ģʽ
	DS1302SendByte(p[5]);				//��
	DS1302SendByte(p[4]);				//��
	DS1302SendByte(p[3]);				//ʱ
	DS1302SendByte(p[2]);				//��
	DS1302SendByte(p[1]);				//��
	DS1302SendByte(0x01);				//�ܣ����ó���һ��û��ʹ��
	DS1302SendByte(p[0]);				//��
	DS1302SendByte(0x80);				//������־�ֽ�
	DelayNOP(100);
	DS1302_RST_L();
}
void InitClock(void)
{
	uint8_t	tmp;

	DS1302_Configuration();
	tmp = ReadDS1302Ram(0);
	if(tmp^0xa5)
	{
		WriteDS1302Ram(0,0xa5);
		Write1302(DS1302_WRITE,0x00);		//�ر�д����
		Write1302(0x90,0x03);				//��ֹ������
		Write1302(DS1302_HOUR,0x00);		//���ó�24Сʱ��
		Write1302(DS1302_SECOND,0x00);		//ʹ��ʱ������
		Write1302(DS1302_WRITE,0x80);		//��д����
	}
}
/*
void TestDS1302(void)
{
	uint8_t i,tt[7],dd1[30],dd2[30];

	DS1302_Configuration();
	InitClock();
	tt[0] = 0x13;
	tt[1] = 0x05;
	tt[2] = 0x23;
	tt[3] = 0x09;
	tt[4] = 0x25;
	tt[5] = 0x00;
	WriteDS1302Clock(tt);
	for(i=0;i<30;i++)
	{
		dd1[i] = i;
		dd2[i] = 0;
	}
	WriteDSRam(dd1,0,30);
	ReadDSRam(dd2,0,30);
	while(1)
	{
		ReadDS1302Clock(tt);
	}
}
*/
//end

