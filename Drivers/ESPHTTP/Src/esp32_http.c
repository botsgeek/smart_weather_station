#include <esp32_http.h>
#include <esp32_chat.h>
#include <common_headers.h>
#include <at_commands.h>
#include <stdbool.h>
#include <stdlib.h>
#include <re.h>
#define MESSAGE_SEND_BUFFER_SIZE (512U)
#define IN_BETWEEN_ACTION_DELAY (1000U)
static char tx_buffer[MESSAGE_SEND_BUFFER_SIZE] = "";
static char* http_enum_string[] = {"0","1","2","3"};
const char* GET_SIZE_AT ="+HTTPGETSIZE:";
const char* SEND_OK ="SEND OK";

struct esp32_http_t 
{
    esp32_chat_t *esp32_chat_object;
    bool initialized;
};

http_request_t DEFAULT_HTTP_REQUEST = {
    .request_type = HTTP_HEAD,
    .content_type = HTTP_CONTENT_APPLICATION_WWW_FROM_URL_ENCODED,
    .url = "",
    .host = "",
    .path = "",
    .transport_type = HTTP_TRANSPORT_OVER_TCP,
    .data = ""
};

esp32_http_t *esp32HttpCreate(esp32_chat_t *esp32_chat_object)
{
        if(!esp32_chat_object)
        {
            return NULL;
        }
        esp32_http_t *esp32_http_object = (esp32_http_t *)malloc(sizeof(esp32_http_t));
        if(!esp32_http_object){
            return NULL;
        }
        esp32_http_object->esp32_chat_object = esp32_chat_object;
        return esp32_http_object;
}

