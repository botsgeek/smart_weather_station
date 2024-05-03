#include <esp32_wifi.h>
#include <esp32_chat.h>
#include <common_headers.h>
#include <at_commands.h>
#include <stdbool.h>
#include <stdlib.h>
#include <re.h>
#define WIFI_MESSAGE_SEND_BUFFER_SIZE (255U)
#define MAX_SSID_LENGTH (32U)
#define MAX_PASSWORD_LENGTH (64U)
#define NO_WIFI_RESPONSE ("AT+CWJAP?\n\r\nOK")
#define IN_BETWEEN_ACTION_DELAY (1000U)
#define MINIMUM_SSID_LENGTH (3U)
#define MINIMUM_PASSWORD_LENGTH (8U)
#define MAXIMUM_SSID_LENGTH (32U)
#define MAXIMUM_PASSWORD_LENGTH (63U)
static char tx_buffer[WIFI_MESSAGE_SEND_BUFFER_SIZE] = "";
static char rx_buffer[WIFI_MESSAGE_SEND_BUFFER_SIZE] = "";
static char wifi_ssid[MAX_SSID_LENGTH] = "";
static char wifi_password[MAX_PASSWORD_LENGTH] = "";
static const char *wifi_pattern = "CWJAP:\"[^\"]*\",";
static const char *ip_pattern = "\"[0-9]*.[0-9]*.[0-9]*.[0-9]*\"";

static esp32_wifi_t *esp32_wifi_object_static = NULL;
struct esp32_wifi_t
{
    esp32_chat_t *esp32_chat_object;
    bool initialized;
};

static error_type_t checkConnected(esp32_wifi_t *esp32_wifi_object, bool *status)
{
    error_type_t result;
    int match_length;
    CLEAR_CHAT_BUFFER(tx_buffer);
    strcpy(tx_buffer, AT_CWJAP_GET_CMD);
    memset(rx_buffer, 0, WIFI_MESSAGE_SEND_BUFFER_SIZE);
    esp32_chat_response_t response = {.buffer = rx_buffer,.buffer_size = WIFI_MESSAGE_SEND_BUFFER_SIZE};
    result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer,&response);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    re_t pattern = re_compile(wifi_pattern);
    int match_idx = re_matchp(pattern, rx_buffer, &match_length);

    *status = (match_idx != -1) ? true : false;
    return SYSTEM_OK;
}

static error_type_t getIpAddress(esp32_wifi_t *esp32_wifi_object, char *ipAddress)
{
    error_type_t result;
    int match_length;
    CLEAR_CHAT_BUFFER(tx_buffer);
    strcpy(tx_buffer, AT_CIFSR_GET_CMD);
    memset(rx_buffer, 0, WIFI_MESSAGE_SEND_BUFFER_SIZE);
    esp32_chat_response_t response = {.buffer = rx_buffer,.buffer_size = WIFI_MESSAGE_SEND_BUFFER_SIZE};
    result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer, &response);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    re_t pattern = re_compile(ip_pattern);
    int match_idx = re_matchp(pattern, rx_buffer, &match_length);
    if(match_idx == -1)
    {
        return SYSTEM_FAILED;
    }
    char *end_index = strchr(rx_buffer+match_idx+1,'\"');
    if(!end_index){
       return SYSTEM_FAILED; 
    }
    size_t ip_size = (int)(end_index-(rx_buffer+match_idx+1));
    strncpy(ipAddress,rx_buffer+match_idx+1,ip_size);
    return SYSTEM_OK;
}

static error_type_t checkDisconnected(esp32_wifi_t *esp32_wifi_object, bool *status)
{
    error_type_t result;
    CLEAR_CHAT_BUFFER(tx_buffer);
    strcpy(tx_buffer, AT_CWJAP_GET_CMD);
    memset(rx_buffer, 0, WIFI_MESSAGE_SEND_BUFFER_SIZE);
    esp32_chat_response_t response = {.buffer = rx_buffer,.buffer_size = WIFI_MESSAGE_SEND_BUFFER_SIZE};
    result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer, &response);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    *status = strstr(rx_buffer, NO_WIFI_RESPONSE) ? true : false;
    return SYSTEM_OK;
}

// This is a singleton
esp32_wifi_t *esp32WifiCreate(esp32_chat_t *esp32_chat_object)
{
    if (!esp32_wifi_object_static)
    {
        esp32_wifi_t *esp32_wifi_object = (esp32_wifi_t *)malloc(sizeof(esp32_wifi_t));
        if(!esp32_wifi_object){
            return NULL;
        }
        esp32_wifi_object->esp32_chat_object = esp32_chat_object;
        esp32_wifi_object_static = esp32_wifi_object;
        return esp32_wifi_object;
    }
    return esp32_wifi_object_static;
}

