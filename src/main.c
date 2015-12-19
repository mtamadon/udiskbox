/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：MicroSD卡(SDIO模式)测试实验，并将测试信息通过串口1在电脑的超级终端上
 *           打印出来
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
 *********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "system_stm32f1xx.h"

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "usually.h"
#include "usart.h"
#include "PICC.h"
#include "string.h"
#include "stdio.h"
#include "STDLIB.H"
#include "ds1302.h"
#include "rfidupan.h"
#include "inttypes.h"

#include "spi_enc28j60.h"
#include "web_server.h"
#include "net.h"


//变量定义

//显示结构体
typedef struct _DISSTRUCT
{
    char Name[32];
    char UserID[32];
    char UdiskInfo2[32];
    char PhoneNum[32];
    char UdiskInfo[32];
    char UdiskState[32];
    char Temperature[32];
    char TimeNow[32];

}DISSTRUCT ;

DISSTRUCT LCDSTRUCT;

//函数申明

void LCDUpdate(char stype);    //LCD显示
void Init_LCDSTRUCT(void);

void Time_Conv(uint8_t * tt,unsigned char cnt,char * timestr);  //日期时间
char PcdRequest(unsigned char req_code,unsigned char *pTagType);
void ncs(unsigned char cse);

void Init_LED(void);
void Init_NVIC(void);
void Delay_Ms(uint16_t time);
void Delay_Us(uint16_t time);

void Init_RfidUpan_GPIO(void);
void Init_RFID(void);  //初始化RFID
unsigned char CheckSum(unsigned char *dat, unsigned char num);
void SendCommand(void);
void LongToStr( char *array, unsigned long number, unsigned char count);

void uchar2str(unsigned char i,unsigned char * strname);
int LCDShowUpanState(unsigned char * filename);

int checkserial(unsigned char *filename,unsigned char *serialarraycheck,unsigned char * namearray);
int writelog(unsigned char * filename,unsigned char * filecontent,unsigned char lencont,unsigned char sflag);
int printallfile(unsigned char * filename );

extern unsigned  int fill_tcp_data_p(unsigned char *buf,unsigned  int pos, const unsigned char *progmem_s);
extern void SendTcp(unsigned int plen);
// 全局变量定义区
uint8_t bTemp;
static unsigned char rfid1=0;
static unsigned char rfid2=0;
static unsigned char door_state=0;
static unsigned char door_closed=0;
int res;
int a;
long upanNum[2]={157384350,2306834590};
static unsigned char upanState=0x00;

// 文件操作变量区域
unsigned char serialnum=10,namenum=8,strall=22; //@xxx(RFID号码10位)+空格(1位)+xxx(名称前面补空格)+0A0D(换行符)
unsigned char upanstrall=32; //@xxx(RFID号码10位)+空格(1位)+xxx(5位upan名称)+空格(1位)+xxx(借U盘人的RFID号码10位)+0A0D(换行符)
char SDPath[4];
FIL fdrd,fdwr;
FATFS fs;
UINT br, bw;            // File R/W count
BYTE buffer;       // file copy buffer
BYTE filetemp[40]="";

unsigned char userfilename[20]="0:/cardlist.txt"; //职工txt
unsigned char upanfilename[20]="0:/upanlist.txt"; //职工txt

uint8_t key_time = 0;
unsigned char count;

extern  unsigned char buf[1501];

extern unsigned char indarray[20];
extern unsigned char lenind;
unsigned char gflag_send=0;
DWORD send_count=0;

