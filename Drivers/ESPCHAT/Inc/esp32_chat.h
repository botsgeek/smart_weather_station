#ifndef __esp32_CHAT_H___
#define __esp32_CHAT_H___
#include <stdint.h>
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct esp32_chat_t esp32_chat_t;
    typedef struct
    {
        UART_HandleTypeDef* uart_object;
    } esp32_chat_config_t;
    esp32_chat_t *esp32ChatCreate(const esp32_chat_config_t *config);
    error_type_t esp32ChatInit(esp32_chat_t *esp32_chat_object);
    error_type_t esp32ChatSendReceive(esp32_chat_t *esp32_chat_object, const char *message, char *response, uint16_t response_size);
    error_type_t esp32ChatSendAsync(esp32_chat_t *esp32_chat_object, const char *message, void (*callback)(char *));
    error_type_t esp32ChatDeInit(esp32_chat_t *esp32_chat_object);
    error_type_t esp32ChatDestroy(esp32_chat_t **esp32_chat_object);
#ifdef __cplusplus
}
#endif
#endif