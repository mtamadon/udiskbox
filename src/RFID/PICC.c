/******************** (C) COPYRIGHT 2013 ********************
* �ļ���          : PICC.c
* ����            : ������ӿ����Ŷӡ�
* �汾            : V2.0.1.0
* ����            : 2013-5-10
* ����            : ���жԿ��Ĳ�������ģ�鷢�͵����
* ����֧��        : QQ: 526181679
********************************************************************************/

/* �����ļ� -------------------------------------------------------------------*/
#include "PICC.h"
/* �궨�� ---------------------------------------------------------------------*/
/* ���ļ�ʹ�õı��� -----------------------------------------------------------*/
PICC Picc;
CMD Cmd;
/* ���ļ�ʹ�õĺ������� -------------------------------------------------------*/
void SendCommand(void);
/* ���ļ������� ---------------------------------------------------------------*/

/*******************************************************************************
* ������         : PICCHalt
* ����           : ֹͣ�����
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void PICCHalt(void)
{
	Cmd.SendBuffer[0] =  2;
	Cmd.SendBuffer[1] = 0x01;
	SendCommand();
}

/*******************************************************************************
* ������         : PICCRequest
* ����           : ���󿨡�
* ����           : mode: Ѱ��ģʽ��
										-0x26: Ѱδ��ֹͣ�Ŀ���
										-0x52: Ѱ���п���
* ���           : ��
* ����           : ��
*******************************************************************************/
void PICCRequest(unsigned char mode)
{
	Cmd.SendBuffer[0] = 3;
	Cmd.SendBuffer[1] = 0x02;
	Cmd.SendBuffer[2] = mode;
	SendCommand();
}

/*******************************************************************************
* ������         : PICCAnticoll
* ����           : ����ײ��
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void PICCAnticoll(void)
{
	Cmd.SendBuffer[0] = 2;
	Cmd.SendBuffer[1] = 0x03;
	SendCommand();
}

/*******************************************************************************
* ������         : PICCSelect
* ����           : ѡ�񿨡�
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void PICCSelect(void)
{
	Cmd.SendBuffer[0] = 2;
	Cmd.SendBuffer[1] = 0x04;
	SendCommand();
}

/*******************************************************************************
* ������         : PICCAuthState
* ����           : ��֤��
* ����           : sector: Ҫ��֤�������š�
                   mode:   ��֤ģʽ��
                        -0x60:��֤��Կ A
                        -0x61:��֤��Կ B
                   key:    ��Կ���顣
* ���           : ��
* ����           : ��
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
* ������         : PICCReadBlockData
* ����           : �������ݡ�
* ����           : block: ��Ҫ���Ŀ��ַ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void PICCReadBlockData(unsigned char block)
{
	Cmd.SendBuffer[0] = 3;
	Cmd.SendBuffer[1] = 0x06;
	Cmd.SendBuffer[2] = block;
	SendCommand();
}

/*******************************************************************************
* ������         : PICCWriteBlockData
* ����           : д�����ݡ�
* ����           : block:     ��Ҫд�Ŀ��ַ��
                   blockData: ���������顣
* ���           : ��
* ����           : ��
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
* ������         : PICCInc
* ����           : ��ֵ��
* ����           : block: ��Ҫ�����Ŀ��ַ��
                   value: ��Ҫ���ӵ�ֵ��
* ���           : ��
* ����           : ��
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
* ������         : PICCDec
* ����           : ��ֵ��
* ����           : block: ��Ҫ�����Ŀ��ַ��
                   value: ��Ҫ����ֵ��
* ���           : ��
* ����           : ��
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
* ������         : PICCRestore
* ����           : �������ݡ�
* ����           : block: ��Ҫ�����Ŀ��ַ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void PICCRestore(unsigned char block)
{
	Cmd.SendBuffer[0] = 3;
	Cmd.SendBuffer[1] = 0x0A;
	Cmd.SendBuffer[2] = block;
	SendCommand();
}

/*******************************************************************************
* ������         : PICCTansfer
* ����           : �洢���ݡ�
* ����           : block: ��Ҫ�����Ŀ��ַ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void PICCTansfer(unsigned char block)
{
	Cmd.SendBuffer[0] = 3;
	Cmd.SendBuffer[1] = 0x0B;
	Cmd.SendBuffer[2] = block;
	SendCommand();
}
/*******************************************************************************
* ������         : OneKeyReadCard
* ����           : һ��������
* ����           : ��
* ���           : ��
* ����           : ��
*******************************************************************************/
void OneKeyReadCard(void)
{
	Cmd.SendBuffer[0] = 2;
	Cmd.SendBuffer[1] = 0x10;
	SendCommand();
}
/*******************************************************************************
* ������         : OneKeyMakeCard
* ����           : һ���쿨��
* ����           : block: ��Ҫ�����Ŀ��ַ��
                   value: ��Ҫ������ֵ��
                   keyA : �쿨���õ���ԿA��
									 keyB : �쿨���õ���ԿB��
* ���           : ��
* ����           : ��
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
* ������         : OneKeyInc
* ����           : һ����ֵ��
* ����           : block: ��Ҫ�����Ŀ��ַ��
                   key  : �쿨���õ���Կ��
                   value: ��Ҫ������ֵ��
* ���           : ��
* ����           : ��
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
* ������         : OneKeyInc
* ����           : һ���ۿ
* ����           : block: ��Ҫ�����Ŀ��ַ��
                   key  : �쿨���õ���Կ��
                   value: ��Ҫ������ֵ��
* ���           : ��
* ����           : ��
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
* ������         : OneKeyReadBlock
* ����           : һ�����顣
* ����           : block: ��Ҫ�����Ŀ��ַ��
                   keyA : ��ԿA��
* ���           : ��
* ����           : ��
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
* ������         : OneKeyWriteBlock
* ����           : һ��д�顣
* ����           : block: ��Ҫ�����Ŀ��ַ��
                   keyB : ��ԿB��
* ���           : ��
* ����           : ��
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
* ������         : CommandProcess
* ����           : �����
* ����           : ��
* ���           : ��
* ����           : ִ�н����
                   -0   : ��ȷ��
                   -0xFF: �޿���
                   -0xFE: ����������
                   -0xFD: �������
                   -0xFC: û���յ����
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


