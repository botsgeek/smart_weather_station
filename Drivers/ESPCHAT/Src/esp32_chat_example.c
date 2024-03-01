#include <esp32_chat.h>
#include <common_headers.h>
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

err = esp32ChatSendReceive(esp32_chat_object, message, buffer, 50);
if (err != SYSTEM_OK)
{
    printf("esp chat send received failed,error code is: %d\n", (int)err);
    exit(1);
}
printf("AT response is:\n %s\n", buffer);
err = esp32ChatDeInit(esp32_chat_object);
if (err != SYSTEM_OK)
{
    printf("esp chat deinit failed,error code is: %d\n", (int)err);
    exit(1);
}
err = esp32ChatSendReceive(esp32_chat_object, message, buffer, 50);
if (err == SYSTEM_OK)
{
    printf("esp chat should not send after deinit");
    exit(1);
}
err = esp32ChatDestroy(&esp32_chat_object);
if (err != SYSTEM_OK || esp32_chat_object != NULL)
{
    printf("esp chat destroy failed,error code is: %d\n", (int)err);
    exit(1);
}
err = esp32ChatSendReceive(esp32_chat_object, message, buffer, 50);
if (err == SYSTEM_OK)
{
    printf("esp chat should not send after destroy");
    exit(1);
}