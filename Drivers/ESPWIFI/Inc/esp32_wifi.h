/**
 * @file esp32_wifi.h
 * @author Rilwan (rsaliu@botsgeeks.com)
 * @brief
 * @version 0.1
 * @date 2024-03-11
 *
 * @copyright Copyright (c) 2024
 *
 *
 * This library enables the stm32 board to connect to WiFi using AT commands
 * with as esp32 over UART
 */
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

    /**
     * @brief esp32_wifi_t object shadow
     *
     */
    typedef struct esp32_wifi_t esp32_wifi_t;
    /**
     * @brief Creates wifi object
     *
     * @param esp32_chat_object
     * @return esp32_wifi_t*
     */
    esp32_wifi_t *esp32WifiCreate(esp32_chat_t *esp32_chat_object);
    /**
     * @brief Initializes WiFi
     *
     * @param esp32_wifi_object
     * @return error_type_t
     */
    error_type_t esp32WifiInit(esp32_wifi_t *esp32_wifi_object);
    /**
     * @brief Connects to Wifi
     *
     * @param esp32_wifi_object
     * @param SSID
     * @param password
     * @return error_type_t
     */
    error_type_t esp32WifiConnect(esp32_wifi_t *esp32_wifi_object, char *SSID, char *password);
    /**
     * @brief Disconnects WiFi
     *
     * @param esp32_wifi_object
     * @return error_type_t
     */
    error_type_t esp32WifiDisconnect(esp32_wifi_t *esp32_wifi_object);
    /**
     * @brief Check if wifi is connected
     *
     * @param esp32_wifi_object
     * @param status
     * @return error_type_t
     */
    error_type_t esp32WifiIsconnected(esp32_wifi_t *esp32_wifi_object, bool *status);
    /**
     * @brief Get IP Address
     *
     * @param esp32_wifi_object
     * @param ip_address_buffer
     * @return error_type_t
     */
    error_type_t esp32WifiGetIPAddress(esp32_wifi_t *esp32_wifi_object, char *ip_address_buffer);
    /**
     * @brief Deinitializes wifi object
     *
     * @param esp32_wifi_object
     * @return error_type_t
     */
    error_type_t esp32WifiDeInit(esp32_wifi_t *esp32_wifi_object);
    /**
     * @brief destroys wifi object
     *
     * @param esp32_wifi_object
     * @return error_type_t
     */
    error_type_t esp32WifiDestroy(esp32_wifi_t **esp32_wifi_object);
#ifdef __cplusplus
}

#endif
#endif