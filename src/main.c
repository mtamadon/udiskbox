/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * �ļ���  ��main.c
 * ����    ��MicroSD��(SDIOģʽ)����ʵ�飬����������Ϣͨ������1�ڵ��Եĳ����ն���
 *           ��ӡ����
 * ʵ��ƽ̨��Ұ��STM32������
 * ��汾  ��ST3.5.0
 *
 * ����    ��wildfire team
 * ��̳    ��http://www.amobbs.com/forum-1008-1.html
 * �Ա�    ��http://firestm32.taobao.com
*********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stm32f1xx_hal_conf.h>
#include "sdio_sdcard.h"
#include "ff.h"
#include "usually.h"
#include "usart.h"
#include "PICC.h"
#include "string.h"
#include "stdio.h"
#include "STDLIB.H"
#include "ds1302.h"
#include "rfidupan.h"

#include "spi_enc28j60.h"
#include "web_server.h"
#include "net.h"


//��������

//��ʾ�ṹ��
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





//��������

void LCDUpdate(char stype);    //LCD��ʾ
void Init_LCDSTRUCT(void);

void Time_Conv(u8 * tt,unsigned char cnt,char * timestr);  //����ʱ��
char PcdRequest(unsigned char req_code,unsigned char *pTagType);
void ncs(unsigned char cse);

void Init_LED(void);
void Init_NVIC(void);
void Delay_Ms(uint16_t time);
void Delay_Us(uint16_t time);

void Init_RfidUpan_GPIO(void);
void Init_RFID(void);  //��ʼ��RFID
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
// ȫ�ֱ���������
u8 bTemp;
static unsigned char rfid1=0;
static unsigned char rfid2=0;
static unsigned char door_state=0;
static unsigned char door_closed=0;
int res;
int a;
long upanNum[2]={157384350,2306834590};
static unsigned char upanState=0x00;

// �ļ�������������
unsigned char serialnum=10,namenum=8,strall=22; //@xxx(RFID����10λ)+�ո�(1λ)+xxx(����ǰ�油�ո�)+0A0D(���з�)
unsigned char upanstrall=32; //@xxx(RFID����10λ)+�ո�(1λ)+xxx(5λupan����)+�ո�(1λ)+xxx(��U���˵�RFID����10λ)+0A0D(���з�)
FIL fdrd,fdwr;
FATFS fs;
UINT br, bw;            // File R/W count
BYTE buffer;       // file copy buffer
BYTE filetemp[40]="";


unsigned char userfilename[20]="0:/cardlist.txt"; //ְ��txt
unsigned char upanfilename[20]="0:/upanlist.txt"; //ְ��txt


u8 key_time = 0;
unsigned char count;



extern  unsigned char buf[1501];



extern unsigned char indarray[20];
extern unsigned char lenind;
unsigned char gflag_send=0;
DWORD send_count=0;

