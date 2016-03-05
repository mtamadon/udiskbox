#ifndef MAIN_H
#define MAIN_H

#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#include "cmsis_os.h"

#include "usart.h"
#include "PICC.h"
#include "ds1302.h"
#include "rfidupan.h"
#include "sd_board.h"

#include "spi_enc28j60.h"
#include "net.h"
#include "lcd.h"
#include "strutils.h"

#define ID_LEN 10
#define RECORD_LEN 22 //@xxx(RFID号码10位)+
                               //空格(1位)+
                               //xxx(名称前面补空格)+
                               //0A0D(换行符)
#define UDISK_RECORD_LEN 32 //@xxx(RFID号码10位)+
                                     //空格(1位)+
                                     //xxx(5位upan名称)+
                                     //空格(1位)+
                                     //xxx(借U盘人的RFID号码10位)+0A0D(换行符)

#define NEED_UPDATE_A 1<<0
#define NEED_UPDATE_E 1<<1

#define UDISK1 1<<0
#define UDISK2 1<<1
#define UDISK3 1<<2

extern DISSTRUCT LCDSTRUCT;

extern const char *userfilename;
extern const char *upanfilename;

extern osThreadId rfid_ThreadID;
extern osThreadId udisk_ThreadID;
extern osThreadId lcdupdate_ThreadID;

extern void monitorRFID();
extern void monitorUdisk();

extern xSemaphoreHandle sem_cmd;

extern uint8_t upanState;

#endif
