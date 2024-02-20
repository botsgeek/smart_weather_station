#include <common_headers.h>
#define PRINT_TIMEOUT 100
int _write(int file, char *ptr, int len)
{
#ifdef DEBUG
  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, PRINT_TIMEOUT);
#endif
  return len;
}