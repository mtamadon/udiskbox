#include "main.h"

DISSTRUCT LCDSTRUCT;
extern FIL staffdb_fd;
extern int checkserial(char *filename, uint8_t *serialarraycheck, uint8_t *namearray);

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
                strcpy(Name_Array,"DS16(100,24,'");
                strcat(Name_Array,LCDSTRUCT.Name);
                strcat(Name_Array,"',4);");

                strcpy(UserID_Array,"DS16(100,48,'");
                strcat(UserID_Array,LCDSTRUCT.UserID);
                strcat(UserID_Array,"',4);");

                strcpy(PhoneNum_Array,"DS16(100,72,'");
                strcat(PhoneNum_Array,LCDSTRUCT.PhoneNum);
                strcat(PhoneNum_Array,"',4);");

                strcpy(UdiskInfo_Array,"DS16(80,96,'");
                strcat(UdiskInfo_Array,LCDSTRUCT.UdiskInfo);
                strcat(UdiskInfo_Array,"',4);");

                strcpy(UdiskInfo_Array2,"DS16(200,96,'");
                strcat(UdiskInfo_Array2,LCDSTRUCT.UdiskInfo2);
                strcat(UdiskInfo_Array2,"',4);");

                strcpy(UdiskState_Array,"DS16(100,120,'");
                strcat(UdiskState_Array,LCDSTRUCT.UdiskState);
                strcat(UdiskState_Array,"',4);");

                strcpy(Temperature_Array,"DS16(100,144,'");
                strcat(Temperature_Array,LCDSTRUCT.Temperature);
                strcat(Temperature_Array,"',4);");

                strcpy(TimeNow_Array,"DS16(100,168,'");
                strcat(TimeNow_Array,LCDSTRUCT.TimeNow);
                strcat(TimeNow_Array,"',4);");

                Lcd_Display("DR2;CLS(0);SPG(1);\r\n");
                Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");
                Lcd_Display("DS16(4,24,'姓名：',15);");
                Lcd_Display(Name_Array);
                Lcd_Display("DS16(4,48,'卡号：',4);");
                Lcd_Display(UserID_Array);
                Lcd_Display("DS16(4,72,'联系电话：',15);");
                Lcd_Display(PhoneNum_Array);
                Lcd_Display("DS16(4,96,'U盘信息：',4);");
                Lcd_Display(UdiskInfo_Array);
                Lcd_Display(UdiskInfo_Array2);
                Lcd_Display("DS16(4,120,'借/还状态：',4);");
                Lcd_Display(UdiskState_Array);
                Lcd_Display("DS16(4,144,'室内温度：',15);");
                Lcd_Display(Temperature_Array);
                Lcd_Display("DS16(4,168,'日期/时间：',15);");
                Lcd_Display(TimeNow_Array);
            } break;
        case 'n':
            {
                strcpy(Name_Array,"DS16(100,24,'");
                strcat(Name_Array,LCDSTRUCT.Name);
                strcat(Name_Array,"',4);");
                Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");
                Lcd_Display(Name_Array);
            }break;
        case 'u':
            {
                strcpy(UserID_Array,"DS16(100,48,'");
                strcat(UserID_Array,LCDSTRUCT.UserID);
                strcat(UserID_Array,"',4);");
                Lcd_Display(UserID_Array);
                HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
            }break;
        case 'p':
            {
                strcpy(PhoneNum_Array,"DS16(100,72,'");
                strcat(PhoneNum_Array,LCDSTRUCT.PhoneNum);
                strcat(PhoneNum_Array,"',4);");
                Lcd_Display(PhoneNum_Array);
            }break;
        case 'i':
            {
                strcpy(UdiskInfo_Array,"DS16(100,96,'");
                strcat(UdiskInfo_Array,LCDSTRUCT.UdiskInfo);
                strcat(UdiskInfo_Array,"',4);");
                Lcd_Display(UdiskInfo_Array);
            }break;
        case 's':
            {
                strcpy(UdiskState_Array,"DS16(100,120,'");
                strcat(UdiskState_Array,LCDSTRUCT.UdiskState);
                strcat(UdiskState_Array,"',4);");
                Lcd_Display(UdiskState_Array);
            }break;
        case 't':
            {
                strcpy(Temperature_Array,"DS16(100,144,'");
                strcat(Temperature_Array,LCDSTRUCT.Temperature);
                strcat(Temperature_Array,"',4);");
                Lcd_Display(Temperature_Array);
            }break;
        case 'd':
            {
                strcpy(TimeNow_Array,"DS16(100,168,'");
                strcat(TimeNow_Array,LCDSTRUCT.TimeNow);
                strcat(TimeNow_Array,"',4);");
                Lcd_Display(TimeNow_Array);
            }break;
        case 'e':
            {
                Lcd_Display("DR2;CLS(0);SPG(1);\r\n");
                Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");
            }break;
    }
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
 ** 函数名称: LCDShowUpanState
 ** 功能描述: 刷新LCD
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int LCDShowUpanState(char * filename)
{
    uint8_t space[4]="  ";
    uint8_t norm[6]="正常";
    uint8_t borrow[6]="借出";
    uint8_t strtmp[30]="";
    uint8_t Uinfolist[64]="";
    uint8_t namearray[10]="";
    uint8_t serialarray[11]="";
    BYTE upantemp[40];
    BYTE dbfilebuffer[40];
    char firstline[20]="";

    unsigned char i,j=0;
    unsigned char pos[4]="";

    Lcd_Display("DR2;CLS(0);SPG(1);\r\n");
    Lcd_Display("CLS(0);DS16(100,0,'SITP U盘管理系统',16);");

    FRESULT res=f_open(&staffdb_fd, filename, FA_OPEN_EXISTING | FA_READ);
    if(res)
    {
        printf("not open"); //TODO
        return -1;
    }
    while(1)
    {
        UINT br=0;
        res = f_read(&staffdb_fd, dbfilebuffer, UDISK_RECORD_LEN, &br );
        if (res||br<UDISK_RECORD_LEN)
            break;
        else
        {
            if (dbfilebuffer[18]=='0')
            {
                memcpy(strtmp,dbfilebuffer+12,5);
                //memcpy(strtmp+5,space,2);
                strtmp[5] = ' ';
                strtmp[6] = ' ';
                memcpy(strtmp+7,norm,4);
                strtmp[11]='\0';
                printf(strtmp); printf("\r\n"); //TODO
            }
            else if(dbfilebuffer[18]=='1')
            {
                memcpy(upantemp,dbfilebuffer,40);
                memcpy(serialarray,dbfilebuffer+20,10);
                serialarray[10]='\0';

                checkserial(userfilename, serialarray, namearray);

                memcpy(strtmp,upantemp+12,5);
                //memcpy(strtmp+5,space,2);
                strtmp[5] = ' ';
                strtmp[6] = ' ';
                memcpy(strtmp+7,borrow,4);
                //memcpy(strtmp+11,space,2);
                strtmp[11] = ' ';
                strtmp[12] = ' ';
                strtmp[13] = '\0';
                strcat(strtmp,namearray);
                printf(strtmp); printf("\r\n"); //TODO
            }
            i++;
            j=i*24;
            uchar2str(j,pos);

            strcpy(Uinfolist,"DS16(100,");
            strcat(Uinfolist,pos);
            strcat(Uinfolist,",'");
            printf(Uinfolist);

            strcat(Uinfolist,strtmp);
            printf(Uinfolist);

            strcat(Uinfolist,"',16);");
            printf(Uinfolist);printf("\r\n");

            strcpy(firstline,"DS16(4,");
            strcat(firstline,pos);
            strcat(firstline,",'姓名：',16);");
            printf(firstline);printf("\r\n");

            Lcd_Display(firstline);
            Lcd_Display(Uinfolist);
        }
    }
    f_close(&staffdb_fd);
    return 0;
}

void LCDUpdateThread()
{
    osEvent lcdupdate_event;
    lcdupdate_event = osSignalWait(NEED_UPDATE_A|NEED_UPDATE_E, osWaitForever);
    if(lcdupdate_event.value.signals == NEED_UPDATE_A)
        LCDUpdate('a');
}
