#ifndef __SDCARD_H__
#define __SDCARD_H__
#include "stm32f4xx_hal_conf.h"

void sdcard_init(void);
int sdcard_read(char *buf, uint32_t sector, uint32_t count);
int sdcard_write(char *buf, uint32_t sector, uint32_t count);

#endif