int main(void)
{

    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     ** 变量定义
     :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

    uint8_t tt[7];
    char rfid_status;
    unsigned char g_ucTempbuf[20];

    //unsigned char serialarraycheck[10]="1119241448";
    unsigned char namearray[20]="";
    int stread,stwrite=0;
    unsigned char strtmp[30]="";



    /////////////////////////////////////////////////////////////////////////////////////////////////
    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     ** 初始化区域
     :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    HAL_Init();
    /*Init_NVIC();     //中断向量表注册函数*/ //TODO: USART2 中断向量注册
    NVIC_Configuration(); //SDIO中断处理初始化
    Init_LED();      //各个外设引脚配置
    Init_Usart();     //串口引脚配置
    Usart_Configuration(USART1,115200); //串口1配置 设置波特率为115200
    Usart_Configuration(USART2,9600); //串口2配置 设置波特率为115200
    Usart_Configuration(USART3,115200); //串口3配置 设置波特率为115200
    Delay_Ms(200);        //等待200ms确保屏幕启动

    Init_LCDSTRUCT();
    // LCDUpdate('a');         //LCD显示
    //LCDUpdate('n');
    printf("start..\n\t");

    Cmd.SendFlag = 0;       //初始化RFID标志位
    Cmd.ReceiveFlag = 0;
    Picc.Value = 0;

    InitClock();            //配置DS1302

    Init_RfidUpan_GPIO();

    /* ENC28J60 SPI 接口初始化 */
    SPI_Enc28j60_Init();//函数初始化
    SetIpMac();
    gflag_send=0;       //变量初始化
    send_count=0;
    /* 挂载文件系统*/

    FATFS_LinkDriver(&SD_Driver, SDPath); //TODO: Error Handle
    f_mount(&fs, (TCHAR const*)SDPath, 0); //TODO: Error Handle

    LCDShowUpanState(upanfilename);

    /*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     ** 循环区域
     :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
    while(1)
    {
        ReadDS1302Clock(tt);
        Time_Conv(tt,6,LCDSTRUCT.TimeNow);
        ////////////////////////////////////////////////////////////////////////////////////      检测开箱
        Web_Server();   //网络模块开始工作

        //////////////////////////////////////////////////////////////////////
        bTemp = CommandProcess();
        if(bTemp == 0)
        {
            //在这里开始你的操作
            //所有有用数据在 Picc
            //卡号 ==>Picc.UID
            //卡类型==>Picc.Type
            //余额 ==>Picc.Value
            if(1)//door_state==0)
            {
                LongToStr(LCDSTRUCT.UserID,Picc.UID,10);
                //LCDUpdate('a');
                stread=checkserial(userfilename,LCDSTRUCT.UserID,namearray);
                if(stread==-1)
                    strcpy(strtmp,"OPEN FILE ERROR");
                else if(stread==0)
                {
                    //strcpy(strtmp,"NOT FIND");
                    door_closed=1;
                }
                else if(stread>0)    //比较是否是合法卡
                {
                    door_state=1;    //开门
                    strcpy(strtmp,namearray);//显示姓名，提示关门
                    strcpy(LCDSTRUCT.Name,strtmp);
                    LCDUpdate('a');
                }
                //strcpy(LCDSTRUCT.Name,strtmp);
                //LCDUpdate('a');
            }
            //door_state=1;
        }
        else if(bTemp == 0xFF)
        {
            //无卡
        }
        else if(bTemp == 0xFE)
        {
            LCDUpdate('e');
        }
        else if(bTemp == 0xFD)
        {
            //参数错误
        }

        ///////////////////////////////////////////////////////////////////////////////////////    检测关门
        if((door_state==1)&&(door_closed==1))
        {
            door_state++;
            rfid1=0;
            rfid2=0;
            //door_closed=0;
            //LCDUpdate('a');
        }
        ///////////////////////////////////////////////////////////////////////////////////////     检测u盘
        if(door_state!=2){door_closed=0;continue;}

        upanState=0x00;
        /////////////////////////////////////////////////////////////////////////////////////////   RFID2
        if(rfid2==0)
        {
            ncs(2);//片选
            Init_RfidUpan();
            rfid_status = PcdRequest(PICC_REQALL,g_ucTempbuf);//扫描卡
            printf("%02x  ",rfid_status);
            if(rfid_status==0)
            {
                rfid_status = PcdAnticoll(g_ucTempbuf);//防冲撞
                if(rfid_status==0)
                {
                    Picc.UID = g_ucTempbuf[0];
                    Picc.UID <<= 8;
                    Picc.UID |= g_ucTempbuf[1];
                    Picc.UID <<= 8;
                    Picc.UID |= g_ucTempbuf[2];
                    Picc.UID <<= 8;
                    Picc.UID |= g_ucTempbuf[3];
                    if((Picc.UID-upanNum[0])==0)
                    {
                        upanState=upanState|0x01;
                        LongToStr(LCDSTRUCT.UdiskInfo,Picc.UID,10);strcat(LCDSTRUCT.UdiskInfo," (1)");
                    }
                    else if((Picc.UID-upanNum[1])==0)
                    {
                        upanState=upanState|0x02;
                        LongToStr(LCDSTRUCT.UdiskInfo2,Picc.UID,10);strcat(LCDSTRUCT.UdiskInfo2," (2)");
                    }
                    rfid2=1;
                }
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////      RFID1
        if(rfid1==0)
        {
            ncs(1);
            Init_RfidUpan();
            rfid_status = PcdRequest(PICC_REQALL,g_ucTempbuf);//扫描卡
            if(rfid_status==0)
            {
                rfid_status = PcdAnticoll(g_ucTempbuf);//防冲撞
                if(rfid_status==0)
                {
                    Picc.UID = g_ucTempbuf[0];
                    Picc.UID <<= 8;
                    Picc.UID |= g_ucTempbuf[1];
                    Picc.UID <<= 8;
                    Picc.UID |= g_ucTempbuf[2];
                    Picc.UID <<= 8;
                    Picc.UID |= g_ucTempbuf[3];
                    if(Picc.UID-upanNum[0]==0)
                    {
                        upanState=upanState|0x01;
                        LongToStr(LCDSTRUCT.UdiskInfo,Picc.UID,10);strcat(LCDSTRUCT.UdiskInfo," (1)");
                    }
                    else if(Picc.UID-upanNum[1]==0)
                    {
                        upanState=upanState|0x02;
                        LongToStr(LCDSTRUCT.UdiskInfo2,Picc.UID,10);strcat(LCDSTRUCT.UdiskInfo2," (2)");
                    }
                    rfid1=1;
                }
            }
        }
        door_state=0;
        ///////////////////////////////////////////////////////////////////////////////////     显示状态
        if((upanState&0x01)==0)
        {
            strcpy(LCDSTRUCT.UdiskInfo,strtmp);strcat(LCDSTRUCT.UdiskInfo," (1)");
        }
        if((upanState&0x02)==0)
        {
            strcpy(LCDSTRUCT.UdiskInfo2,strtmp);strcat(LCDSTRUCT.UdiskInfo2," (2)");
        }
        LCDUpdate('a');




    }


}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Time_Conv
 ** 功能描述: LED IO引脚配置
 ** 参数描述：无
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Time_Conv(u8 * tt,unsigned char cnt,char * timestr)
{

    timestr[0]='2';                //年
    timestr[1]='0';
    timestr[2]=(tt[0]>>4) + 0x30;
    timestr[3]=(tt[0]&0x0f)+0x30;
    timestr[4]='-';

    timestr[5]=(tt[1]>>4) + 0x30;  //月
    timestr[6]=(tt[1]&0x0f)+0x30;
    timestr[7]='-';

    timestr[8]=(tt[2]>>4) + 0x30;  //日
    timestr[9]=(tt[2]&0x0f)+0x30;
    timestr[10]=' ';

    timestr[11]=(tt[3]>>4) + 0x30;  //时
    timestr[12]=(tt[3]&0x0f)+0x30;
    timestr[13]=':';

    timestr[14]=(tt[4]>>4) + 0x30;  //分
    timestr[15]=(tt[4]&0x0f)+0x30;
    timestr[16]='\0';




}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Init_LED
 ** 功能描述: LED IO引脚配置
 ** 参数描述：无
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_LED()
{
    GPIO_InitTypeDef GPIO_InitStructure;     //定义一个GPIO结构体变量

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG,ENABLE); //使能各个端口时钟，重要！！！

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_14;      //配置LED端口挂接到6、12、13端口
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_14;          //配置LED端口挂接到6、12、13端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //通用输出推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //配置端口速度为50M
    GPIO_Init(GPIOG, &GPIO_InitStructure);        //根据参数初始化GPIOD寄存器RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE,ENABLE); //使能各个端口时钟，重要！！！

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD,ENABLE); //使能各个端口时钟，重要！！！
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13;          //配置LED端口挂接到6、12、13端口
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_13;          //配置LED端口挂接到6、12、13端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //通用输出推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //配置端口速度为50M
    GPIO_Init(GPIOD, &GPIO_InitStructure);        //根据参数初始化GPIOD寄存器


}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Init_NVIC
 ** 功能描述: 系统中断配置
 ** 参数描述：无
 ** 作  　者: Dream
 ** 日　  期: 2011年5月14日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_NVIC(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;   //定义一个NVIC向量表结构体变量

#ifdef  VECT_TAB_RAM         //向量表基地址选择

    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);   //将0x20000000地址作为向量表基地址(RAM)
#else

    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //将0x08000000地址作为向量表基地址(FLASH)
#endif

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组 为2

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   //配置串口2为中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //设置占先优先级为2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;     //设置副优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;      //使能串口1中断
    NVIC_Init(&NVIC_InitStructure);          //根据参数初始化中断寄存器
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Delay_Ms_Ms
 ** 功能描述: 延时1MS (可通过仿真来判断他的准确度)
 ** 参数描述：time (ms) 注意time<65535
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Ms(uint16_t time)  //延时函数
{
    uint16_t i,j;
    for(i=0;i<time;i++)
        for(j=0;j<10260;j++);
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Delay_Ms_Us
 ** 功能描述: 延时1us (可通过仿真来判断他的准确度)
 ** 参数描述：time (us) 注意time<65535
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Us(uint16_t time)  //延时函数
{
    uint16_t i,j;
    for(i=0;i<time;i++)
        for(j=0;j<9;j++);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Init_RFID
 ** 功能描述: 初始化RFID
 ** 参数描述：
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_RFID()  //初始化RFID
{
    ;
}

/*******************************************************************************
 * 函数名         : CheckSum
 * 描述           : 命令校验。
 * 输入           :  dat : 要校验的数组。
num : 校验的字节数。
 * 输出           : 无
 * 返回           : 校验值。
 *******************************************************************************/
unsigned char CheckSum(unsigned char *dat, unsigned char num)
{
    unsigned char bTemp = 0, i;

    for(i = 0; i < num; i ++){bTemp ^= dat[i];}
    return bTemp;
}

/*******************************************************************************
 * 函数名         : SendCommand
 * 描述           : 命令开始发送函数。
 * 输入           : 无
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void SendCommand(void)
{
    while(Cmd.SendFlag != 0);
    Cmd.SendFlag = 1;
    Cmd.SendBuffer[Cmd.SendBuffer[0]] = CheckSum(Cmd.SendBuffer, Cmd.SendBuffer[0]);
    Cmd.SendPoint = Cmd.SendBuffer[0] + 1;
    USART_SendData(USART2, 0x7F);
}

/*******************************************************************************
 * 函数名         : LCDNumber
 * 描述           : 往 12864 写数字
 * 输入           : address: 地址
 number : 数字
 count  : 数字显示的长度
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void LongToStr( char *array, unsigned long number, unsigned char count)
{
    // unsigned char array[11];
    unsigned char i;

    array[count] = 0;
    for(i = count; i > 0; i --){array[i-1] = number % 10+'0';number /= 10;}
    for(i = 0; i < count-1; i ++){if(array[i]=='0'){array[i] = ' ';}else{break;}}
}

/*******************************************************************************
 * 函数名         : Init_LCDSTRUCT
 * 描述           : 初始化LCDSTRUCT结构体,全部赋值空
 * 输入           : 空
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void Init_LCDSTRUCT()
{
    strcpy(LCDSTRUCT.Name,"");
    strcpy(LCDSTRUCT.UserID,"");
    strcpy(LCDSTRUCT.PhoneNum,"");
    strcpy(LCDSTRUCT.UdiskInfo,"");
    strcpy(LCDSTRUCT.UdiskState,"");
    strcpy(LCDSTRUCT.Temperature,"");
    strcpy(LCDSTRUCT.TimeNow,"");

}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: LCDUpdate
 ** 功能描述: 刷新LCD
 ** 参数描述：无
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void LCDUpdate(char stype)
{
    char Name_Array[64];
    char UserID_Array[64];
    char UdiskInfo_Array2[64];
    char PhoneNum_Array[64];
    char UdiskInfo_Array[64];
    char UdiskState_Array[64];
    char Temperature_Array[64];
    char TimeNow_Array[64];

    switch(stype)
    {

        case 'a':
            {

                strcpy(Name_Array,"DS16(100,24,'");strcat(Name_Array,LCDSTRUCT.Name);strcat(Name_Array,"',4);");
                strcpy(UserID_Array,"DS16(100,48,'");strcat(UserID_Array,LCDSTRUCT.UserID);strcat(UserID_Array,"',4);");
                strcpy(PhoneNum_Array,"DS16(100,72,'");strcat(PhoneNum_Array,LCDSTRUCT.PhoneNum);strcat(PhoneNum_Array,"',4);");
                strcpy(UdiskInfo_Array,"DS16(80,96,'");strcat(UdiskInfo_Array,LCDSTRUCT.UdiskInfo);strcat(UdiskInfo_Array,"',4);");
                strcpy(UdiskInfo_Array2,"DS16(200,96,'");strcat(UdiskInfo_Array2,LCDSTRUCT.UdiskInfo2);strcat(UdiskInfo_Array2,"',4);");
                strcpy(UdiskState_Array,"DS16(100,120,'");strcat(UdiskState_Array,LCDSTRUCT.UdiskState);strcat(UdiskState_Array,"',4);");
                strcpy(Temperature_Array,"DS16(100,144,'");strcat(Temperature_Array,LCDSTRUCT.Temperature);strcat(Temperature_Array,"',4);");
                strcpy(TimeNow_Array,"DS16(100,168,'");strcat(TimeNow_Array,LCDSTRUCT.TimeNow);strcat(TimeNow_Array,"',4);");

                Lcd_Display("DR2;CLS(0);SPG(1);\r\n");
                Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");
                Lcd_Display("DS16(4,24,'姓名：',15);");      Lcd_Display(Name_Array);
                Lcd_Display("DS16(4,48,'卡号：',4);");        Lcd_Display(UserID_Array);
                Lcd_Display("DS16(4,72,'联系电话：',15);");  Lcd_Display(PhoneNum_Array);
                Lcd_Display("DS16(4,96,'U盘信息：',4);");    Lcd_Display(UdiskInfo_Array);Lcd_Display(UdiskInfo_Array2);
                Lcd_Display("DS16(4,120,'借/还状态：',4);");   Lcd_Display(UdiskState_Array);
                Lcd_Display("DS16(4,144,'室内温度：',15);");   Lcd_Display(Temperature_Array);
                Lcd_Display("DS16(4,168,'日期/时间：',15);");  Lcd_Display(TimeNow_Array);
            }break;
        case 'n':
            {
                strcpy(Name_Array,"DS16(100,24,'");strcat(Name_Array,LCDSTRUCT.Name);strcat(Name_Array,"',4);");

                Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");
                Lcd_Display(Name_Array);
            }break;
        case 'u':
            {
                strcpy(UserID_Array,"DS16(100,48,'");strcat(UserID_Array,LCDSTRUCT.UserID);strcat(UserID_Array,"',4);");Lcd_Display(UserID_Array);
                LED2=~LED2;
            }break;
        case 'p':
            {
                strcpy(PhoneNum_Array,"DS16(100,72,'");strcat(PhoneNum_Array,LCDSTRUCT.PhoneNum);strcat(PhoneNum_Array,"',4);");Lcd_Display(PhoneNum_Array);
            }break;
        case 'i':
            {
                strcpy(UdiskInfo_Array,"DS16(100,96,'");strcat(UdiskInfo_Array,LCDSTRUCT.UdiskInfo);strcat(UdiskInfo_Array,"',4);");Lcd_Display(UdiskInfo_Array);
            }break;
        case 's':
            {
                strcpy(UdiskState_Array,"DS16(100,120,'");strcat(UdiskState_Array,LCDSTRUCT.UdiskState);strcat(UdiskState_Array,"',4);");Lcd_Display(UdiskState_Array);
            }break;
        case 't':
            {
                strcpy(Temperature_Array,"DS16(100,144,'");strcat(Temperature_Array,LCDSTRUCT.Temperature);strcat(Temperature_Array,"',4);");Lcd_Display(Temperature_Array);
            }break;
        case 'd':
            {
                strcpy(TimeNow_Array,"DS16(100,168,'");strcat(TimeNow_Array,LCDSTRUCT.TimeNow);strcat(TimeNow_Array,"',4);");Lcd_Display(TimeNow_Array);
            }break;
        case 'e':
            {
                Lcd_Display("DR2;CLS(0);SPG(1);\r\n");
                Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");

            }break;
    }




}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: uchar2str
 ** 功能描述: 刷新LCD
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void uchar2str(unsigned char i,unsigned char * strname)
{
    unsigned j,k,m=0;
    k=i/100;
    if(k>0) {strname[m++]=k+0x30;}

    k=(i-k*100)/10;
    if(k>0) {strname[m++]=k+0x30;}

    k=i%10;
    if(k>0) {strname[m++]=k+0x30;}

    strname[m]='\0';



}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: LCDShowUpanState
 ** 功能描述: 刷新LCD
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int LCDShowUpanState(unsigned char * filename)
{
    char space[4]="  ";
    char norm[6]="正常";
    char borrow[6]="借出";
    char strtmp[30]="";
    char Uinfolist[64]="";
    unsigned char namearray[10]="";
    unsigned char serialarray[11]="";
    BYTE upantemp[40];
    char firstline[20]="";

    unsigned char i,j=0;
    unsigned char pos[4]="";


    br=1;
    i=0;j=0;

    Lcd_Display("DR2;CLS(0);SPG(1);\r\n");
    Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");

    res=f_open(&fdrd,filename, FA_OPEN_EXISTING | FA_READ);
    if(res)
    {
        printf("not open");
        return -1;
    }
    for(;;)
    {
        res = f_read( &fdrd, filetemp, upanstrall, &br );
        if (res||br<upanstrall) break;
        else
        {
            if (filetemp[18]=='0')
            {
                memcpy(strtmp,filetemp+12,5);
                memcpy(strtmp+5,space,2);
                memcpy(strtmp+7,norm,4);
                strtmp[11]='\0';
                printf(strtmp);printf("\r\n");

            }
            else if(filetemp[18]=='1')
            {
                memcpy(upantemp,filetemp,40);
                memcpy(serialarray,filetemp+20,10);serialarray[10]='\0';

                checkserial(userfilename,serialarray,namearray);

                memcpy(strtmp,upantemp+12,5);
                memcpy(strtmp+5,space,2);
                memcpy(strtmp+7,borrow,4);
                memcpy(strtmp+11,space,2);
                strtmp[13]='\0';
                strcat(strtmp,namearray);
                printf(strtmp);printf("\r\n");

            }
            i++;
            j=i*24;
            uchar2str(j,pos);
            strcpy(Uinfolist,"DS16(100,");printf(Uinfolist);
            strcat(Uinfolist,pos);printf(Uinfolist);
            strcat(Uinfolist,",'");printf(Uinfolist);
            strcat(Uinfolist,strtmp);printf(Uinfolist);
            strcat(Uinfolist,"',16);");printf(Uinfolist);printf("\r\n");

            strcpy(firstline,"DS16(4,");
            strcat(firstline,pos);
            strcat(firstline,",'姓名：',16);");
            printf(firstline);printf("\r\n");
            Lcd_Display(firstline);
            Lcd_Display(Uinfolist);

        }
    }
    f_close(&fdrd);
    return 0;
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: checkserial
 ** 功能描述: 通过rfid读出对应的名字
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2015年7月19日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int checkserial(unsigned char * filename,unsigned char * serialarraycheck,unsigned char * namearray)
{

    unsigned char namelen=0;
    unsigned char serialarray[10]="";

    unsigned int j,k,m=0;
    unsigned char check_flag=0;

    br=1;



    res=f_open(&fdrd,filename, FA_OPEN_EXISTING | FA_READ);
    printf("3 ");
    if(res)
    {
        printf("not open");
        return -1;
    }
    printf("4 ");
    for(;;)
    {
        res = f_read( &fdrd, filetemp, strall, &br );
        printf("%02x ",res);
        if (res||br==0) break;
        else
        {
            if (filetemp[0]=='@')
            {
                for(j=1;j<=serialnum;j++)
                {
                    serialarray[j-1]=filetemp[j];
                    check_flag=serialarray[j-1]-serialarraycheck[j-1];
                    if (check_flag) break;
                }
                if (!check_flag)
                {
                    for (k=j;k<strall;k++)
                    {
                        if ((filetemp[k]!=' ')&&(filetemp[k]!=0x0a) &&(filetemp[k]!=0x0d) )
                        {
                            namearray[m++]=filetemp[k];
                            namelen++;

                        }
                    }
                    namearray[m]='\0';
                    return namelen;
                }
            }
        }
    }
    printf("5 ");
    f_close(&fdrd);
    return 0;

}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: writelog
 ** 功能描述: 写入log日志文件
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2015年7月19日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int writelog(unsigned char * filename,unsigned char * filecontent,unsigned char lencont,unsigned char sflag )
{

    bw=1  ;

    if(sflag==0)
    {
        res = f_open(&fdwr,filename,FA_CREATE_ALWAYS| FA_WRITE);
        if(res)
        {
            printf("not open");
            return -1 ;
        }
    }
    else if(sflag==1)
    {
        res = f_open(&fdwr,filename,FA_OPEN_ALWAYS| FA_WRITE);
        if(res)
        {
            printf("not open");
            return -1 ;
        }
        f_lseek(&fdwr,fdwr.fsize);
    }

    res = f_write(&fdwr, filecontent, lencont, &bw);
    if(res)
        printf("write error!\n");
    f_close(&fdwr);

    return 0;


}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: writelog
 ** 功能描述: 写入log日志文件
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2015年7月19日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int printallfile(unsigned char * filename )
{
    unsigned int plen;
    br=1;

    res=f_open(&fdrd,filename, FA_OPEN_EXISTING | FA_READ);
    if(res)
    {
        printf("not open");
        return -1;
    }

    if(gflag_send==0)
    {
        res =f_read( &fdrd, filetemp, strall, &br );
        if (res||br==0)
        {
            gflag_send=2;
            return -2;
        }
        else
        {
            if(br<strall)
            {
                filetemp[br]='\0';
                gflag_send=2;
            }
            else
            {
                filetemp[strall]='\0';
                gflag_send=1;
            }
            send_count+=strall;
            plen=fill_tcp_data_p(buf,0,filetemp);
            SendTcp(plen);
            printf("%d\r\n",gflag_send);
            printf(filetemp);printf("\r\n");

        }
    }
    else if(gflag_send==1)
    {
        f_lseek(&fdrd,send_count);
        res =f_read( &fdrd, filetemp, strall, &br );
        if (res||br==0)
        {
            gflag_send=2;
            return -2;
        }
        else
        {
            if(br<strall)
            {
                filetemp[br]='\0';
                gflag_send=2;
            }
            else
            {
                filetemp[strall]='\0';
                gflag_send=1;
            }
            send_count+=strall;
            plen=fill_tcp_data_p(buf,0,filetemp);
            SendTcp(plen);
            printf("%d\r\n",gflag_send);
            printf(filetemp);printf("\r\n");

        }
    }
    else if(gflag_send==2)
    {
        plen=fill_tcp_data_p(buf,0,"lend");
        SendTcp(plen);
        printf("%d\r\n",gflag_send);
        //printf(filetemp);

    }


    f_close(&fdrd);
    return 0;


}



/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

