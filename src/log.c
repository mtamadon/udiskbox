#include "log.h"
#include "web_server.h"
#include "net.h"

extern unsigned  int fill_tcp_data_p(unsigned char *buf,unsigned  int pos, const unsigned char *progmem_s);
extern void SendTcp(unsigned int plen);

UINT br, bw;            // File R/W count
extern uint8_t RECORD_LEN;
extern  unsigned char buf[1501];

FIL log_fd;

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: writelog
 ** 功能描述: 写入log日志文件
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2015年7月19日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int writelog(char * filename, unsigned char * filecontent, unsigned char lencont, unsigned char sflag )
{
    FRESULT log_return;
    bw=1  ;

    if(sflag==0)
    {
        log_return = f_open(&log_fd, filename,FA_CREATE_ALWAYS| FA_WRITE);
        if(log_return)
        {
            printf("not open"); //TODO
            return -1 ;
        }
    }
    else if(sflag==1)
    {
        log_return = f_open(&log_fd,filename,FA_OPEN_ALWAYS| FA_WRITE);
        if(log_return)
        {
            printf("not open"); //TODO
            return -1 ;
        }
        f_lseek(&log_fd,log_fd.fsize);
    }

    log_return = f_write(&log_fd, filecontent, lencont, &bw);
    /*if(log_return)*/
        printf("write error!\n"); //TODO
    f_close(&log_fd);

    return 0;
}

unsigned char gflag_send=0;
DWORD send_count=0;
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: writelog
 ** 功能描述: 写入log日志文件
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2015年7月19日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int printallfile(char * filename )
{
    FRESULT log_return;
    unsigned int plen;
    br=1;
    FIL log_fd;
    BYTE logfilebuffer[40];

    log_return=f_open(&log_fd,filename, FA_OPEN_EXISTING | FA_READ);
    if(log_return)
    {
        printf("not open"); //TODO
        return -1;
    }

    if(gflag_send==0)
    {
        log_return =f_read( &log_fd, logfilebuffer, RECORD_LEN, &br );
        if (log_return||br==0)
        {
            gflag_send=2;
            return -2;
        }
        else
        {
            if(br<RECORD_LEN)
            {
                logfilebuffer[br]='\0';
                gflag_send=2;
            }
            else
            {
                logfilebuffer[RECORD_LEN]='\0';
                gflag_send=1;
            }
            send_count+=RECORD_LEN;
            plen=fill_tcp_data_p(buf,0,logfilebuffer);
            SendTcp(plen);
            printf("%d\r\n",gflag_send);
            printf(logfilebuffer);printf("\r\n");

        }
    }
    else if(gflag_send==1)
    {
        f_lseek(&log_fd,send_count);
        log_return =f_read( &log_fd, logfilebuffer, RECORD_LEN, &br );
        if (log_return||br==0)
        {
            gflag_send=2;
            return -2;
        }
        else
        {
            if(br<RECORD_LEN)
            {
                logfilebuffer[br]='\0';
                gflag_send=2;
            }
            else
            {
                logfilebuffer[RECORD_LEN]='\0';
                gflag_send=1;
            }
            send_count+=RECORD_LEN;
            plen=fill_tcp_data_p(buf,0,logfilebuffer);
            SendTcp(plen);
            printf("%d\r\n",gflag_send); //TODO
            printf(logfilebuffer);printf("\r\n");

        }
    }
    else if(gflag_send==2)
    {
        plen=fill_tcp_data_p(buf,0,"lend");
        SendTcp(plen);
        printf("%d\r\n",gflag_send); //TODO
        //printf(logfilebuffer);

    }

    f_close(&log_fd);
    return 0;
}
