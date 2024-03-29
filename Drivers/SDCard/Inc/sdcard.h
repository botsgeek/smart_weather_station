#ifndef __SDCARD_H__
#define __SDCARD_H__

#include <stdint.h>
#include <stdbool.h>
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#include "ff.h"
#include "fatfs.h"
#define SD_SPI_HANDLE hspi1

#ifdef __cplusplus
extern "C" {
#endif
typedef struct sdcard_t sdcard_t;
typedef struct {
    SPI_HandleTypeDef *spi_handle;
} sdcard_config_t;

typedef struct {
    float total_space_mb;
    float free_space_mb;
} sdcard_info_t;

typedef struct {
    uint8_t *buffer;
    uint32_t size;
} sdcard_data_t;


sdcard_t *sdcardCreate(const sdcard_config_t *config);
error_type_t sdcardInit(sdcard_t *sdcard_object);
error_type_t sdcardGetInfo(sdcard_t *sdcard_object,FATFS *fs, sdcard_info_t *info);
error_type_t sdcardCreateFile(sdcard_t *sdcard_object, const char *filename, FIL* file);
error_type_t sdcardReadFile(sdcard_t *sdcard_object, const sdcard_data_t *data, FIL* file);
error_type_t sdcardWriteFile(sdcard_t *sdcard_object, const sdcard_data_t *data, FIL* file);
error_type_t sdcardDeleteFile(sdcard_t *sdcard_object, const char *filename, FIL* file);
error_type_t sdcardCloseFile(sdcard_t *sdcard_object, FIL* file);
error_type_t sdcardDeInit(sdcard_t *sdcard_object);
error_type_t sdcardDestroy(sdcard_t **sdcard_object);
error_type_t sdcardFileMount(sdcard_t *sdcard_object,FATFS *fs);
error_type_t sdcardFileUnMount(sdcard_t *sdcard_object,FATFS *fs);
#ifdef __cplusplus
}
#endif

#endif 