error_type_t esp32WifiInit(esp32_wifi_t *esp32_wifi_object)
{
    error_type_t result;
    if (esp32_wifi_object == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (esp32_wifi_object->esp32_chat_object == NULL)
    {
        return SYSTEM_INVALID_PARAMETER;
    }
    CLEAR_CHAT_BUFFER(tx_buffer);
    AT_CWMODE_SET_CMD(tx_buffer, WIFI_NULL_MODE);
    esp32_chat_response_t response = {.buffer = rx_buffer,.buffer_size = WIFI_MESSAGE_SEND_BUFFER_SIZE};
    result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer,&response);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    CLEAR_CHAT_BUFFER(tx_buffer);
    AT_CWMODE_SET_CMD(tx_buffer, WIFI_STATION_MODE);
    result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer,&response);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    esp32_wifi_object->initialized = true;
    return SYSTEM_OK;
}
static error_type_t validate(esp32_wifi_t *esp32_wifi_object)
{
    if (esp32_wifi_object == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!esp32_wifi_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    return SYSTEM_OK;
}
error_type_t esp32WifiConnect(esp32_wifi_t *esp32_wifi_object, char *SSID, char *password)
{
    error_type_t result;
    result = validate(esp32_wifi_object);
    bool connected = false;
    if (result != SYSTEM_OK)
    {
        return result;
    }
    if (strlen(SSID) < MINIMUM_SSID_LENGTH || strlen(password) < MINIMUM_PASSWORD_LENGTH ||
        strlen(SSID) > MAXIMUM_SSID_LENGTH || strlen(password) > MAXIMUM_PASSWORD_LENGTH)
    {
        return SYSTEM_INVALID_LENGTH;
    }
    if (checkConnected(esp32_wifi_object, &connected) != SYSTEM_OK)
    {
        return result;
    }
    if (connected)
    {
        memcpy(wifi_ssid, SSID, strlen(SSID) + 1);
        memcpy(wifi_password, password, strlen(password) + 1);
        return SYSTEM_OK;
    }
    CLEAR_CHAT_BUFFER(tx_buffer);
    AT_CWJAP_SET_CMD(tx_buffer, SSID, password);
    esp32_chat_response_t response = {.buffer = rx_buffer,.buffer_size = WIFI_MESSAGE_SEND_BUFFER_SIZE};
    result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer,&response);
    if (result == SYSTEM_FAILED)
    {
        HAL_Delay(IN_BETWEEN_ACTION_DELAY);
        result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer, &response);
    }
    if (result == SYSTEM_OK)
    {
        memcpy(wifi_ssid, SSID, strlen(SSID) + 1);
        memcpy(wifi_password, password, strlen(password) + 1);
    }
    return result;
}
error_type_t esp32WifiDisconnect(esp32_wifi_t *esp32_wifi_object)
{
    error_type_t result;
    result = validate(esp32_wifi_object);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    bool connected = false;
    if (checkConnected(esp32_wifi_object, &connected) != SYSTEM_OK)
    {
        return result;
    }
    if (!connected)
    {
        return SYSTEM_OK;
    }
    CLEAR_CHAT_BUFFER(tx_buffer);
    strcpy(tx_buffer, AT_CWQAP_SET_CMD);
    esp32_chat_response_t response = {.buffer = rx_buffer,.buffer_size = WIFI_MESSAGE_SEND_BUFFER_SIZE};
    result = esp32ChatSendReceive(esp32_wifi_object->esp32_chat_object, tx_buffer, &response);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    return SYSTEM_OK;
}
error_type_t esp32WifiGetIPAddress(esp32_wifi_t *esp32_wifi_object, char *ip_address_buffer)
{
    error_type_t result;
    result = validate(esp32_wifi_object);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    return getIpAddress(esp32_wifi_object,ip_address_buffer);   
}
error_type_t esp32WifiIsconnected(esp32_wifi_t *esp32_wifi_object, bool *status)
{
    error_type_t result;
    result = validate(esp32_wifi_object);
    if (result != SYSTEM_OK)
    {
        return result;
    }
    return checkConnected(esp32_wifi_object, status);
}
error_type_t esp32WifiDeInit(esp32_wifi_t *esp32_wifi_object)
{
    if (esp32_wifi_object)
    {
        esp32_wifi_object->initialized = false;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}
error_type_t esp32WifiDestroy(esp32_wifi_t **esp32_wifi_object)
{
    if (*esp32_wifi_object)
    {
        free(*esp32_wifi_object);
        *esp32_wifi_object = NULL;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}