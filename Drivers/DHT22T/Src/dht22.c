#include <dht22.h>
#include <stdbool.h>
#include <common_headers.h>
#define FIRST_TRIGGER_TIME 18000
#define SECOND_TRIGGER_TIME 40
#define ACK_WAIT_TIME 80
#define BEFORE_DATA_WAIT_TIME 50
#define TIMEOUT_TICK 1
#define TIMEOUT_PERIOD 1000
#define ON_PERIOD 28
#define MAX_ON_PERIOD 70
#define ALLOWANCE 5
#define BYTES_IN_DATA 5
#define BYTE_SIZE 8
#define TOTAL_DATA_LENGTH 40

struct dht22_t
{
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
    bool initialized;
};

error_type_t dht22Init(dht22_t *dht22_object)
{
    if (!dht22_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }

    if (dht22_object->gpio_port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (dht22_object->gpio_port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (dht22_object->gpio_port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (dht22_object->gpio_port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
    else if (dht22_object->gpio_port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
    else if (dht22_object->gpio_port == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
    else
    {
        return SYSTEM_INVALID_PIN_PORT;
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = dht22_object->gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    // set pin high as default
    HAL_GPIO_WritePin(dht22_object->gpio_port, dht22_object->gpio_pin, GPIO_PIN_SET);
    // set pin high as default
    dht22_object->initialized = true;
    return SYSTEM_OK;
}
dht22_t *dht22Create(const dht22_config_t *config)
{
    if (!config)
    {
        return NULL;
    }
    dht22_t *dht22_object = (dht22_t *)malloc(sizeof(dht22_t));
    dht22_object->gpio_pin = config->gpio_pin;
    dht22_object->gpio_port = config->gpio_port;
    return dht22_object;
}
static error_type_t detectAck(dht22_t *dht22_object)
{
    bool low_ack = false;
    bool high_ack = false;
    delayUs(((ACK_WAIT_TIME / 2) + ALLOWANCE));
    if (HAL_GPIO_ReadPin(dht22_object->gpio_port, dht22_object->gpio_pin) == GPIO_PIN_RESET)
    {
        low_ack = true;
    }
    delayUs(ACK_WAIT_TIME);
    if (HAL_GPIO_ReadPin(dht22_object->gpio_port, dht22_object->gpio_pin) == GPIO_PIN_SET)
    {
        high_ack = true;
    }
    return ((low_ack && high_ack)) ? SYSTEM_OK : SYSTEM_NO_ACK;
}

static void trigger(dht22_t *dht22_object)
{
    HAL_GPIO_WritePin(dht22_object->gpio_port, dht22_object->gpio_pin, GPIO_PIN_RESET);
    delayUs(FIRST_TRIGGER_TIME + ALLOWANCE);
    HAL_GPIO_WritePin(dht22_object->gpio_port, dht22_object->gpio_pin, GPIO_PIN_SET);
    delayUs(SECOND_TRIGGER_TIME - ALLOWANCE);
}

static error_type_t getData(dht22_t *dht22_object, uint8_t *bytes)
{
    uint8_t data_count = 0;
    for (uint8_t x = 0; x < BYTES_IN_DATA; x++)
    {
        uint8_t single_byte = 0;
        for (int k = (BYTE_SIZE - 1); k >= 0; k--)
        {
            volatile uint8_t bit_data = 0;
            volatile uint16_t counter = 0;
            while (HAL_GPIO_ReadPin(dht22_object->gpio_port, dht22_object->gpio_pin) != GPIO_PIN_RESET)
            {
                delayUs(TIMEOUT_TICK);
                counter++;
                if (counter >= TIMEOUT_PERIOD)
                {
                    return SYSTEM_NO_RESPONSE;
                }
            }
            counter = 0;
            while (HAL_GPIO_ReadPin(dht22_object->gpio_port, dht22_object->gpio_pin) != GPIO_PIN_SET)
            {
                delayUs(TIMEOUT_TICK);
                counter++;
                if (counter >= (BEFORE_DATA_WAIT_TIME + ALLOWANCE))
                {
                    return SYSTEM_NO_RESPONSE;
                }
            }
            counter = 0;
            uint16_t time_before = __HAL_TIM_GET_COUNTER(&htim3);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
            while (HAL_GPIO_ReadPin(dht22_object->gpio_port, dht22_object->gpio_pin) != GPIO_PIN_RESET)
            {
                if ((__HAL_TIM_GET_COUNTER(&htim3) - time_before) >= (MAX_ON_PERIOD + ALLOWANCE))
                {
                    return SYSTEM_NO_RESPONSE;
                }
            }
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            uint16_t diff = __HAL_TIM_GET_COUNTER(&htim3) - time_before;
            if (diff >= (ON_PERIOD + ALLOWANCE))
            {
                bit_data = 1;
            }
            counter = 0;
            single_byte = single_byte | (bit_data << k);
            data_count++;
        }
        bytes[x] = single_byte;
    }

    return SYSTEM_OK;
}

static error_type_t validateCheckSum(uint8_t *data)
{
    uint16_t sum = 0;
    for (uint8_t x = 0; x < BYTES_IN_DATA - 1; x++)
    {
        sum += data[x];
    }
    printf("sum is: %d\n", sum);
    printf("rounded sum is: %d\n", (uint8_t)sum);
    printf("checksum is: %d\n", data[BYTES_IN_DATA - 1]);
    if (((uint8_t)sum) == data[BYTES_IN_DATA - 1])
    {
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_CHECK_SUM_VALIDATION_FAILED;
    }
}

void convert_value(uint8_t *raw_data, dht22_data_t *data)
{
    static const uint8_t shift_value = 8;
    static const uint8_t divisor = 10;
    uint16_t sixteen_bit_val = (raw_data[0] << shift_value) | raw_data[1];
    float temp = ((float)sixteen_bit_val / divisor);
    data->humidity = temp;
    sixteen_bit_val = (raw_data[2] << shift_value) | raw_data[3];
    temp = ((float)sixteen_bit_val / divisor);
    data->temperature = temp;
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