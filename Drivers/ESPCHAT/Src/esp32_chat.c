#include <esp32_chat.h>
#include <stdbool.h>
#include <common_headers.h>
#include <string.h>
#include <at_commands.h>
#define SERIAL_SEND_TIMEOUT 100

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
        printf("failed here");
        return SYSTEM_FAILED;
    }
}

static bool checkCustomResponse(char* expected_response, char *receive_buffer, uint16_t counter_index)
{
    uint8_t response_length = strlen(expected_response);
    uint16_t counter_length = counter_index + 1;
    if (counter_length > response_length)
    {
        uint8_t response_index = response_length - 1;
        uint16_t local_counter = 0;
        while(local_counter <= response_index){
            if(expected_response[response_index-local_counter] != receive_buffer[counter_index-local_counter]){
                return false;
            }
            local_counter++;
        }
        printf("found expected result");
        return true;
    }
    else
    {
        return false;
    }
}


static error_type_t read(esp32_chat_t *esp32_chat_object, uint32_t timeout, esp32_chat_response_t* response)
{
    uint16_t counter = 0;
    uint32_t start_time = HAL_GetTick();
    while (counter < response->buffer_size - 1)
    {
        if (((HAL_GetTick()) - start_time) > timeout)
        {
            return SYSTEM_TIMED_OUT;
        }
        HAL_StatusTypeDef result =
            HAL_UART_Receive(esp32_chat_object->uart_object, response->buffer + (counter), 1, timeout);
        if (result != HAL_OK)
        {
                printf("failed here");
            return SYSTEM_FAILED;
        }else{
            start_time = HAL_GetTick();
        }
        if(response->expected_response){
            if (checkCustomResponse(response->expected_response,response->buffer, counter))
            {
                response->buffer[++counter] = 0;
                return SYSTEM_OK;
            }
        }
        else{
            if (checkCustomResponse(OK_RESPONSE,response->buffer, counter) ||
                checkCustomResponse(ERROR_RESPONSE,response->buffer, counter) ||
                checkCustomResponse(BUSY_RESPONSE,response->buffer, counter))
            {
                response->buffer[++counter] = 0;
                return SYSTEM_OK;
            }
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
error_type_t esp32ChatSendReceive(esp32_chat_t *esp32_chat_object, const char *message, esp32_chat_response_t* response )
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
    result = read(esp32_chat_object, (response->timeout)?response->timeout:SERIAL_SEND_TIMEOUT, response);
    if(result != SYSTEM_OK){
        return result;
    }
    if(response->expected_response){
        return SYSTEM_OK;
    }
    result = atCheckResponse(response->buffer);
    switch (result)
    {
    case AT_BUSY:
        return SYSTEM_BUSY;
    case AT_ERROR:
        return SYSTEM_FAILED;

    case AT_OK:
        return SYSTEM_OK;
    default:
        return SYSTEM_INVALID_RESPONSE;
    }
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