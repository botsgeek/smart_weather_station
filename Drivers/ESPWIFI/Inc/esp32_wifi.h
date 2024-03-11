#ifndef __esp32_WIFI_H___
#define __esp32_WIFI_H___
#include <stdint.h>
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#include <esp32_chat.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct esp32_wifi_t esp32_wifi_t;
    esp32_wifi_t *esp32WifiCreate(esp32_chat_t *esp32_chat_object);
    error_type_t esp32WifiInit(esp32_wifi_t *esp32_wifi_object);
    error_type_t esp32WifiConnect(esp32_wifi_t *esp32_wifi_object, char *SSID, char *password);
    error_type_t esp32WifiDisconnect(esp32_wifi_t *esp32_wifi_object);
    error_type_t esp32WifiIsconnected(esp32_wifi_t *esp32_wifi_object, bool *status);
    error_type_t esp32WifiDeInit(esp32_wifi_t *esp32_wifi_object);
    error_type_t esp32WifiDestroy(esp32_wifi_t **esp32_wifi_object);
#ifdef __cplusplus
}

#endif
#endif