int main(void)
{


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		** ��������
		:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

		u8 tt[7];
	  char rfid_status;
    unsigned char g_ucTempbuf[20];

	  //unsigned char serialarraycheck[10]="1119241448";
	  unsigned char namearray[20]="";
	  int stread,stwrite=0;
	  unsigned char strtmp[30]="";



	/////////////////////////////////////////////////////////////////////////////////////////////////
		/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		** ��ʼ������
		:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		SystemInit();					//ϵͳʱ������
		Init_NVIC();					//�ж�������ע�ắ��
		NVIC_Configuration(); //SDIO�жϴ����ʼ��
		Init_LED();						//����������������
		Init_Usart();					//������������
		Usart_Configuration(USART1,115200);	//����1���� ���ò�����Ϊ115200
		Usart_Configuration(USART2,9600);	//����2���� ���ò�����Ϊ115200
		Usart_Configuration(USART3,115200);	//����3���� ���ò�����Ϊ115200
		Delay_Ms(200);        //�ȴ�200msȷ����Ļ����

		Init_LCDSTRUCT();
	//	LCDUpdate('a');         //LCD��ʾ
		//LCDUpdate('n');
		printf("start..\n\t");

		Cmd.SendFlag = 0;       //��ʼ��RFID��־λ
		Cmd.ReceiveFlag = 0;
		Picc.Value = 0;



		InitClock();            //����DS1302
	// 	tt[0] = 0x15;
	// 	tt[1] = 0x04;
	// 	tt[2] = 0x27;
	// 	tt[3] = 0x21;
	// 	tt[4] = 0x05;
	// 	tt[5] = 0x00;
	// 	WriteDS1302Clock(tt);
	//	for(i=1;i<3;i++)
	//{
		//Init_RfidUpan_GPIO();
		//ncs(1);
	 // Init_RfidUpan();
		//ncs(2);
	  //Init_RfidUpan();
	//	Delay_Ms(20);
	//}

		Init_RfidUpan_GPIO();



	/* ENC28J60 SPI �ӿڳ�ʼ�� */
  SPI_Enc28j60_Init();//������ʼ��
	SetIpMac();
	gflag_send=0;       //������ʼ��
  send_count=0;
	/* �����ļ�ϵͳ*/

	f_mount(0,&fs);
  LCDShowUpanState(upanfilename);

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
		** ѭ������
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
		while(1)
		{
			ReadDS1302Clock(tt);
			Time_Conv(tt,6,LCDSTRUCT.TimeNow);
////////////////////////////////////////////////////////////////////////////////////	    	��⿪��
			Web_Server();   //����ģ�鿪ʼ����

//////////////////////////////////////////////////////////////////////
			bTemp = CommandProcess();
			if(bTemp == 0)
			{
				//�����￪ʼ��Ĳ���
				//�������������� Picc
				//����	==>Picc.UID
				//������==>Picc.Type
				//���	==>Picc.Value
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
					else if(stread>0)   	//�Ƚ��Ƿ��ǺϷ���
					{
						door_state=1;				//����
						strcpy(strtmp,namearray);//��ʾ��������ʾ����
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
				//�޿�
			}
			else if(bTemp == 0xFE)
			{
				LCDUpdate('e');
			}
			else if(bTemp == 0xFD)
			{
				//��������
			}

///////////////////////////////////////////////////////////////////////////////////////				������
			if((door_state==1)&&(door_closed==1))
			{
    		door_state++;
				rfid1=0;
				rfid2=0;
				//door_closed=0;
				//LCDUpdate('a');
			}
///////////////////////////////////////////////////////////////////////////////////////   		���u��
			if(door_state!=2){door_closed=0;continue;}

			upanState=0x00;
/////////////////////////////////////////////////////////////////////////////////////////			RFID2
			if(rfid2==0)
			{
				ncs(2);//Ƭѡ
				Init_RfidUpan();
				rfid_status = PcdRequest(PICC_REQALL,g_ucTempbuf);//ɨ�迨
				printf("%02x  ",rfid_status);
				if(rfid_status==0)
				{
					rfid_status = PcdAnticoll(g_ucTempbuf);//����ײ
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
///////////////////////////////////////////////////////////////////////////////////////////     	RFID1
			if(rfid1==0)
			{
				ncs(1);
				Init_RfidUpan();
				rfid_status = PcdRequest(PICC_REQALL,g_ucTempbuf);//ɨ�迨
				if(rfid_status==0)
				{
					rfid_status = PcdAnticoll(g_ucTempbuf);//����ײ
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
///////////////////////////////////////////////////////////////////////////////////     ��ʾ״̬
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
** ��������: Time_Conv
** ��������: LED IO��������
** ������������
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Time_Conv(u8 * tt,unsigned char cnt,char * timestr)
{

	timestr[0]='2';                //��
	timestr[1]='0';
	timestr[2]=(tt[0]>>4) + 0x30;
	timestr[3]=(tt[0]&0x0f)+0x30;
	timestr[4]='-';

	timestr[5]=(tt[1]>>4) + 0x30;  //��
	timestr[6]=(tt[1]&0x0f)+0x30;
	timestr[7]='-';

	timestr[8]=(tt[2]>>4) + 0x30;  //��
	timestr[9]=(tt[2]&0x0f)+0x30;
	timestr[10]=' ';

	timestr[11]=(tt[3]>>4) + 0x30;  //ʱ
	timestr[12]=(tt[3]&0x0f)+0x30;
	timestr[13]=':';

	timestr[14]=(tt[4]>>4) + 0x30;  //��
	timestr[15]=(tt[4]&0x0f)+0x30;
	timestr[16]='\0';




}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Init_LED
** ��������: LED IO��������
** ������������
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_LED()
{
	GPIO_InitTypeDef GPIO_InitStructure;					//����һ��GPIO�ṹ�����

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG,ENABLE);	//ʹ�ܸ����˿�ʱ�ӣ���Ҫ������

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;										//����LED�˿ڹҽӵ�6��12��13�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   	//ͨ���������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOG, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOD�Ĵ���RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE,ENABLE);	//ʹ�ܸ����˿�ʱ�ӣ���Ҫ������

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD,ENABLE);	//ʹ�ܸ����˿�ʱ�ӣ���Ҫ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;										//����LED�˿ڹҽӵ�6��12��13�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   	//ͨ���������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOD, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOD�Ĵ���


}


/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Init_NVIC
** ��������: ϵͳ�ж�����
** ������������
** ��  ����: Dream
** �ա�  ��: 2011��5��14��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_NVIC(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;			//����һ��NVIC������ṹ�����

	#ifdef  VECT_TAB_RAM  							//���������ַѡ��

	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//��0x20000000��ַ��Ϊ���������ַ(RAM)
	#else

	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //��0x08000000��ַ��Ϊ���������ַ(FLASH)
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж��� Ϊ2

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//���ô���2Ϊ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  	//���ø����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//ʹ�ܴ���1�ж�
	NVIC_Init(&NVIC_InitStructure);							  	//���ݲ�����ʼ���жϼĴ���
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Delay_Ms_Ms
** ��������: ��ʱ1MS (��ͨ���������ж�����׼ȷ��)
** ����������time (ms) ע��time<65535
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Ms(uint16_t time)  //��ʱ����
{
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<10260;j++);
}
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Delay_Ms_Us
** ��������: ��ʱ1us (��ͨ���������ж�����׼ȷ��)
** ����������time (us) ע��time<65535
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Delay_Us(uint16_t time)  //��ʱ����
{
	uint16_t i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<9;j++);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: Init_RFID
** ��������: ��ʼ��RFID
** ����������
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Init_RFID()  //��ʼ��RFID
{
	;
}

/*******************************************************************************
* ������         : CheckSum
* ����           : ����У�顣
* ����           : 	dat : ҪУ������顣
										num : У����ֽ�����
* ���           : ��
* ����           : У��ֵ��
*******************************************************************************/
unsigned char CheckSum(unsigned char *dat, unsigned char num)
{
  unsigned char bTemp = 0, i;

  for(i = 0; i < num; i ++){bTemp ^= dat[i];}
  return bTemp;
}

/*******************************************************************************
* ������         : SendCommand
* ����           : ���ʼ���ͺ�����
* ����           : ��
* ���           : ��
* ����           : ��
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
* ������         : LCDNumber
* ����           : �� 12864 д����
* ����           : address: ��ַ
                   number : ����
                   count  : ������ʾ�ĳ���
* ���           : ��
* ����           : ��
*******************************************************************************/
void LongToStr( char *array, unsigned long number, unsigned char count)
{
//	unsigned char array[11];
	unsigned char i;

	array[count] = 0;
	for(i = count; i > 0; i --){array[i-1] = number % 10+'0';number /= 10;}
	for(i = 0; i < count-1; i ++){if(array[i]=='0'){array[i] = ' ';}else{break;}}
}

/*******************************************************************************
* ������         : Init_LCDSTRUCT
* ����           : ��ʼ��LCDSTRUCT�ṹ��,ȫ����ֵ��
* ����           : ��
* ���           : ��
* ����           : ��
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
** ��������: LCDUpdate
** ��������: ˢ��LCD
** ������������
** ��  ����: Dream
** �ա�  ��: 2011��6��20��
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
				Lcd_Display("CLS(0);DS16(100,0,'SITP U�̹���ϵͳ',16);");
				Lcd_Display("DS16(4,24,'������',15);");	     Lcd_Display(Name_Array);
				Lcd_Display("DS16(4,48,'���ţ�',4);");	       Lcd_Display(UserID_Array);
				Lcd_Display("DS16(4,72,'��ϵ�绰��',15);");	 Lcd_Display(PhoneNum_Array);
				Lcd_Display("DS16(4,96,'U����Ϣ��',4);");	   Lcd_Display(UdiskInfo_Array);Lcd_Display(UdiskInfo_Array2);
				Lcd_Display("DS16(4,120,'��/��״̬��',4);");   Lcd_Display(UdiskState_Array);
				Lcd_Display("DS16(4,144,'�����¶ȣ�',15);");   Lcd_Display(Temperature_Array);
				Lcd_Display("DS16(4,168,'����/ʱ�䣺',15);");  Lcd_Display(TimeNow_Array);
			}break;
	 case 'n':
	    {
				strcpy(Name_Array,"DS16(100,24,'");strcat(Name_Array,LCDSTRUCT.Name);strcat(Name_Array,"',4);");

				Lcd_Display("CLS(0);DS16(100,0,'SITP U�̹���ϵͳ',16);");
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
				Lcd_Display("CLS(0);DS16(100,0,'SITP U�̹���ϵͳ',16);");

		  }break;
	}




}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: uchar2str
** ��������: ˢ��LCD
** ������������
** ��  ����: cuikun
** �ա�  ��: 2011��6��20��
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
** ��������: LCDShowUpanState
** ��������: ˢ��LCD
** ������������
** ��  ����: cuikun
** �ա�  ��: 2011��6��20��
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int LCDShowUpanState(unsigned char * filename)
{
	 char space[4]="  ";
	 char norm[6]="����";
	 char borrow[6]="���";
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
	Lcd_Display("CLS(0);DS16(100,0,'SITP U�̹���ϵͳ',16);");

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
				strcat(firstline,",'������',16);");
				printf(firstline);printf("\r\n");
				Lcd_Display(firstline);
				Lcd_Display(Uinfolist);

			}
	  }
		f_close(&fdrd);
		return 0;
	}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
** ��������: checkserial
** ��������: ͨ��rfid������Ӧ������
** ������������
** ��  ����: cuikun
** �ա�  ��: 2015��7��19��
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
** ��������: writelog
** ��������: д��log��־�ļ�
** ������������
** ��  ����: cuikun
** �ա�  ��: 2015��7��19��
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
** ��������: writelog
** ��������: д��log��־�ļ�
** ������������
** ��  ����: cuikun
** �ա�  ��: 2015��7��19��
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

