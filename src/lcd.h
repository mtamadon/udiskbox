#include "string.h"
#include "stdint.h"

#ifndef LCD_H
#define LCD_H

typedef struct _DISSTRUCT
{
    char Name[32];
    char UserID[32];
    char PhoneNum[32];
    char UdiskInfo[32];
    char UdiskInfo2[32];
    char UdiskState[32];
    char Temperature[32];
    char TimeNow[32];
} DISSTRUCT ;

void Init_LCDSTRUCT(void);
void LCDUpdate(char stype);    //LCD显示
int LCDShowUpanState(char * filename);

#endif //LCD_H
