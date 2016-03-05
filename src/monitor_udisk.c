#include "main.h"

extern void Time_Conv(uint8_t * tt,unsigned char cnt,char * timestr);  //日期时间

extern PICC Picc;
extern DISSTRUCT LCDSTRUCT;

const uint32_t upanNum[2]={157384350, 2306834590};

uint8_t checkUdiskState(uint8_t ports);
uint8_t upanState = 0x00;

void monitorUdisk()
{
    upanState=0x00;
    while(1)
    {
        uint8_t state = checkUdiskState( (uint8_t)0x01<<2 & (uint8_t)0x01<<1 & (uint8_t)0x01 );

        if((upanState ^ UDISK1) != 0) //TODO
        {
            strcat(LCDSTRUCT.UdiskInfo1," (1)");
        }

        if((upanState ^ UDISK2) != 0)
        {
            strcat(LCDSTRUCT.UdiskInfo2," (2)");
        }

        if((upanState ^ UDISK3) != 0)
        {
            strcat(LCDSTRUCT.UdiskInfo3," (3)");
        }

        osSignalSet(lcdupdate_ThreadID, NEED_UPDATE_A);
        vTaskDelay(100/portTICK_PERIOD_MS); //FIXME
    }
}

uint8_t checkUdiskState(uint8_t ports)
{
    uint8_t state = 0x00;
    unsigned char g_ucTempbuf[20];

    for(uint8_t i = 0; i<8; i++)
    {
        if( (ports & 0x01<<i) == 0)
        {
            state &= ~(0x01 << i);
            continue;
        }
        uint8_t mi_status;
        ncs(i);//片选
        Init_RfidUpan();
        mi_status = PcdRequest(PICC_REQALL, g_ucTempbuf);//扫描卡
        printf("%02x  ", mi_status); //TODO

        if(mi_status==MI_OK)
        {
            mi_status = PcdAnticoll(g_ucTempbuf);//防冲撞
            if(mi_status==MI_OK)
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
                    LongToStr(LCDSTRUCT.UdiskInfo1,Picc.UID,10);
                    strcat(LCDSTRUCT.UdiskInfo1," (1)");
                }
                else if((Picc.UID-upanNum[1])==0)
                {
                    upanState=upanState|0x02;
                    LongToStr(LCDSTRUCT.UdiskInfo2,Picc.UID,10);
                    strcat(LCDSTRUCT.UdiskInfo2," (2)");
                }
            }
        }
    }

    return state;
}
