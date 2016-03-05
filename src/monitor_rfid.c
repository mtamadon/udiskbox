#include "main.h"

extern int checkserial(const char *filename, uint8_t *serialarraycheck, uint8_t *namearray);
extern void LCDUpdate(char stype);
extern void Time_Conv(uint8_t * tt,unsigned char cnt,char * timestr);  //日期时间

int stread, stwrite=0;

void monitorRFID()
{
    unsigned char namearray[20]="";
    unsigned char strtmp[30]="";
    uint8_t cmd_status;

    Cmd.SendFlag = 0;       //初始化RFID标志位
    Cmd.ReceiveFlag = 0;
    Picc.Value = 0;
    uint8_t tt[7];

    while(1)
    {
        ReadDS1302Clock(tt);
        Time_Conv(tt, 6, LCDSTRUCT.TimeNow);

        xSemaphoreTake(sem_cmd, 0);
        cmd_status = CommandProcess();
        xSemaphoreGive(sem_cmd);

        if(cmd_status == 0)
        {
            //在这里开始你的操作
            //所有有用数据在 Picc
            //卡号 ==>Picc.UID
            //卡类型==>Picc.Type
            //余额 ==>Picc.Value

            if(1)//door_state==0)
            {
                LongToStr(LCDSTRUCT.UserID, Picc.UID, 10);
                //LCDUpdate('a');
                stread = checkserial(userfilename, LCDSTRUCT.UserID, namearray);
                if(stread==-1)
                    strcpy(strtmp, "OPEN FILE ERROR");
                else if(stread==0)
                {
                    //strcpy(strtmp,"NOT FIND");
                    closeDoor();
                }
                else if(stread>0)    //比较是否是合法卡
                {
                    openDoor(); //TODO: send signal
                    strcpy(strtmp, (char *)namearray);//显示姓名，提示关门
                    strcpy(LCDSTRUCT.Name, strtmp);
                    LCDUpdate('a');
                }
                //strcpy(LCDSTRUCT.Name,strtmp);
                //LCDUpdate('a');
            }
            //door_state=1;
        }
        else if(cmd_status == 0xFF)
        {
            //无卡
            continue;
        }
        else if(cmd_status == 0xFE)
        {
            LCDUpdate('e');
        }
        else if(cmd_status == 0xFD)
        {
            //参数错误
        }
    }

    /*
    //   RFID2
    if(rfid2==0)
    {
    }
    //      RFID1
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
    */
}

