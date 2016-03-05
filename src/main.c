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
#include "main.h"

void Init_RfidUpan_GPIO(void);

void Time_Conv(uint8_t * tt,unsigned char cnt,char * timestr);  //日期时间

void Init_LED(void);
void Delay_Ms(uint16_t time);
void Delay_Us(uint16_t time);

unsigned char CheckSum(unsigned char *dat, unsigned char num);
void SendCommand(void);

extern UART_HandleTypeDef Console_Handle;
extern UART_HandleTypeDef RFIDUART_Handle;
extern USART_HandleTypeDef USART3_Handle;

// 全局变量定义区
/*static unsigned char rfid1=0;*/
/*static unsigned char rfid2=0;*/
/*static unsigned char door_state=0;*/
/*static unsigned char door_closed=0;*/
uint8_t door_state = 0;

// 文件操作变量区域
/*unsigned char namenum=8;*/
char SDPath[4];
FATFS fs;

const char *userfilename = "0:/cardlist.txt"; //职工txt
const char *upanfilename = "0:/upanlist.txt"; //职工txt

/*extern unsigned char indarray[20];*/
/*extern unsigned char lenind;*/

void SystemClock_Config(void);

void HAL_MspInit()
{
    __HAL_RCC_AFIO_CLK_ENABLE();
}

osThreadId rfid_ThreadID;
osThreadId udisk_ThreadID;
osThreadId lcdupdate_ThreadID;

xSemaphoreHandle sem_cmd;

