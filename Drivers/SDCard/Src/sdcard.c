#include <sdcard.h>
#include <string.h>
#include <stdbool.h>
#include <common_headers.h>
#include "ff.h"
sdcard_t *sdcard_object;

struct sdcard_t {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    bool initialized;
};


    sdcard_t *sdcard_create(const sdcard_config_t *config) {
  
    if (config == NULL || config->spi_handle == NULL)
    {
        return NULL;
    }

   sdcard_t *sdcard_object = (sdcard_t *)malloc(sizeof(sdcard_t));
   if (sdcard_object == NULL) {
        return NULL;
    }

   sdcard_object->spi_handle = config->spi_handle;
   sdcard_object->cs_pin = config->cs_pin;
   sdcard_object->cs_port = config->cs_port;
   sdcard_object->initialized = false;

   return sdcard_object;
}

// Function to initialize the SD card
error_type_t sdcard_Init(sdcard_t *sdcard_object) {
//     FATFS FatFs; 	
//     ///FIL fil; 		
//     FRESULT fres; 

//   // Open the file system
//   fres = f_mount(&FatFs, "", 1); //1=mount now
//   if (fres != FR_OK) {
// 	printf("f_mount error (%i)\r\n", fres);
// 	while(1);
//   }
    // Implement the initialization process
    if (!sdcard_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }

    if (sdcard_object->cs_port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (sdcard_object->cs_port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (sdcard_object->cs_port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (sdcard_object->cs_port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
    else if (sdcard_object->cs_port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
    else if (sdcard_object->cs_port == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
    else
    {
        return SYSTEM_INVALID_PIN_PORT;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = sdcard_object->cs_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    // set pin high as default
    HAL_GPIO_WritePin(sdcard_object->cs_port, sdcard_object->cs_pin, GPIO_PIN_SET);
    // set pin high as default
    //sdcard_object->initialized = true;
    return SYSTEM_OK;
}


error_type_t sdcard_get_info(FATFS *fs, sdcard_info_t *info) {
    DWORD fre_clust, fre_sect, tot_sect;
    FRESULT res;

    if (!fs || !info) {
        return SYSTEM_NULL_PARAMETER;
    }

//     // Get free space
    res = f_getfree("", &fre_clust, &fs);
    if (res != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

//     // Calculate total sectors and free sectors
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

//     // Populate the info structure
    info->total_space_mb = ((tot_sect / 2)/1024);
    info->free_space_mb = ((fre_sect / 2)/1024);

    // info->total_space_mb = ((tot_sect * 512) / 1024) / 1024;
    // info->free_space_mb = ((fre_sect * 512) / 1024) / 1024;


   return SYSTEM_OK;
}


error_type_t sdcard_create_file(sdcard_t *sdcard_object, const char *filename, FIL* file) {
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

    // file_res = f_open(file, filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    // if (file_res != FR_OK) {
    //      if (file_res == FR_EXIST) {
    //         printf("Failed to create file '%s': File already exists\n", filename);
    //      } else {
    //     printf("Failed to open or create file '%s'\n", filename);
    //     }
    //     return SYSTEM_OPERATION_FAILED;
    // }


    return SYSTEM_OK;

}

// Function to read data from a file on the SD card
error_type_t sdcard_read_file(sdcard_t *sdcard_object, const sdcard_data_t *data,FIL* file){

    if (!sdcard_object || !file || !data) {
        return SYSTEM_NULL_PARAMETER;
    }


    FRESULT fres;

    // Read data from the file
    UINT bytesRead;
    fres = f_read(file, data->buffer, data->size, &bytesRead);
    if (fres != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;
}


error_type_t sdcard_write_file(sdcard_t *sdcard_object, const sdcard_data_t *data,FIL* file){
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

error_type_t sdcard_close_file(sdcard_t *sdcard_object, FIL* file) {
    if (!sdcard_object || !file) {
        return SYSTEM_NULL_PARAMETER;
    }

    // Close the file
    FRESULT fres = f_close(file);
    if (fres != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;
}
error_type_t sdcard_delete_file(sdcard_t *sdcard_object, const char *filename, FIL* file) {
    if (!sdcard_object || !filename || !file) {
        return SYSTEM_NULL_PARAMETER;
    }

    // Delete the file
    FRESULT fres = f_unlink(filename);
    if (fres != FR_OK) {
        return SYSTEM_OPERATION_FAILED;
    }

    return SYSTEM_OK;
}


// //Function to deinitialize the SD card
error_type_t sdcard_Deinit(sdcard_t *sdcard_object){
    
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

// // Function to destroy the sdcard_t structure
error_type_t sdcard_destroy(sdcard_t **sdcard_object) {
    // Implement the code to free the memory allocated for the sdcard_t structure
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

