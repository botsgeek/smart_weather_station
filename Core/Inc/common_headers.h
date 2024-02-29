#ifndef _COMMON_HEADERS_H_
#define _COMMON_HEADERS_H_
#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define SENSOR_INVALID_VALUE (-1)

    typedef enum
    {
        SYSTEM_OK,
        SYSTEM_NULL_PARAMETER,
        SYSTEM_INVALID_PARAMETER,
        SYSTEM_INVALID_PIN_NUMBER,
        SYSTEM_INVALID_STATE,
        SYSTEM_INVALID_MODE,
        SYSTEM_INVALID_BAUD_RATE,
        SYSTEM_INVALID_LENGTH,
        SYSTEM_TIMED_OUT,
        SYSTEM_BUFFER_OVERFLOW,
        SYSTEM_UNKNOWN,
        SYSTEM_FAILED,
        SYSTEM_INVALID_TEMPERATURE,
        SYSTEM_INVALID_PIN_PORT,
        SYSTEM_NO_RESPONSE,
        SYSTEM_NO_ACK,
        SYSTEM_CHECK_SUM_VALIDATION_FAILED,
        SYSTEM_BUSY
    } error_type_t;
    extern TIM_HandleTypeDef htim3;
    extern UART_HandleTypeDef huart1;
    static void inline __attribute__((always_inline)) delayUs(uint16_t delay)
    {
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        while (__HAL_TIM_GET_COUNTER(&htim3) < delay)
        {
            asm volatile("nop");
        }
    }
    int _write(int file, char *ptr, int len);

#ifdef __cplusplus
}
#endif
#endif