/******************** (C) COPYRIGHT 2013 ********************
 * 文件名          : PICC.c
 * 作者            : 优灵电子开发团队。
 * 版本            : V2.0.1.0
 * 日期            : 2013-5-10
 * 描述            : 所有对卡的操作，给模块发送的命令。
 * 技术支持        : QQ: 526181679
 ********************************************************************************/

/* 包含文件 -------------------------------------------------------------------*/
#include "PICC.h"
/* 宏定义 ---------------------------------------------------------------------*/
/* 本文件使用的变量 -----------------------------------------------------------*/
PICC Picc;
CMD Cmd;
/* 本文件使用的函数声明 -------------------------------------------------------*/
void SendCommand(void);
/* 本文件函数体 ---------------------------------------------------------------*/

/*******************************************************************************
 * 函数名         : PICCHalt
 * 描述           : 停止卡命令。
 * 输入           : 无
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCHalt(void)
{
    Cmd.SendBuffer[0] =  2;
    Cmd.SendBuffer[1] = 0x01;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCRequest
 * 描述           : 请求卡。
 * 输入           : mode: 寻卡模式。
 -0x26: 寻未被停止的卡。
 -0x52: 寻所有卡。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCRequest(unsigned char mode)
{
    Cmd.SendBuffer[0] = 3;
    Cmd.SendBuffer[1] = 0x02;
    Cmd.SendBuffer[2] = mode;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCAnticoll
 * 描述           : 防冲撞。
 * 输入           : 无
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCAnticoll(void)
{
    Cmd.SendBuffer[0] = 2;
    Cmd.SendBuffer[1] = 0x03;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCSelect
 * 描述           : 选择卡。
 * 输入           : 无
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCSelect(void)
{
    Cmd.SendBuffer[0] = 2;
    Cmd.SendBuffer[1] = 0x04;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCAuthState
 * 描述           : 验证。
 * 输入           : sector: 要验证的扇区号。
mode:   验证模式。
-0x60:验证密钥 A
-0x61:验证密钥 B
key:    密钥数组。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCAuthState(unsigned char sector, unsigned char mode, unsigned char *key)
{
    unsigned char i;

    Cmd.SendBuffer[0] = 10;
    Cmd.SendBuffer[1] = 0x05;
    Cmd.SendBuffer[2] = sector;
    Cmd.SendBuffer[3] = mode;
    for(i = 0; i < 6; i ++)
    {
        Cmd.SendBuffer[4 + i] = key[i];
    }
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCReadBlockData
 * 描述           : 读块数据。
 * 输入           : block: 将要读的块地址。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCReadBlockData(unsigned char block)
{
    Cmd.SendBuffer[0] = 3;
    Cmd.SendBuffer[1] = 0x06;
    Cmd.SendBuffer[2] = block;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCWriteBlockData
 * 描述           : 写块数据。
 * 输入           : block:     将要写的块地址。
 blockData: 块数据数组。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCWriteBlockData(unsigned char block, unsigned char *blockData)
{
    unsigned char i;

    Cmd.SendBuffer[0] = 19;
    Cmd.SendBuffer[1] = 0x07;
    Cmd.SendBuffer[2] = block;
    for(i = 0; i < 16; i ++)
    {
        Cmd.SendBuffer[3 + i] = blockData[i];
    }
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCInc
 * 描述           : 增值。
 * 输入           : block: 将要操作的块地址。
value: 将要增加的值。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCInc(unsigned char block, unsigned long value)
{
    Cmd.SendBuffer[0] = 7;
    Cmd.SendBuffer[1] = 0x08;
    Cmd.SendBuffer[2] = block;
    Cmd.SendBuffer[3] = (unsigned char)(value >> 24);
    Cmd.SendBuffer[4] = (unsigned char)(value >> 16);
    Cmd.SendBuffer[5] = (unsigned char)(value >> 8);
    Cmd.SendBuffer[6] = (unsigned char)(value);
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCDec
 * 描述           : 减值。
 * 输入           : block: 将要操作的块地址。
value: 将要减的值。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCDec(unsigned char block, unsigned long value)
{
    Cmd.SendBuffer[0] = 7;
    Cmd.SendBuffer[1] = 0x09;
    Cmd.SendBuffer[2] = block;
    Cmd.SendBuffer[3] = (unsigned char)(value >> 24);
    Cmd.SendBuffer[4] = (unsigned char)(value >> 16);
    Cmd.SendBuffer[5] = (unsigned char)(value >> 8);
    Cmd.SendBuffer[6] = (unsigned char)(value);
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCRestore
 * 描述           : 缓冲数据。
 * 输入           : block: 将要操作的块地址。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCRestore(unsigned char block)
{
    Cmd.SendBuffer[0] = 3;
    Cmd.SendBuffer[1] = 0x0A;
    Cmd.SendBuffer[2] = block;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : PICCTansfer
 * 描述           : 存储数据。
 * 输入           : block: 将要操作的块地址。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void PICCTansfer(unsigned char block)
{
    Cmd.SendBuffer[0] = 3;
    Cmd.SendBuffer[1] = 0x0B;
    Cmd.SendBuffer[2] = block;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : OneKeyReadCard
 * 描述           : 一键读卡。
 * 输入           : 无
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void OneKeyReadCard(void)
{
    Cmd.SendBuffer[0] = 2;
    Cmd.SendBuffer[1] = 0x10;
    SendCommand();
}

/*******************************************************************************
 * 函数名         : OneKeyMakeCard
 * 描述           : 一键办卡。
 * 输入           : block: 将要操作的块地址。
value: 将要操作的值。
keyA : 办卡设置的密钥A。
keyB : 办卡设置的密钥B。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void OneKeyMakeCard(unsigned char block, unsigned long value, unsigned char *keyA, unsigned char *keyB)
{
    unsigned char i;

    Cmd.SendBuffer[0] = 19;
    Cmd.SendBuffer[1] = 0x11;
    Cmd.SendBuffer[2] = block;
    Cmd.SendBuffer[3] = (unsigned char)(value >> 24);
    Cmd.SendBuffer[4] = (unsigned char)(value >> 16);
    Cmd.SendBuffer[5] = (unsigned char)(value >> 8);
    Cmd.SendBuffer[6] = (unsigned char)(value);
    for(i = 0; i < 6; i ++)
    {
        Cmd.SendBuffer[7 + i] = keyA[i];
    }
    for(i = 0; i < 6; i ++)
    {
        Cmd.SendBuffer[13 + i] = keyB[i];
    }
    SendCommand();
}

/*******************************************************************************
 * 函数名         : OneKeyInc
 * 描述           : 一键充值。
 * 输入           : block: 将要操作的块地址。
key  : 办卡设置的密钥。
value: 将要操作的值。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void OneKeyInc(unsigned char block, unsigned char *key, unsigned long value)
{
    unsigned char i;

    Cmd.SendBuffer[0] = 13;
    Cmd.SendBuffer[1] = 0x12;
    Cmd.SendBuffer[2] = block;
    for(i = 0; i < 6; i ++)
    {
        Cmd.SendBuffer[3 + i] = key[i];
    }
    Cmd.SendBuffer[9] = (unsigned char)(value >> 24);
    Cmd.SendBuffer[10] = (unsigned char)(value >> 16);
    Cmd.SendBuffer[11] = (unsigned char)(value >> 8);
    Cmd.SendBuffer[12] = (unsigned char)(value);
    SendCommand();
}

/*******************************************************************************
 * 函数名         : OneKeyInc
 * 描述           : 一键扣款。
 * 输入           : block: 将要操作的块地址。
 key  : 办卡设置的密钥。
 value: 将要操作的值。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void OneKeyDec(unsigned char block, unsigned char *key, unsigned long value)
{
    unsigned char i;

    Cmd.SendBuffer[0] = 13;
    Cmd.SendBuffer[1] = 0x13;
    Cmd.SendBuffer[2] = block;
    for(i = 0; i < 6; i ++)
    {
        Cmd.SendBuffer[3 + i] = key[i];
    }
    Cmd.SendBuffer[9] = (unsigned char)(value >> 24);
    Cmd.SendBuffer[10] = (unsigned char)(value >> 16);
    Cmd.SendBuffer[11] = (unsigned char)(value >> 8);
    Cmd.SendBuffer[12] = (unsigned char)(value);
    SendCommand();
}

/*******************************************************************************
 * 函数名         : OneKeyReadBlock
 * 描述           : 一键读块。
 * 输入           : block: 将要操作的块地址。
keyA : 密钥A。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void OneKeyReadBlock(unsigned char block, unsigned char *keyA)
{
    unsigned char i;

    Cmd.SendBuffer[0] = 9;
    Cmd.SendBuffer[1] = 0x13;
    Cmd.SendBuffer[2] = block;
    for(i = 0; i < 6; i ++)
    {
        Cmd.SendBuffer[3 + i] = keyA[i];
    }
    SendCommand();
}

/*******************************************************************************
 * 函数名         : OneKeyWriteBlock
 * 描述           : 一键写块。
 * 输入           : block: 将要操作的块地址。
 keyB : 密钥B。
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void OneKeyWriteBlock(unsigned char block, unsigned char *keyB, unsigned char *blockData)
{
    unsigned char i;

    Cmd.SendBuffer[0] = 25;
    Cmd.SendBuffer[1] = 0x13;
    Cmd.SendBuffer[2] = block;
    for(i = 0; i < 6; i ++)
    {
        Cmd.SendBuffer[3 + i] = keyB[i];
    }
    for(i = 0; i < 16; i ++)
    {
        Cmd.SendBuffer[9 + i] = blockData[i];
    }
    SendCommand();
}

/*******************************************************************************
 * 函数名         : CommandProcess
 * 描述           : 命令处理。
 * 输入           : 无
 * 输出           : 无
 * 返回           : 执行结果。
 -0   : 正确。
 -0xFF: 无卡。
 -0xFE: 卡操作错误。
 -0xFD: 命令错误。
 -0xFC: 没有收到命令。
 *******************************************************************************/
