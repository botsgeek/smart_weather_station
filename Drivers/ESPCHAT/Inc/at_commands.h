#ifndef __AT_COMMANDS_H___
#define __AT_COMMANDS_H___
#include <string.h>
#include <stdint.h>
#define ERROR_RESPONSE ("\r\nERROR")
#define OK_RESPONSE ("\r\nOK")
#define BUSY_RESPONSE ("busy p...")

#define AT_CMD ("AT\n")
#define AT_RESET_CMD ("AT+RST\n")
#define AT_CWMODE_QUERY_CMD ("AT+CWMODE?\n")
#define WIFI_NULL_MODE "0"
#define WIFI_STATION_MODE "1"
#define WIFI_SOFT_AP_MODE "2"
#define WIFI_SOFT_AP_STATION_MODE "3"
#define AT_CWMODE_SET_CMD(x, y)    \
    do                             \
    {                              \
        strcat((x), "AT+CWMODE="); \
        strcat((x), (y));          \
        strcat((x), (","));        \
        strcat((x), ("1"));        \
        strcat((x), ("\n"));       \
        strcat((x), ("\0"));       \
    } while (0)
#define AT_CWJAP_SET_CMD(x, y, z)   \
    do                              \
    {                               \
        strcat((x), ("AT+CWJAP=")); \
        strcat((x), ("\""));        \
        strcat((x), (y));           \
        strcat((x), ("\""));        \
        strcat((x), (","));         \
        strcat((x), ("\""));        \
        strcat((x), (z));           \
        strcat((x), ("\""));        \
        strcat((x), ("\n"));        \
        strcat((x), ("\0"));        \
    } while (0)
#define AT_CWJAP_GET_CMD ("AT+CWJAP?\n")
#define AT_CWQAP_SET_CMD ("AT+CWQAP\n")
#define AT_CIFSR_GET_CMD ("AT+CIFSR\n")

#define EXTRA_AT_TERMINATOR_LENGTH 3
typedef enum
{
    AT_OK,
    AT_ERROR,
    AT_BUSY,
    AT_UNKNOWN
} at_response_type_t;
static inline at_response_type_t atCheckResponse(const char *message)
{
    if (strstr(message, BUSY_RESPONSE))
    {
        return AT_BUSY;
    }
    else if (strstr(message, OK_RESPONSE))
    {
        return AT_OK;
    }
    else if (strstr(message, ERROR_RESPONSE))
    {
        return AT_ERROR;
    }
    else
    {
        return AT_UNKNOWN;
    }
}

#endif