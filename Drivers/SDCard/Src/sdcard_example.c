#include <stdio.h>
#include <sdcard.h>
#include <common_headers.h>

void example()
{
   sdcard_config_t sdcard_config = {
    .spi_handle = &hspi1,
   
  };
//Create the object
  sdcard_t *sdcard_object = sdcardCreate(&sdcard_config);
  if (!sdcard_object) {
    printf("Failed to configure SD card instance\n");
    exit(1);
  }
//Initialize the sdcard 
  error_type_t err = sdcardInit(sdcard_object);
  if (err != SYSTEM_OK) {
    printf("Failed to initialize SD card\n");
    exit(1);
  }

  FATFS fs;
 
  // Mount the SD card
  if (f_mount(&fs, "", 1) != FR_OK)
  {
    printf("Failed to mount SD card\n");
    exit(1);
    //return 1;
  }

  // Get SD card information
  sdcard_info_t sdcard_info;
  error_type_t result = sdcardGetInfo(sdcard_object,&fs, &sdcard_info);
  if (result != SYSTEM_OK) {
    printf("Failed to get SD card information\n");
    exit(1);
  }

  // // Print the SD card information
  printf("Total space: %2f MB\n", sdcard_info.total_space_mb);
  printf("Free space: %2f MB\n", sdcard_info.free_space_mb);

 // Create a file if it doesn't exist

  FIL fil;
  const char *filename = "test.txt";
 // err = sdcard_create_file(sdcard_object, filename, &fil);
 err = sdcardCreateFile(sdcard_object, filename, &fil);
  if (err != SYSTEM_OK) {
    printf("Failed to create file\n");
    exit(1);
  }

  // Write to the file
  const char *write_data = "Hello, SD card!";
  sdcard_data_t data = {
    .buffer = (uint8_t*)write_data,
    .size = strlen(write_data)
  };
  err = sdcardWriteFile(sdcard_object, &data, &fil);
  if (err != SYSTEM_OK) {
    printf("Failed to write to file\n");
    exit(1);
  }

  err = sdcardReadFile(sdcard_object, &data, &fil);
  if (err != SYSTEM_OK) {
    printf("Failed to read file\n");
    free(data.buffer);
    //f_close(&fil);
    exit(1);
  }

    // Print the read data
  printf("Read data: %s\n", data.buffer);

     // Close the file
  err = sdcardCloseFile(sdcard_object, &fil);
  if (err != SYSTEM_OK) {
    printf("Failed to close file\n");
    exit(1);
  }

  // Delete the file
  err = sdcardDeleteFile(sdcard_object, filename, &fil);
  if (err != SYSTEM_OK) {
    printf("Failed to delete file\n");
    exit(1);
  }

   /* Deinitialize the SD card */
  err = sdcardDeInit(sdcard_object);
  if (err != SYSTEM_OK) {
    printf("Failed to deinitialize SD card\n");
    exit(1);
  }

  /* Destroy the SD card object */
  err = sdcardDestroy(&sdcard_object);
  if (err != SYSTEM_OK) {
    printf("Failed to destroy SD card object\n");
    exit(1);
  }

    // Unmount the SD card
  f_mount(NULL, "", 0);
}