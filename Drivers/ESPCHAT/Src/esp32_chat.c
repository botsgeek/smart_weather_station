#include <esp32_chat.h>
#include <stdbool.h>
#include <common_headers.h>
#include <string.h>
#define ERROR_RESPONSE ("\r\nERROR")
#define OK_RESPONSE ("\r\nOK")
#define BUSY_RESPONSE ("busy P...")
#define SERIAL_SEND_TIMEOUT 100

esp32_chat_config_t DEFAULT_ESPCHAT_CONFIG = {
    .uart_object = NULL,
    .instance = USART2,
    .baud = 115200,
    .word_length = UART_WORDLENGTH_8B,
    .stop_bit = UART_STOPBITS_1,
    .parity = UART_PARITY_NONE,
    .mode = UART_MODE_TX_RX,
    .hw_flow_ctrl = UART_HWCONTROL_NONE,
    .oversampling = UART_OVERSAMPLING_16

};
struct esp32_chat_t
{
    UART_HandleTypeDef *uart_object;
    bool initialized;
};

error_type_t esp32ChatInit(esp32_chat_t *esp32_chat_object)
{
    if (!esp32_chat_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (HAL_UART_Init(esp32_chat_object->uart_object) != HAL_OK)
    {
        Error_Handler();
        return SYSTEM_FAILED;
    }
    esp32_chat_object->initialized = true;
    return SYSTEM_OK;
}
esp32_chat_t *esp32ChatCreate(const esp32_chat_config_t *config)
{
    if (!config)
    {
        return NULL;
    }
    esp32_chat_t *esp32_chat_object = (esp32_chat_t *)malloc(sizeof(esp32_chat_t));
    esp32_chat_object->uart_object = config->uart_object;
    esp32_chat_object->uart_object->Instance = config->instance;
    esp32_chat_object->uart_object->Init.BaudRate = config->baud;
    esp32_chat_object->uart_object->Init.WordLength = config->word_length;
    esp32_chat_object->uart_object->Init.StopBits = config->stop_bit;
    esp32_chat_object->uart_object->Init.Parity = config->parity;
    esp32_chat_object->uart_object->Init.Mode = config->mode;
    esp32_chat_object->uart_object->Init.HwFlowCtl = config->hw_flow_ctrl;
    esp32_chat_object->uart_object->Init.OverSampling = config->oversampling;
    return esp32_chat_object;
}

error_type_t esp32ChatDeInit(esp32_chat_t *esp32_chat_object)
{
    if (esp32_chat_object)
    {
        esp32_chat_object->initialized = false;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}

static error_type_t write(esp32_chat_t *esp32_chat_object, const char *message)
{
    if (!message)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    HAL_StatusTypeDef result =
        HAL_UART_Transmit(esp32_chat_object->uart_object, (uint8_t *)message, strlen(message), SERIAL_SEND_TIMEOUT);
    if (result == HAL_OK)
    {
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_FAILED;
    }
}
static bool checkOk(char *receive_buffer, uint16_t counter)
{
    static uint8_t ok_length = strlen(OK_RESPONSE);
    if (counter > ok_length)
    {
        uint16_t offset = (counter - ok_length);
        if (memcmp((void *)OK_RESPONSE, (void *)(receive_buffer + offset), ok_length) == 0)
        {
            printf("OK found\n");
            receive_buffer[counter++] = '\0';
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

static bool checkError(char *receive_buffer, uint16_t counter)
{
    static uint8_t error_length = strlen(ERROR_RESPONSE);
    if (counter > error_length)
    {
        uint16_t offset = (counter - error_length);
        if (memcmp((void *)ERROR_RESPONSE, (void *)(receive_buffer + offset), error_length) == 0)
        {
            printf("ERROR found\n");
            receive_buffer[counter++] = '\0';
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

static bool checkBusy(char *receive_buffer, uint16_t counter)
{
    static uint8_t busy_length = strlen(BUSY_RESPONSE);
    if (counter > busy_length)
    {
        uint16_t offset = (counter - busy_length);
        if (memcmp((void *)BUSY_RESPONSE, (void *)(receive_buffer + offset), busy_length) == 0)
        {
            printf("BUSY found\n");
            receive_buffer[counter++] = '\0';
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

static error_type_t read(esp32_chat_t *esp32_chat_object, uint32_t timeout, char *buffer, uint16_t buffer_size)
{
    uint16_t counter = 0;
    uint32_t start_time = HAL_GetTick();
    while (counter < buffer_size - 1)
    {
        if (((HAL_GetTick()) - start_time) > timeout)
        {
            return SYSTEM_TIMED_OUT;
        }
        HAL_StatusTypeDef result =
            HAL_UART_Receive(esp32_chat_object->uart_object, buffer + (counter), 1, timeout);
        if (result != HAL_OK)
        {
            return SYSTEM_FAILED;
        }
        if (checkOk(buffer, counter) || checkError(buffer, counter) || checkBusy(buffer, counter))
        {
            return SYSTEM_OK;
        }
        counter++;
    }
    return SYSTEM_BUFFER_OVERFLOW;
}

static error_type_t validate(esp32_chat_t *esp32_chat_object)
{
    if (esp32_chat_object == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!esp32_chat_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    return SYSTEM_OK;
}
error_type_t esp32ChatSendReceive(esp32_chat_t *esp32_chat_object, const char *message, char *response, uint16_t response_size)
{
    error_type_t result;
    result = validate(esp32_chat_object);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    result = write(esp32_chat_object, message);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    return read(esp32_chat_object, SERIAL_SEND_TIMEOUT, response, response_size);
}
error_type_t esp32ChatSendAsync(esp32_chat_t *esp32_chat_object, const char *message, void (*callback)(char *))
{
    error_type_t result;
    result = validate(esp32_chat_object);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    return SYSTEM_OK;
}

error_type_t esp32ChatDestroy(esp32_chat_t **esp32_chat_object)
{
    if (*esp32_chat_object)
    {
        free(*esp32_chat_object);
        *esp32_chat_object = NULL;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}