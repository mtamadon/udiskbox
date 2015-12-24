/******************** (C) COPYRIGHT 2013 ********************
* 文件名          : PICC.h
* 作者            : 优灵电子开发团队
* 版本            : V2.0.1.0
* 日期            : 2013-5-10
* 描述            : 文件 PICC.c 的头文件
* 技术支持        : QQ: 526181679
*********************************************************************************/

#ifndef __PICC_H
#define __PICC_H

/* 包含文件 -------------------------------------------------------------------*/
/* 外部类型 -------------------------------------------------------------------*/
typedef struct _CMD
{
    unsigned char ReceiveBuffer[32];
    unsigned char SendBuffer[32];
    unsigned char ReceivePoint;
    unsigned char SendPoint;
    unsigned char SendFlag;
    unsigned char ReceiveFlag;
}CMD;

typedef struct _PICC
{
	unsigned int Type;
	unsigned long UID;
	unsigned long Value;
	unsigned char KeyA[6];
	unsigned char KeyB[6];
	unsigned char BlockData[16];
}PICC;

/* 外部变量 -------------------------------------------------------------------*/
extern PICC Picc;
extern CMD Cmd;
/* 外部函数 ------------------------------------------------------------------ */
void PICCHalt(void);
void PICCRequest(unsigned char mode);
void PICCAnticoll(void);
void PICCSelect(void);
void PICCAuthState(unsigned char sector, unsigned char mode, unsigned char *key);
void PICCReadBlockData(unsigned char block);
void PICCWriteBlockData(unsigned char block, unsigned char *blockData);
void PICCInc(unsigned char block, unsigned long value);
void PICCDec(unsigned char block, unsigned long value);
void PICCRestore(unsigned char block);
void PICCTansfer(unsigned char block);
void OneKeyReadCard(void);
void OneKeyMakeCard(unsigned char block, unsigned long value, unsigned char *keyA, unsigned char *keyB);
void OneKeyInc(unsigned char block, unsigned char *key, unsigned long value);
void OneKeyDec(unsigned char block, unsigned char *key, unsigned long value);
unsigned char CommandProcess(void);
#endif