error_type_t esp32HttpInit(esp32_http_t *esp32_http_object)
{
    if (esp32_http_object == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (esp32_http_object->esp32_chat_object == NULL)
    {
        return SYSTEM_INVALID_PARAMETER;
    }
    esp32_http_object->initialized = true;
    return SYSTEM_OK;
}
static error_type_t validate(esp32_http_t *esp32_http_object)
{
    if (esp32_http_object == NULL)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!esp32_http_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    return SYSTEM_OK;
}

error_type_t esp32HttpDeInit(esp32_http_t *esp32_http_object)
{
    if (esp32_http_object)
    {
        esp32_http_object->initialized = false;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}
error_type_t esp32HttpDestroy(esp32_http_t **esp32_http_object)
{
    if (*esp32_http_object)
    {
        free(*esp32_http_object);
        *esp32_http_object = NULL;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
}

error_type_t esp32HttpSend(esp32_http_t *esp32_http_object,http_request_t request, char* response)
{
    error_type_t result = SYSTEM_FAILED;
    result = validate(esp32_http_object);
    if(result != SYSTEM_OK){
        return result;
    }
    CLEAR_CHAT_BUFFER(tx_buffer);
    AT_HTTP_CMD(tx_buffer);
    strncat(tx_buffer,http_enum_string[request.request_type],MAX_AT_BUFF_ARGUMENT_LENGTH);
    strcat(tx_buffer,",");
    strncat(tx_buffer,http_enum_string[request.content_type],MAX_AT_BUFF_ARGUMENT_LENGTH);
    strcat(tx_buffer,",");
    if(strlen(request.url)>0){
        strcat(tx_buffer,"\"");
    }
    strncat(tx_buffer,request.url,MAX_AT_BUFF_ARGUMENT_LENGTH);
    if(strlen(request.url)>0){
        strcat(tx_buffer,"\"");
    }
    strcat(tx_buffer,",");
    if(strlen(request.host)>0){
        strcat(tx_buffer,"\"");
    }
    strncat(tx_buffer,request.host,MAX_AT_BUFF_ARGUMENT_LENGTH);
    if(strlen(request.host)>0){
        strcat(tx_buffer,"\"");
    }
    strcat(tx_buffer,",");
    if(strlen(request.path)>0){
        strcat(tx_buffer,"\"");
    }
    strncat(tx_buffer,request.path,MAX_AT_BUFF_ARGUMENT_LENGTH);
    if(strlen(request.path)>0){
        strcat(tx_buffer,"\"");
    }
    strcat(tx_buffer,",");
    strncat(tx_buffer,http_enum_string[request.transport_type],MAX_AT_BUFF_ARGUMENT_LENGTH);
    if(strlen(request.data)>0)
    {
        strcat(tx_buffer,",");
        strcat(tx_buffer,request.data);
    }
    strcat(tx_buffer,"\n");
    memset(response, 0, MESSAGE_SEND_BUFFER_SIZE);
    esp32_chat_response_t esp_response = {.buffer = response,.buffer_size= MESSAGE_SEND_BUFFER_SIZE};
    result = esp32ChatSendReceive(esp32_http_object->esp32_chat_object, tx_buffer, &esp_response);
    return result;
}

error_type_t esp32HttpGetSize(esp32_http_t *esp32_http_object,char* url,uint32_t* http_size)
{
    error_type_t result = SYSTEM_FAILED;
    result = validate(esp32_http_object);
    if(result != SYSTEM_OK){
        return result;
    }
    CLEAR_CHAT_BUFFER(tx_buffer);
    AT_HTTP_GETSIZE_CMD(tx_buffer,url);
    char buffer[100];
    char size_string[20];
    esp32_chat_response_t response = {.buffer = buffer, .buffer_size=100};
    result = esp32ChatSendReceive(esp32_http_object->esp32_chat_object,tx_buffer, &response);
    if(result != SYSTEM_OK){
        return result;
    }

    char* index = strstr(buffer,GET_SIZE_AT);
    if(!index){
        return SYSTEM_INVALID_RESPONSE;
    }
    index = index + strlen(GET_SIZE_AT);
    char* end_index = strchr(index,'\r');
    if(!end_index){
        return SYSTEM_INVALID_RESPONSE;
    }
    //end_index = end_index + (int)index;
    memcpy(size_string,index,(end_index-index));
    printf("string size is: %s\n",size_string);
    *http_size = atoi(size_string);
    return SYSTEM_OK;



}

 error_type_t esp32HttpPost(esp32_http_t *esp32_http_object,esp32_http_post_request_t * post_request,char* response)
 {
    error_type_t result = SYSTEM_FAILED;
    result = validate(esp32_http_object);
    if(result != SYSTEM_OK){
        return result;
    } 
    if(strlen(post_request->payload) != post_request->payload_size){
        return SYSTEM_INVALID_LENGTH;
    }
    CLEAR_CHAT_BUFFER(tx_buffer);
    strcat(tx_buffer,AT_HTTPCPOST_CMD);
    strcat(tx_buffer,"\"");
    strncat(tx_buffer,post_request->url,MAX_AT_BUFF_ARGUMENT_LENGTH);
    strcat(tx_buffer,"\"");
    strcat(tx_buffer,",");
    char payload_string[10];
    sprintf(payload_string,"%ld",post_request->payload_size);
    strncat(tx_buffer,payload_string,MAX_AT_BUFF_ARGUMENT_LENGTH);
    if(post_request->additional_headers){
        strcat(tx_buffer,",");
        strncat(tx_buffer,post_request->additional_headers,MAX_AT_BUFF_ARGUMENT_LENGTH);
    }
    strcat(tx_buffer,"\n");
    esp32_chat_response_t post_response = {.buffer = response, .buffer_size=MESSAGE_SEND_BUFFER_SIZE,.expected_response="OK\r\n\r\n>",.timeout=7000};
    result = esp32ChatSendReceive(esp32_http_object->esp32_chat_object,tx_buffer, &post_response);
    if(result != SYSTEM_OK){
        return result;
    } 
    CLEAR_CHAT_BUFFER(tx_buffer);
    strncat(tx_buffer,post_request->payload,MAX_AT_BUFF_ARGUMENT_LENGTH);
    post_response.expected_response = SEND_OK;
    post_response.timeout = 10000;
    result = esp32ChatSendReceive(esp32_http_object->esp32_chat_object,tx_buffer, &post_response);
    if(result != SYSTEM_OK){
        return result;
    } 

    // Comment code left here because according to the document
    // AT + HTTP is supposed to retrieve the Http error code but 
    // that's not working.  The issue has been logged on the esp-at repo



    // CLEAR_CHAT_BUFFER(tx_buffer);
    // strcat(tx_buffer,"AT+HTTPCLIENT\n");
    // post_response.timeout = 0;
    // post_response.expected_response = "";
    // result = esp32ChatSendReceive(esp32_http_object->esp32_chat_object,tx_buffer,&post_response);
    // if(result != SYSTEM_OK){
    //     return result;
    // } 
    return SYSTEM_OK;

 }
