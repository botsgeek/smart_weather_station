#ifndef __DHT22_H___
#define __DHT22_H___
#include <stdint.h>
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct dht22_t dht22_t;
    typedef struct
    {
        GPIO_TypeDef *gpio_port;
        uint16_t gpio_pin;
    } dht22_config_t;

    typedef struct
    {
        float humidity;
        float temperature;
    } dht22_data_t;

    dht22_t *dht22Create(const dht22_config_t *config);
    error_type_t dht22Init(dht22_t *dht22_object);
    error_type_t dht22DeInit(dht22_t *dht22_object);
    error_type_t dht22Read(dht22_t *dht22_object,dht22_data_t* data);
#ifdef __cplusplus
}
#endif
#endif