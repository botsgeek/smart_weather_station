#ifndef __BME280_H___
#define __BME280_H___

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct bme280_t bme280_t;

typedef struct
{
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
} bme280_config_t;

typedef struct
{
    float temperature;
    float pressure;
    float humidity;
} bme280_data_t;

typedef enum
{
    BME280_SLEEP = 0x00,
    BME280_FORCED = 0x01,
    BME280_NORMAL = 0x03
} bme280_mode_t;

typedef enum
{
    AVAILABLE = 0x00,
    UPDATING = 0x01,
    MEASURING = 0x08
} bme280_measuring_status_t;

typedef enum
{
    T_0_5_MS = 0x00,
    T_62_5_MS = 0x01,
    T_125_MS = 0x02,
    T_250_MS = 0x03,
    T_500_MS = 0x04,
    T_10000_MS = 0x05,
    T_10_MS = 0x06,
    T_20_MS = 0x07
} bme280_timeStandby_t;

typedef enum
{
    FILTER_COEF_OFF = 0x00,
    FILTER_COEF_2 = 0x01,
    FILTER_COEF_4 = 0x02,
    FILTER_COEF_8 = 0x03,
    FILTER_COEF_16 = 0x04
} bme280_FilterCoef_t;

typedef enum
{
    BME280_REGISTER_DIG_T1 = 0x88,
    BME280_REGISTER_DIG_T2 = 0x8A,
    BME280_REGISTER_DIG_T3 = 0x8C,

    BME280_REGISTER_DIG_P1 = 0x8E,
    BME280_REGISTER_DIG_P2 = 0x90,
    BME280_REGISTER_DIG_P3 = 0x92,
    BME280_REGISTER_DIG_P4 = 0x94,
    BME280_REGISTER_DIG_P5 = 0x96,
    BME280_REGISTER_DIG_P6 = 0x98,
    BME280_REGISTER_DIG_P7 = 0x9A,
    BME280_REGISTER_DIG_P8 = 0x9C,
    BME280_REGISTER_DIG_P9 = 0x9E,

    BME280_REGISTER_DIG_H1 = 0xA1,
    BME280_REGISTER_DIG_H2 = 0xE1,
    BME280_REGISTER_DIG_H3 = 0xE3,
    BME280_REGISTER_DIG_H4 = 0xE4,
    BME280_REGISTER_DIG_H5 = 0xE5,
    BME280_REGISTER_DIG_H6 = 0xE7,

    BME280_REGISTER_CHIPID = 0xD0,
    BME280_REGISTER_VERSION = 0xD1,
    BME280_REGISTER_SOFTRESET = 0xE0,

    BME280_REGISTER_CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0

    BME280_REGISTER_CONTROLHUMID = 0xF2,
    BME280_REGISTER_STATUS = 0XF3,
    BME280_REGISTER_CONTROL = 0xF4,
    BME280_REGISTER_CONFIG = 0xF5,
    BME280_REGISTER_PRESSUREDATA = 0xF7,
    BME280_REGISTER_TEMPDATA = 0xFA,
    BME280_REGISTER_HUMIDDATA = 0xFD
} bme280_RegisterCoef_t;

typedef struct {
    uint16_t dig_T1; ///< temperature compensation value
    int16_t dig_T2;  ///< temperature compensation value
    int16_t dig_T3;  ///< temperature compensation value

    uint16_t dig_P1; ///< pressure compensation value
    int16_t dig_P2;  ///< pressure compensation value
    int16_t dig_P3;  ///< pressure compensation value
    int16_t dig_P4;  ///< pressure compensation value
    int16_t dig_P5;  ///< pressure compensation value
    int16_t dig_P6;  ///< pressure compensation value
    int16_t dig_P7;  ///< pressure compensation value
    int16_t dig_P8;  ///< pressure compensation value
    int16_t dig_P9;  ///< pressure compensation value

    uint8_t dig_H1; ///< humidity compensation value
    int16_t dig_H2; ///< humidity compensation value
    uint8_t dig_H3; ///< humidity compensation value
    int16_t dig_H4; ///< humidity compensation value
    int16_t dig_H5; ///< humidity compensation value
    int8_t dig_H6;  ///< humidity compensation value
} bme280_calib_data;
error_type_t bme280Init(bme280_t *bme280_object);
error_type_t bme280DeInit(bme280_t *bme280_object);
error_type_t bme280Destroy(bme280_t **bme280_object);
error_type_t bme280ReadData(bme280_t *bme280_object, float *temperature, float *pressure, float *humidity);


#ifdef __cplusplus
}
#endif

#endif
