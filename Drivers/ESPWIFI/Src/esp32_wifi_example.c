
#include <common_headers.h>
#include <esp32_chat.h>
#include <esp32_wifi.h>
const char *SSID = "SMC";
const char *password = "smc12345";
uint8_t counter = 0;
esp32_chat_config_t config = DEFAULT_ESPCHAT_CONFIG;
config.uart_object = &huart2;
esp32_chat_t *esp32_chat_object = esp32ChatCreate(&config);
char *message = "AT\n";
if (!esp32_chat_object)
{
    printf("esp chat object not created\n");
    exit(1);
}
error_type_t err = esp32ChatInit(esp32_chat_object);
if (err != SYSTEM_OK)
{
    printf("esp chat object init failed\n");
    exit(1);
}
esp32_wifi_t *esp32_wifi_object = esp32WifiCreate(esp32_chat_object);
err = esp32WifiInit(esp32_wifi_object);
while (err != SYSTEM_OK)
{
    if (counter++ > MAX_RETRY_COUNT)
    {
        printf("esp wifi object init failed\n");
        exit(1);
    }
    HAL_Delay(1000);
    err = esp32WifiInit(esp32_wifi_object);
}
counter = 0;
HAL_Delay(1000);
err = esp32WifiConnect(esp32_wifi_object, SSID, password);
while (err == SYSTEM_FAILED)
{
    if (counter++ > MAX_RETRY_COUNT)
    {
        printf("esp wifi object connect failed\n");
        exit(1);
    }
    HAL_Delay(1000);
    err = esp32WifiConnect(esp32_wifi_object, SSID, password);
}
HAL_Delay(1000);
bool is_connected = false;
counter = 0;
err = esp32WifiIsconnected(esp32_wifi_object, &is_connected);
while (err != SYSTEM_OK)
{
    if (counter++ > MAX_RETRY_COUNT)
    {
        printf("esp wifi object connection check failed\n");
        exit(1);
    }
    HAL_Delay(2000);
    err = esp32WifiIsconnected(esp32_wifi_object, &is_connected);
}
if (!is_connected)
{
    printf("esp wifi object connection failed\n");
    exit(1);
}
counter = 0;
printf("wifi is connected\n");
HAL_Delay(1000);
err = esp32WifiDisconnect(esp32_wifi_object);
while (err != SYSTEM_OK)
{
    if (counter++ > MAX_RETRY_COUNT)
    {
        printf("esp wifi object disconnection  failed\n");
        exit(1);
    }
    HAL_Delay(1000);
    err = esp32WifiDisconnect(esp32_wifi_object);
}
counter = 0;
err = esp32WifiIsconnected(esp32_wifi_object, &is_connected);
while (err != SYSTEM_OK)
{
    if (counter++ > MAX_RETRY_COUNT)
    {
        printf("esp wifi object connection check failed\n");
        exit(1);
    }
    HAL_Delay(1000);
    err = esp32WifiIsconnected(esp32_wifi_object, &is_connected);
}
if (is_connected)
{
    printf("esp wifi object connection failed\n");
    exit(1);
}

err = esp32WifiDeInit(esp32_wifi_object);
if (err != SYSTEM_OK)
{
    printf("esp wifi object deinit  failed\n");
    exit(1);
}
err = esp32WifiDestroy(&esp32_wifi_object);
if (err != SYSTEM_OK)
{
    printf("esp wifi object destroy  failed\n");
    exit(1);
}
if (esp32_wifi_object != NULL)
{
    printf("esp wifi object not set to null\n");
    exit(1);
}