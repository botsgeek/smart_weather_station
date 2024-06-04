
#include <esp32_chat.h>
#include <esp32_http.h>
#include <common_headers.h>
#include <esp32_wifi.h>
#define MAX_RETRY_COUNT 3
UART_HandleTypeDef huart2;
void http_example(){
  static char buffer[512];
  printf("program started\n");
  const char *SSID = "hyphenA";
  const char *password = "hyphen1234";
  uint8_t counter = 0;
  esp32_chat_config_t config = {
    .uart_object = &huart2
  };
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

  err = esp32WifiGetIPAddress(esp32_wifi_object,buffer);
  while (err != SYSTEM_OK){
    if (counter++ > MAX_RETRY_COUNT)
    {
      printf("esp wifi object disconnection  failed\n");
      exit(1);
    }
    HAL_Delay(1000);
    err = esp32WifiGetIPAddress(esp32_wifi_object,buffer);
  }
  counter = 0;
  printf("wifi ip address is: %s\n",buffer);
  HAL_Delay(1000);
  esp32_http_t* esp32_http_object = esp32HttpCreate(esp32_chat_object);
  if (!esp32_http_object)
  {
    printf(" esp32_http_object not created\n");
    exit(1);
  }
  err = esp32HttpInit(esp32_http_object);
  if(err != SYSTEM_OK){
      printf("esp wifi object disconnection  failed\n");
      exit(1);
  }
  uint32_t size_http = 0;
  char* url = "http://httpbin.org/get";
  err = esp32HttpGetSize(esp32_http_object,url,&size_http);
  while (err != SYSTEM_OK){
    if (counter++ > MAX_RETRY_COUNT)
    {
      printf("esp get size  failed\n");
      exit(1);
    }
    HAL_Delay(1000);
    err = esp32HttpGetSize(esp32_http_object,url,buffer);
  }
  size_http = atoi(buffer);
  printf("response size is: %d\n",size_http);
  counter = 0;
  http_request_t http_request = DEFAULT_HTTP_REQUEST;
  http_request.request_type = HTTP_GET;
  http_request.url = url;
  http_request.host = "httpbin.org";
  http_request.path = "/get";
  buffer[0] = '\0';
  err = esp32HttpSend(esp32_http_object,http_request,buffer);
  while (err != SYSTEM_OK){
    if (counter++ > MAX_RETRY_COUNT)
    {
      printf("esp http_send failed  failed\n");
      exit(1);
    }
    HAL_Delay(1000);
    err = esp32HttpSend(esp32_http_object,http_request,buffer);
  }
  counter = 0;
  char * payload = "{\n"
                          "   \"name\": \"Apple MacBook Pro 16\",\n"
                          "   \"data\": {\n"
                          "      \"year\": 2019,\n"
                          "      \"price\": 1849.99,\n"
                          "      \"CPU model\": \"Intel Core i9\",\n"
                          "      \"Hard disk size\": \"1 TB\"\n"
                          "   }\n"
                          "}";
char dummy[256];
sprintf(dummy,"2,\"Host: api.restful-api.dev\",\"content-type: application/json\"");
  esp32_http_post_request_t post_request = {.url = "https://api.restful-api.dev/objects",.payload_size = strlen(payload),.payload =payload,
  .additional_headers=dummy};
  err = esp32HttpPost(esp32_http_object,&post_request,buffer);
  while (err != SYSTEM_OK){
    if (counter++ > MAX_RETRY_COUNT)
    {
      printf("esp http post failed  failed\n");
      exit(1);
    }
    HAL_Delay(1000);
    err = esp32HttpPost(esp32_http_object,&post_request,buffer);
  }
  err = esp32HttpDeInit(esp32_http_object);
  if (err != SYSTEM_OK)
  {
    printf("esp http object deinit  failed\n");
    exit(1);
  }
  err = esp32HttpDestroy(&esp32_http_object);
  if (err != SYSTEM_OK)
  {
    printf("esp http object destroy  failed\n");
    exit(1);
  }
  counter = 0;
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
}