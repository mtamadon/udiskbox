#ifndef DS1302_H
#define DS1302_H

#include <inttypes.h>

extern void InitClock(void);
extern void ReadDS1302Clock(uint8_t *p);
extern void WriteDS1302Clock(uint8_t *p);
extern void ReadDSRam(uint8_t *p,uint8_t add,uint8_t cnt);
extern void WriteDSRam(uint8_t *p,uint8_t add,uint8_t cnt);
extern void TestDS1302(void);

#endif

