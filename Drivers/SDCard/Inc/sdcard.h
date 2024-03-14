#ifndef __SDCARD_H__
#define __SDCARD_H__

#include <stdint.h>
#include <stdbool.h>
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#include "ff.h"
#include "fatfs.h"


#ifdef __cplusplus
extern "C" {
#endif
typedef struct sdcard_t sdcard_t;
typedef struct {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    //file object pointer FIL fil
} sdcard_config_t;

typedef struct {
    float total_space_mb;
    float free_space_mb;
} sdcard_info_t;

typedef struct {
    uint8_t *buffer;
    uint32_t size;
} sdcard_data_t;

//typedef struct sdcard_t sdcard_t;

sdcard_t *sdcard_create(const sdcard_config_t *config);
error_type_t sdcard_Init(sdcard_t *sdcard_object);
// error_type_t sdcard_get_info(const FATFS *fs, sdcard_info_t *info);
error_type_t sdcard_get_info(FATFS *fs, sdcard_info_t *info);
error_type_t sdcard_create_file(sdcard_t *sdcard_object, const char *filename, FIL* file);
error_type_t sdcard_read_file(sdcard_t *sdcard_object, const sdcard_data_t *data, FIL* file);
error_type_t sdcard_write_file(sdcard_t *sdcard_object, const sdcard_data_t *data, FIL* file);
error_type_t sdcard_delete_file(sdcard_t *sdcard_object, const char *filename, FIL* file);
error_type_t sdcard_close_file(sdcard_t *sdcard_object, FIL* file);
error_type_t sdcard_Deinit(sdcard_t *sdcard_object);
error_type_t sdcard_destroy(sdcard_t **sdcard_object);
//add API for file close
#ifdef __cplusplus
}
#endif

#endif 