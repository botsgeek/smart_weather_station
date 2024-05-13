#include <stdio.h>
#include <bme280_spi.h>
#include <common_headers.h>

void example()
{
    bme280_config_t config = {
        .spi_handle = &hspi1,
        .gpio_port = GPIOA,     // Update with your actual GPIO port
        .gpio_pin = GPIO_PIN_4, // Update with your actual GPIO pin
    };

    bme280_t *bme280_object = bme280Create(&config);
    if (!bme280_object)
    {
        printf("Failed to configure bme280 instance\n");
        exit(1);
    }

    error_type_t err = bme280Init(bme280_object);
    if (err != SYSTEM_OK)
    {
        printf("Failed to initialize bme280 \n");
        exit(1);
    }

    float temperature, pressure, humidity;
    err = bme280ReadData(bme280_object, &temperature, &pressure, &humidity);
    if (err != SYSTEM_OK)
    {
        printf("Failed to Read bme280 \n");
        exit(1);
    }
    printf("Temperature: %.2f C\n", temperature);
    printf("Pressure: %.2f hPa\n", pressure);
    printf("Humidity: %.2f %%\n", humidity);

    HAL_Delay(1000); // Delay for 1 second (adjust as needed)

    err = bme280DeInit(bme280_object);
    if (err != SYSTEM_OK)
    {
        printf("Failed to deinitialize bme280\n");
        exit(1);
    }

    err = bme280Destroy(&bme280_object);
    if (err == SYSTEM_OK)
    {
        printf("Failed to destroy bme280\n");
        exit(1);
    }
    printf("destroy success\n");

    // Delay for some time
    HAL_Delay(1000); // Delay for 1 second (adjust as needed)
}