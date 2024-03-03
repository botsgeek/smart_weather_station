#include "sdcard.h"
#include <stdbool.h>
#include <common_headers.h>


struct sdcard_t
{
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
    bool initialized;
};

error_type_t sdcardInit(sdcard_t *sdcard_object)
{
    if (!sdcard_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }

    if (sdcard_object->gpio_port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (sdcard_object->gpio_port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (sdcard_object->gpio_port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (sdcard_object->gpio_port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
    else if (sdcard_object->gpio_port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
    else if (sdcard_object->gpio_port == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
    else
    {
        return SYSTEM_INVALID_PIN_PORT;
    }

    // GPIO_InitTypeDef GPIO_InitStruct = {0};

    // GPIO_InitStruct.Pin = _object->gpio_pin;
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    // GPIO_InitStruct.Pull = GPIO_PULLUP;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    // // set pin high as default
    // HAL_GPIO_WritePin(dht22_object->gpio_port, dht22_object->gpio_pin, GPIO_PIN_SET);
    // // set pin high as default
    // dht22_object->initialized = true;
    // return SYSTEM_OK;
}

sdcard_t *sdcardCreate(const sdcard_config_t *config)
{
    if (!config)
    {
        return NULL;
    }
    sdcard_t *sdcard_object = (sdcard_t *)malloc(sizeof(dht22_t));
    sdcard_object->gpio_pin = config->gpio_pin;
    sdcard_object->gpio_port = config->gpio_port;
    return sdcard_object;
}

error_type_t dht22Read(dht22_t *dht22_object, dht22_data_t *data)
{
    error_type_t err;
    if (dht22_object == NULL || data == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!dht22_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    trigger(dht22_object);
    err = detectAck(dht22_object);
    if (err != SYSTEM_OK)
    {
        return err;
    }
    uint8_t bytes[BYTES_IN_DATA] = {0};
    err = getData(dht22_object, bytes);
    if (err != SYSTEM_OK)
    {
        return err;
    }
    err = validateCheckSum(bytes);
    if (err != SYSTEM_OK)
    {
        return err;
    }
    convert_value(bytes, data);
    return SYSTEM_OK;
}


error_type_t dht22Read(dht22_t *dht22_object, dht22_data_t *data)
{
    error_type_t err;
    if (dht22_object == NULL || data == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!dht22_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    trigger(dht22_object);
    err = detectAck(dht22_object);
    if (err != SYSTEM_OK)
    {
        return err;
    }
    uint8_t bytes[BYTES_IN_DATA] = {0};
    err = getData(dht22_object, bytes);
    if (err != SYSTEM_OK)
    {
        return err;
    }
    err = validateCheckSum(bytes);
    if (err != SYSTEM_OK)
    {
        return err;
    }
    convert_value(bytes, data);
    return SYSTEM_OK;
}


error_type_t dht22Destroy(dht22_t **dht22_object)
{
    if (*dht22_object)
    {
        free(*dht22_object);
        *dht22_object = NULL;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}

error_type_t dht22DeInit(dht22_t *dht22_object)
{
    if (dht22_object)
    {
        dht22_object->initialized = false;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}