unsigned char CommandProcess(void)
{
    unsigned char i;

    if(Cmd.ReceiveFlag)
    {
        switch(Cmd.ReceiveBuffer[1])
        {
            case 0x01:
                //noting to do
                break;
            case 0x02:
                Picc.Type = Cmd.ReceiveBuffer[3];
                Picc.Type <<= 8;
                Picc.Type |= Cmd.ReceiveBuffer[4];
                break;
            case 0x03:
                Picc.UID = Cmd.ReceiveBuffer[3];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[4];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];
                break;
            case 0x04:
                Picc.UID = Cmd.ReceiveBuffer[3];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[4];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];

                break;
            case 0x05:
                //nothing to do
                break;
            case 0x06:
                for(i = 0; i < 16; i ++)
                {
                    Picc.BlockData[i] = Cmd.ReceiveBuffer[3 + i];
                }
                break;
            case 0x07:
                //nothing to do
                break;
            case 0x08:
                //nothing to do
                break;
            case 0x09:
                //nothing to do
                break;
            case 0x0A:
                //nothing to do
                break;
            case 0x0B:
                //nothing to do
                break;
            case 0x10:
                Picc.Type = Cmd.ReceiveBuffer[3];
                Picc.Type <<= 8;
                Picc.Type |= Cmd.ReceiveBuffer[4];

                Picc.UID = Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[7];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[8];
                break;
            case 0x11:
                Picc.Type = Cmd.ReceiveBuffer[3];
                Picc.Type <<= 8;
                Picc.Type |= Cmd.ReceiveBuffer[4];

                Picc.UID = Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[7];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[8];
                break;
            case 0x12:
                Picc.Type = Cmd.ReceiveBuffer[3];
                Picc.Type <<= 8;
                Picc.Type |= Cmd.ReceiveBuffer[4];

                Picc.UID = Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[7];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[8];

                Picc.Value = Cmd.ReceiveBuffer[9];
                Picc.Value <<= 8;
                Picc.Value = Cmd.ReceiveBuffer[10];
                Picc.Value <<= 8;
                Picc.Value = Cmd.ReceiveBuffer[11];
                Picc.Value <<= 8;
                Picc.Value = Cmd.ReceiveBuffer[12];
                break;
            case 0x13:
                Picc.Type = Cmd.ReceiveBuffer[3];
                Picc.Type <<= 8;
                Picc.Type |= Cmd.ReceiveBuffer[4];

                Picc.UID = Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[7];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[8];

                Picc.Value = Cmd.ReceiveBuffer[9];
                Picc.Value <<= 8;
                Picc.Value = Cmd.ReceiveBuffer[10];
                Picc.Value <<= 8;
                Picc.Value = Cmd.ReceiveBuffer[11];
                Picc.Value <<= 8;
                Picc.Value = Cmd.ReceiveBuffer[12];
                break;
            case 0x14:
                Picc.Type = Cmd.ReceiveBuffer[3];
                Picc.Type <<= 8;
                Picc.Type |= Cmd.ReceiveBuffer[4];

                Picc.UID = Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[7];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[8];

                for(i = 0; i < 16; i ++)
                {
                    Picc.BlockData[i] = Cmd.ReceiveBuffer[9 + i];
                }
                break;
            case 0x15:
                Picc.Type = Cmd.ReceiveBuffer[3];
                Picc.Type <<= 8;
                Picc.Type |= Cmd.ReceiveBuffer[4];

                Picc.UID = Cmd.ReceiveBuffer[5];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[6];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[7];
                Picc.UID <<= 8;
                Picc.UID |= Cmd.ReceiveBuffer[8];

                break;
            default:
                return 0xFD;
                break;
        }
        Cmd.ReceiveFlag = 0;
        return Cmd.ReceiveBuffer[2];
    }
    return 0xFC;
}


