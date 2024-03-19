#include <stdio.h>
#include <dht22.h>
#include <common_headers.h>
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

void example()
{
    error_type_t err;
    dht22_config_t dht22_config_object = {
        .gpio_port = GPIOA,
        .gpio_pin = GPIO_PIN_11};
    dht22_t *dht22_object = dht22Create(&dht22_config_object);
    if (!dht22_object)
    {
        exit(1);
    }
    err = dht22Init(dht22_object);
    if (err != SYSTEM_OK)
    {
        exit(1);
    }
    HAL_Delay(2000);
    dht22_data_t dht_data = {0.0, 0.0};
    err = dht22Read(dht22_object, &dht_data);
    if (err != SYSTEM_OK)
    {
        exit(1);
    }
    printf("humidity is: %.2f\n", dht_data.humidity);
    printf("temerature is: %.2f\n", dht_data.temperature);
    err = dht22DeInit(dht22_object);
    if (err != SYSTEM_OK)
    {
        exit(1);
    }
    err = dht22Init(dht22_object);
    if (err != SYSTEM_OK)
    {
        exit(1);
    }
    printf("deinitialized success\n");
    HAL_Delay(2000);
    dht_data.humidity = 0;
    dht_data.temperature = 0;
    err = dht22Read(dht22_object, &dht_data);
    if (err != SYSTEM_OK)
    {
        exit(1);
    }
    printf("humidity is: %.2f\n", dht_data.humidity);
    printf("temerature is: %.2f\n", dht_data.temperature);
    err = dht22DeInit(dht22_object);
    if (err != SYSTEM_OK)
    {
        exit(1);
    }
    err = dht22Destroy(&dht22_object);
    if (err == SYSTEM_OK)
    {
        exit(1);
    }
    printf("destroy success\n");
}
