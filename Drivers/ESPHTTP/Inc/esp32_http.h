/**
 * @file esp32_http.h
 * @author Rilwan (rsaliu@botsgeeks.com)
 * @brief 
 * @version 0.1
 * @date 2024-04-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __esp32_HTTP_H___
#define __esp32_HTTP_H___
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
    typedef enum {
        HTTP_HEAD=1,
        HTTP_GET,
        HTTP_POST,
        HTTP_PUT,
        HTTP_DELETE,
    }http_request_type_t;

    typedef enum {
        HTTP_CONTENT_APPLICATION_WWW_FROM_URL_ENCODED=0,
        HTTP_CONTENT_APPLICATION_JSON,
        HTTP_CONTENT_MULTIPART_FORM_DATA,
        HTTP_CONTENT_TEXT_XML
    }http_content_type_t;

    typedef enum{
        HTTP_TRANSPORT_OVER_TCP=1,
        HTTP_TRANSPORT_OVER_SSL
    }http_transport_type_t;
    typedef struct{
        http_request_type_t request_type;
        http_content_type_t content_type;
        char * url;
        char* host;
        char * path;
        http_transport_type_t transport_type;
        char * data;
    }http_request_t;
    extern http_request_t DEFAULT_HTTP_REQUEST;
    typedef struct{
        char* url;
        uint32_t payload_size;
        char* payload;
        char* additional_headers;
    }esp32_http_post_request_t;
    /**
     * @brief esp32_http_t object shadow
     *
     */
    typedef struct esp32_http_t esp32_http_t;
    /**
     * @brief Creates http object
     *
     * @param esp32_chat_object
     * @return esp32_http_t*
     */
    esp32_http_t *esp32HttpCreate(esp32_chat_t *esp32_chat_object);
    /**
     * @brief Initializes http
     *
     * @param esp32_http_object
     * @return error_type_t
     */
    error_type_t esp32HttpInit(esp32_http_t *esp32_http_object);
    /**
     * @brief 
     * 
     * @param esp32_http_object 
     * @param request 
     * @param response 
     * @return error_type_t 
     */
    error_type_t esp32HttpSend(esp32_http_t *esp32_http_object,http_request_t request, char* response);
    /**
     * @brief 
     * 
     * @param esp32_http_object 
     * @param url 
     * @param http_size 
     * @return error_type_t 
     */
    error_type_t esp32HttpGetSize(esp32_http_t *esp32_http_object,char* url,uint32_t* http_size);
    /**
     * @brief 
     * 
     * @param esp32_http_object 
     * @param url 
     * @param payload_size 
     * @param additional_headers 
     * @return error_type_t 
     */
    error_type_t esp32HttpPost(esp32_http_t *esp32_http_object,esp32_http_post_request_t * post_request,char* response);
    /**
     * @brief Deinitializes http object
     *
     * @param esp32_http_object
     * @return error_type_t
     */
    error_type_t esp32HttpDeInit(esp32_http_t *esp32_http_object);
    /**
     * @brief destroys http object
     *
     * @param esp32_http_object
     * @return error_type_t
     */
    error_type_t esp32HttpDestroy(esp32_http_t **esp32_http_object);
#ifdef __cplusplus
}

#endif
#endif