int main(void)
{
    //unsigned char serialarraycheck[10]="1119241448";

    /////////////////////////////////////////////////////////////////////////////////////////////////
    HAL_Init();
    SystemClock_Config();
    Init_LED();      //各个外设引脚配置

    DebugLogConsoleConfig(USART1);
    RFIDUSARTConfig(USART2);
    LCDUSARTConfig(USART3);
    Delay_Ms(200);        //等待200ms确保屏幕启动

    Init_LCDSTRUCT();
    /*LCDUpdate('a');         //LCD显示*/
    //LCDUpdate('n');
    printf("hello world\r\n\t");
    /*HAL_UART_Transmit(&Console_Handle, (uint8_t*)hello, 13, 5000);*/

    InitClock();            //配置DS1302

    Init_RfidUpan_GPIO();

    /* ENC28J60 SPI 接口初始化 */
    /*SPI_Enc28j60_Init();//函数初始化*/
    /*SetIpMac();*/ //TODO

    /* 挂载文件系统*/
    if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
    {
        /*##-2- Register the file system object to the FatFs module ##############*/
        if(f_mount(&fs, (TCHAR const*)SDPath, 0) != FR_OK)
        {
            /* FatFs Initialization Error */
            HardFault_Handler();
        }
    }

    LCDShowUpanState(upanfilename);

    vSemaphoreCreateBinary(sem_cmd);

    osThreadDef(MONITOR_RFID, monitorRFID, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    osThreadDef(MONITOR_UDISK, monitorUdisk, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
    osThreadDef(UPDATE_LCD, LCDUpdateThread, osPriorityLow, 0, configMINIMAL_STACK_SIZE);

    rfid_ThreadID = osThreadCreate(osThread(MONITOR_RFID), NULL);
    udisk_ThreadID = osThreadCreate(osThread(MONITOR_UDISK), NULL);
    lcdupdate_ThreadID = osThreadCreate(osThread(UPDATE_LCD), NULL);

    osKernelStart();

    while(1);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef clkinitstruct = {0};
    RCC_OscInitTypeDef oscinitstruct = {0};

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
    oscinitstruct.HSEState        = RCC_HSE_ON;
    oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
    oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
    oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
    oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
    {
        /* Initialization Error */
        while(1);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
    clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
    {
        /* Initialization Error */
        while(1);
    }
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Time_Conv
 ** 功能描述: LED IO引脚配置
 ** 参数描述：无
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Time_Conv(uint8_t * tt,unsigned char cnt,char * timestr)
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

    /*RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG,ENABLE); //使能各个端口时钟，重要！！！*/
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_14;      //配置LED端口挂接到6、12、13端口
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;     //通用输出推挽
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     //配置端口速度为50M
    //根据参数初始化GPIOD寄存器
    //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE,ENABLE); //使能各个端口时钟，重要！！！
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

    //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD,ENABLE); //使能各个端口时钟，重要！！！
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_PIN_13;          //配置LED端口挂接到6、12、13端口
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;     //通用输出推挽
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;     //配置端口速度为50M
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);        //根据参数初始化GPIOD寄存器
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
        for(j=0;j<10260;j++)
            __NOP();
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
        for(j=0;j<9;j++)
            __NOP();
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

void Init_RfidUpan_GPIO()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable the GPIO Clock */
    /*RCC_APB2PeriphClockCmd(MF522_RST_CLK, ENABLE);*/
    __HAL_RCC_MF522_PORT_CLK_ENABLE();
    /* Configure the GPIO pin */
    GPIO_InitStructure.Pin = MF522_RST_PIN| MF522_MOSI_PIN| MF522_SCK_PIN| MF522_NSS_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;

    HAL_GPIO_Init(MF522_PORT, &GPIO_InitStructure);

    /* Enable the GPIO Clock */
    /*RCC_APB2PeriphClockCmd(MF522_MISO_CLK, ENABLE);*/

    /* Configure the GPIO pin */
    GPIO_InitStructure.Pin = MF522_MISO_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;

    HAL_GPIO_Init(MF522_MISO_PORT, &GPIO_InitStructure);
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
    unsigned char cmd_status = 0, i;

    for(i = 0; i < num; i ++)
    {
        cmd_status ^= dat[i];
    }
    return cmd_status;
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
    USART_SendByte( &RFIDUART_Handle, 0x7F);
}

 //FIXME
void RFIDUART_IRQHandler(void)
{
    USART_HandleTypeDef * husart = (USART_HandleTypeDef*) &RFIDUART_Handle; //TODO: movo out of handler
    uint32_t tmp_flag = 0, tmp_it_source = 0;

    static __IO uint8_t bTemp, flag=0;
    static __IO uint8_t sflag = 0;

    /* 串口接收 */
    /*if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)*/
    tmp_flag = __HAL_USART_GET_FLAG(husart, USART_FLAG_RXNE);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_RXNE);
    if((tmp_flag != RESET) && (tmp_it_source != RESET))
    {
        /* 清串口中断标记 */
        /*bTemp = USART_ReceiveData(USART2);*/
        __IO uint32_t t = husart->Instance->DR;
        bTemp = (uint8_t)(t & (uint32_t)0x00FF); //TODO
        /* 上条命令处理完成才接收 */
        if(Cmd.ReceiveFlag == 0)
        {
            if(xSemaphoreTake(sem_cmd, 0) == pdFALSE)
                return;
            /* 7F 标记，为 0 表示上一个数据不是7F */
            if(flag == 0)
            {
                /* 上一个标记不是7F，这个是，打上标记 */
                if(bTemp == 0x7F)
                    flag = 1;
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
            if(Cmd.ReceivePoint >= 32)
            {
                Cmd.ReceivePoint = 0;
            }
            /* 接收指针大于 2 个开始对比命令长度和接收指针，一致表示收到完整命令 */
            if(Cmd.ReceivePoint > 2)
            {
                if(Cmd.ReceivePoint == Cmd.ReceiveBuffer[0]+1)
                {
                    Cmd.ReceiveFlag = 1;
                    xSemaphoreGiveFromISR(sem_cmd, pdFALSE);
                }
            }
        }
    }
    /* 发送 */
    /*if(USART_GetFlagStatus(USART2, USART_FLAG_TC) == SET)*/
    tmp_flag = __HAL_USART_GET_FLAG(husart, USART_FLAG_TC);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_TC);
    if((tmp_flag != RESET) && (tmp_it_source != RESET))
    {
        /*USART_ClearFlag(USART2, USART_FLAG_TC);*/
        __HAL_USART_CLEAR_FLAG(husart, USART_FLAG_TC);
        /* 发送指针不为0时继续发送 */
        if(Cmd.SendPoint != 0)
        {
            /* 7F判断 */
            if(sflag == 0)
            {
                Cmd.SendPoint--;
                /*USART_SendData(USART2, Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint]);*/
                WRITE_REG(husart->Instance->DR, Cmd.SendBuffer[Cmd.SendBuffer[0]-Cmd.SendPoint] & (uint8_t) 0xFF);
                if(Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint] == 0x7F)
                {
                    sflag = 1;
                }
            }
            else
            {
                sflag = 0;
                /*USART_SendData(USART2, 0x7F);*/
                WRITE_REG(husart->Instance->DR, 0x7F);
            }
        }
        /* 发送指针为0时打上发送标记表示发送完成 */
        else
        {
            Cmd.SendFlag = 0;
        }
    }
}

