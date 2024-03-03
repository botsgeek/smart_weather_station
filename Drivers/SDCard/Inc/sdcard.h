#ifndef __SDCARD_H___
#define __SDCARD_H___
#include <stdint.h>
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct sdcard_t sdcard_t;
    typedef struct
    {
        GPIO_TypeDef *gpio_port;
        uint16_t gpio_pin;
    } sdcard_config_t;

    typedef struct
    {
        float humidity;
        float temperature;
    } sdcard_data_t;

    sdcard_t *sdcardCreate(const sdcard_config_t *config);
    error_type_t sdcardInit(sdcard_t *sdcard_object);
    error_type_t sdcardDeInit(sdcard_t *sdcard_object);
    error_type_t sdcardRead(sdcard_t *sdcard_object, sdcard_data_t *data);
    error_type_t sdcardWrite(sdcard_t *sdcard_object, sdcard_data_t *data);
    error_type_t sdcardDestroy(sdcard_t **sdcard_object);
#ifdef __cplusplus
}
#endif
#endif