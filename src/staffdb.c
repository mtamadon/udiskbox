#include "main.h"

FIL staffdb_fd;

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: checkserial
 ** 功能描述: 通过rfid读出对应的名字
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2015年7月19日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
int checkserial(char * filename,unsigned char * serialarraycheck,unsigned char * namearray)
{
    unsigned char namelen=0;
    unsigned char serialarray[10]="";

    unsigned int j,k,m=0;
    unsigned char check_flag=0;
    BYTE dbfilebuffer[40];

    UINT br=1;

    FRESULT res = f_open(&staffdb_fd, filename, FA_OPEN_EXISTING | FA_READ);
    printf("3 ");
    if(res)
    {
        printf("not open");
        return -1;
    }
    printf("4 ");
    for(;;)
    {
        res = f_read( &staffdb_fd, dbfilebuffer, RECORD_LEN, &br );
        printf("%02x ",res);
        if (res||br==0)
            break;
        else
        {
            if (dbfilebuffer[0]=='@')
            {
                for(j=1; j<=ID_LEN; j++)
                {
                    serialarray[j-1]=dbfilebuffer[j];
                    check_flag=serialarray[j-1]-serialarraycheck[j-1];
                    if(check_flag)
                        break;
                }
                if (!check_flag)
                {
                    for(k=j; k<RECORD_LEN; k++)
                    {
                        if((dbfilebuffer[k]!=' ')&&(dbfilebuffer[k]!=0x0a) &&(dbfilebuffer[k]!=0x0d) )
                        {
                            namearray[m++]=dbfilebuffer[k];
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
    f_close(&staffdb_fd);
    return 0;
}

