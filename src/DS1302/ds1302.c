#include <stm32f1xx_hal.h>

#define NOP() __NOP

#define DS1302_CLK_H()	(GPIOE->BSRR=GPIO_PIN_4)
#define DS1302_CLK_L()	(GPIOE->BRR=GPIO_PIN_4) //TODO: donot use BRR

#define DS1302_RST_H()	(GPIOE->BSRR=GPIO_PIN_6)
#define DS1302_RST_L()	(GPIOE->BRR=GPIO_PIN_6)

#define DS1302_OUT_H()	(GPIOE->BSRR=GPIO_PIN_5)
#define DS1302_OUT_L()	(GPIOE->BRR=GPIO_PIN_5)

#define DS1302_IN_X		(GPIOE->IDR&GPIO_PIN_5)

#define Time_24_Hour	0x00	//24时制控制
#define Time_Start		0x00	//开始走时

#define DS1302_SECOND	0x80	//DS1302各寄存器操作命令定义
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
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;	//50M时钟速度
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* PE4,5,6输出 */
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
		DelayNOP(50);		//加延时
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
		DelayNOP(50);		//加延时
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

//读取时间函数
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
读取DS1302中的RAM
addr:地址,从0到30,共31个字节的空间
返回为所读取的数据
*/
uint8_t ReadDS1302Ram(uint8_t addr)
{
	uint8_t	tmp,res;

	tmp = (addr<<1)|0xc0;
	res = Read1302(tmp);
	return(res);
}

/*
写DS1302中的RAM
addr:地址,从0到30,共31个字节的空间
data:要写的数据
*/
void WriteDS1302Ram(uint8_t addr,uint8_t data)
{
	uint8_t	tmp;

	Write1302(DS1302_WRITE,0x00);		//关闭写保护
	tmp = (addr<<1)|0xc0;
	Write1302(tmp,data);
	Write1302(DS1302_WRITE,0x80);		//打开写保护
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
读时间函数,顺序为:年周月日时分秒
*/
void ReadDS1302Clock(uint8_t *p)
{
	DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(0xbf);			//突发模式
	DS1302_IN();
	p[5] = DS1302ReceiveByte();		//秒
	p[4] = DS1302ReceiveByte();		//分
	p[3] = DS1302ReceiveByte();		//时
	p[2] = DS1302ReceiveByte();		//日
	p[1] = DS1302ReceiveByte();		//月
	DS1302ReceiveByte();			//周
	p[0] = DS1302ReceiveByte();		//年
	DS1302ReceiveByte();			//保护标志字节
	DelayNOP(100);
	DS1302_RST_L();
}

/*
写时间函数,顺序为:年周月日时分秒
*/
void WriteDS1302Clock(uint8_t *p)
{
	Write1302(DS1302_WRITE,0x00);		//关闭写保护
	DS1302_OUT();
	DS1302_RST_L();
	DS1302_CLK_L();
	DS1302_RST_H();
	DelayNOP(100);
	DS1302SendByte(0xbe);				//突发模式
	DS1302SendByte(p[5]);				//秒
	DS1302SendByte(p[4]);				//分
	DS1302SendByte(p[3]);				//时
	DS1302SendByte(p[2]);				//日
	DS1302SendByte(p[1]);				//月
	DS1302SendByte(0x01);				//周，设置成周一，没有使用
	DS1302SendByte(p[0]);				//年
	DS1302SendByte(0x80);				//保护标志字节
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
		Write1302(DS1302_WRITE,0x00);		//关闭写保护
		Write1302(0x90,0x03);				//禁止涓流充电
		Write1302(DS1302_HOUR,0x00);		//设置成24小时制
		Write1302(DS1302_SECOND,0x00);		//使能时钟运行
		Write1302(DS1302_WRITE,0x80);		//打开写保护
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

