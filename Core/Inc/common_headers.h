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
        OK,
        NULL_PARAMETER,
        INVALID_PARAMETER,
        INVALID_PIN_NUMBER,
        INVALID_STATE,
        INVALID_MODE,
        INVALID_BAUD_RATE,
        INVALID_LENGTH,
        TIMED_OUT,
        BUFFER_OVERFLOW,
        UNKNOWN,
        FAILED,
        INVALID_TEMPERATURE,
        INVALID_PIN_PORT,
        NO_RESPONSE,
        NO_ACK,
        CHECK_SUM_VALIDATION_FAILED
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