#include <sdcard.h>
#include <string.h>
#include <stdbool.h>
#include <common_headers.h>
#include "ff.h"
#include "fatfs.h"
sdcard_t *sdcard_object;

struct sdcard_t {
    SPI_HandleTypeDef* spi_handle;
    bool initialized;
};


    sdcard_t *sdcardCreate(const sdcard_config_t *config) {
  
    if (config == NULL || config->spi_handle == NULL)
    {
        return NULL;
    }

   sdcard_t *sdcard_object = (sdcard_t *)malloc(sizeof(sdcard_t));
   if (sdcard_object == NULL) {
        return NULL;
    }

   sdcard_object->spi_handle = config->spi_handle;
   sdcard_object->initialized = false;

   return sdcard_object;
}

error_type_t sdcardInit(sdcard_t *sdcard_object) {
    if (!sdcard_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }
  /* SPI1 parameter configuration*/
    MX_FATFS_Init();
    return SYSTEM_OK;
}


error_type_t sdcardGetInfo(sdcard_t *sdcard_object, FATFS *fs, sdcard_info_t *info){
     if (!sdcard_object || !info) {
        return SYSTEM_NULL_PARAMETER;
    }

    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res;

    if (!fs || !info) {
        return SYSTEM_NULL_PARAMETER;
    }


    res = f_getfree("", &fre_clust, &fs);
    if (res != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    info->total_space_mb = ((tot_sect / 2)/1024);
    info->free_space_mb = ((fre_sect / 2)/1024);


   return SYSTEM_OK;
}


error_type_t sdcardCreateFile(sdcard_t *sdcard_object, const char *filename, FIL* file) {
    // Implement the code to create a file on the SD card
     if (!sdcard_object || !filename || !file) {
        return SYSTEM_NULL_PARAMETER;
    }
    
    FRESULT file_res;

    // Open or create a file
    file_res = f_open(file, filename,  FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS | FA_READ);
    if (file_res != FR_OK) {
        // Handle error
        printf("Failed to open or create file: %s\n", filename);
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;

}


error_type_t sdcardReadFile(sdcard_t *sdcard_object, const sdcard_data_t *data, FIL* file){

    if (!sdcard_object || !file || !data) {
        return SYSTEM_NULL_PARAMETER;
    }
    FRESULT fres;

    UINT bytesRead;
    fres = f_read(file, data->buffer, data->size, &bytesRead);
    if (fres != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;
}

error_type_t sdcardWriteFile(sdcard_t *sdcard_object, const sdcard_data_t *data, FIL* file){
    if (!sdcard_object || !data ||!file ) {
        return SYSTEM_NULL_PARAMETER;
     }

    FRESULT fres_write;
    UINT bytesWritten;
    
    fres_write = f_write(file, data->buffer, data->size, &bytesWritten);
    if (fres_write != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;
 }

error_type_t sdcardCloseFile(sdcard_t *sdcard_object, FIL* file) {
    if (!sdcard_object || !file) {
        return SYSTEM_NULL_PARAMETER;
    }

 
    FRESULT fres = f_close(file);
    if (fres != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;
}
error_type_t sdcardDeleteFile(sdcard_t *sdcard_object, const char *filename, FIL* file) {
    if (!sdcard_object || !filename || !file) {
        return SYSTEM_NULL_PARAMETER;
    }

    FRESULT fres = f_unlink(filename);
    if (fres != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;
}

error_type_t sdcardDeInit(sdcard_t *sdcard_object){
    
    if (sdcard_object)
    {
        sdcard_object->initialized = false;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }

}

error_type_t sdcardDestroy(sdcard_t **sdcard_object) {
    if (*sdcard_object)
    {
        free(*sdcard_object);
        *sdcard_object = NULL;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}

error_type_t sdcardFileMount(sdcard_t *sdcard_object,FATFS *fs){
    if (!sdcard_object) {
        return SYSTEM_NULL_PARAMETER;
    }

    FRESULT res = f_mount(&fs, "", 1);
     if ( res != FR_OK)
    {
        printf("Failed to mount SD card\n");
        
        return SYSTEM_FAILED;
    }
    else
    HAL_Delay(2000);
    return SYSTEM_OK;

}

error_type_t sdcardFileUnMount(sdcard_t *sdcard_object,FATFS *fs){
     if (!sdcard_object) {
        return SYSTEM_NULL_PARAMETER;
    }

    FRESULT res = f_mount(NULL, "", 0);
     if ( res != FR_OK)
    {
    printf("Failed to unmount SD card\n");
    
    return SYSTEM_FAILED;

    }
    return SYSTEM_OK;



}
