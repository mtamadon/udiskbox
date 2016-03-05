#include "strutils.h"

/*******************************************************************************
 * 函数名         : LCDNumber
 * 描述           : 往 12864 写数字
 * 输入           : address: 地址
 number : 数字
 count  : 数字显示的长度
 * 输出           : 无
 * 返回           : 无
 *******************************************************************************/
void LongToStr(char *array, unsigned long number, unsigned char count)
{
    // unsigned char array[11];
    unsigned char i;

    array[count] = 0;
    for(i = count; i > 0; i --)
    {
        array[i-1] = number % 10+'0';
        number /= 10;
    }
    for(i = 0; i < count-1; i ++)
    {
        if(array[i]=='0')
        {
            array[i] = ' ';
        }
        else
        {
            break;
        }
    }
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: uchar2str
 ** 功能描述: 刷新LCD
 ** 参数描述：无
 ** 作  　者: cuikun
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void uchar2str(uint8_t i,unsigned char * strname)
{
    unsigned k,m=0;
    k=i/100;
    if(k>0)
        strname[m++]=k+0x30;

    k=(i-k*100)/10;
    if(k>0)
        strname[m++]=k+0x30;

    k=i%10;
    if(k>0)
        strname[m++]=k+0x30;

    strname[m]='\0';
}
