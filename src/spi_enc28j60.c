/******************** (C) COPYRIGHT 2011 迷你嵌入式开发工作室 ********************
 * 文件名  ：spi.c
 * 描述    ：ENC28J60(以太网芯片) SPI接口应用函数库
 *
 * 实验平台：野火STM32开发板
 * 硬件连接： ------------------------------------
 *           |PB13         ：ENC28J60-INT (没用到)|
 *           |PA6-SPI1-MISO：ENC28J60-SO          |
 *           |PA7-SPI1-MOSI：ENC28J60-SI          |
 *           |PA5-SPI1-SCK ：ENC28J60-SCK         |
 *           |PA4-SPI1-NSS ：ENC28J60-CS          |
 *           |PE1          ：ENC28J60-RST (没用)  |
 *            ------------------------------------
 * 库版本  ：ST3.0.0
 * 作者    ：fire  QQ: 313303034
 * 博客    ：firestm32.blog.chinaunix.net
**********************************************************************************/
#include "spi_enc28j60.h"
#include "stm32f1xx_hal_gpio.h"
/*#include "stm32f1xx_hal_spi.h"*/


static SPI_HandleTypeDef SPI_HandleStructure;

/*
 * 函数名：SPI1_Init
 * 描述  ：ENC28J60 SPI 接口初始化
 * 输入  ：无
 * 输出  ：无
 * 返回  ：无
 */
void SPI_Enc28j60_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 SPI1 时钟 */
    /*RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);*/
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    /* ---------enc28j60 通信I/O初始化----------------
     * PA5-SPI1-SCK :ENC28J60_SCK
     * PA6-SPI1-MISO:ENC28J60_SO
     * PA7-SPI1-MOSI:ENC28J60_SI
     */

    GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;		   // 复用输出
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ---------enc28j60 控制I/O初始化----------------*/
    /* PA4-SPI1-NSS:ENC28J60_CS */ 											 // 片选
    GPIO_InitStructure.Pin = GPIO_PIN_4;
    GPIO_InitStructure.Pin = GPIO_PIN_4;
    GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	   // 推免输出
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

    /* PB13:ENC28J60_INT */						// 中断引脚没用到

    /* PE1:ENC28J60_RST*/				      // 复位似乎不用也可以

    /* SPI1 配置 */
    SPI_HandleStructure.Instance = SPI1;
    SPI_HandleStructure.Init.Direction = SPI_DIRECTION_2LINES;
    SPI_HandleStructure.Init.Mode = SPI_MODE_MASTER;
    SPI_HandleStructure.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_HandleStructure.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_HandleStructure.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_HandleStructure.Init.NSS = SPI_NSS_SOFT;
    SPI_HandleStructure.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    SPI_HandleStructure.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_HandleStructure.Init.CRCPolynomial = 7;

    HAL_SPI_Init(&SPI_HandleStructure);

    /* 使能 SPI1  */ /*SPI_Cmd(SPI1, ENABLE);*/
    __HAL_SPI_ENABLE(&SPI_HandleStructure);
  /*switch(HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)aTxBuffer, (uint8_t *)aRxBuffer, BUFFERSIZE, 5000))*/
}

/*
 * 函数名：SPI1_ReadWrite
 * 描述  ：SPI1读写一字节数据
 * 输入  ：
 * 输出  ：
 * 返回  ：
 */
unsigned char SPI1_ReadWrite(unsigned char writedat)
{
    uint8_t readdat;
    HAL_SPI_TransmitReceive(&SPI_HandleStructure, &writedat, &readdat, 1, 5000);
    return readdat;
}

