#include <tft_9341.h>
#include <string.h>

#define TFT_9341_SPI_TIMEOUT 1000

const uint8_t TFT_9341_INIT_REGISTERS[] = {

    ILI9341_PWCTR1, 1, 0x1B,         // Power control VRH[5:0]
    ILI9341_PWCTR2, 1, 0x12,         // Power control SAP[2:0];BT[3:0]
    ILI9341_VMCTR1, 2, 0x26, 0x26,   // VCM control
    ILI9341_VMCTR2, 1, 0xB0,         // VCM control2
    ILI9341_MADCTL, 1, 0x08,         // Memory Access Control
    ILI9341_VSCRSADD, 2, 0x00, 0x00, // Vertical scroll zero
    ILI9341_PIXFMT, 1, 0x55,
    ILI9341_FRMCTR1, 2, 0x00, 0x1A,
    ILI9341_DFUNCTR, 4, 0x0A, 0xA2, 0x27, 0x00, // Display Function Control

    ILI9341_GAMMASET, 1, 0x01,                               // Gamma curve selected
    ILI9341_GMCTRP1, 15, 0x1F, 0x24, 0x24, 0x0D, 0x12, 0x09, // Set Gamma
    0x52, 0xB7, 0x3F, 0x0C, 0x15, 0x06, 0x0E, 0x08, 0x00,
    ILI9341_GMCTRN1, 15, 0x00, 0x1B, 0x1B, 0x02, 0x0E, 0x06, // Set Gamma
    0x2E, 0x48, 0x3F, 0x03, 0x0A, 0x09, 0x31, 0x37, 0x1F,
    0x2B, 4, 0x00, 0x00, 0x01, 0x3F,
    0x2A, 4, 0x00, 0x00, 0x00, 0xEF,

    ILI9341_SLPOUT, 1, 0x80, // Exit Sleep
    ILI9341_DISPON, 1, 0x80, // Display on
    0x00                     // End of list
};
struct tft_9341_t
{
    tft_9341_config_t config;
    bool initialized;
};

static void enableCSPin(tft_9341_t *tft_9341_object)
{

    HAL_GPIO_WritePin(tft_9341_object->config.cs_gpio_port, tft_9341_object->config.cs_gpio_pin, GPIO_PIN_RESET);
}

static void disableCSPin(tft_9341_t *tft_9341_object)
{

    HAL_GPIO_WritePin(tft_9341_object->config.cs_gpio_port, tft_9341_object->config.cs_gpio_pin, GPIO_PIN_SET);
}

static error_type_t sendCommand(tft_9341_t *tft_9341_object, uint8_t command)
{
    HAL_StatusTypeDef spi_error;
    if (tft_9341_object->config.mode == TFT_9341_MODE_4_LINE)
    {
        HAL_GPIO_WritePin(tft_9341_object->config.dc_gpio_port, tft_9341_object->config.dc_gpio_pin, GPIO_PIN_RESET);
        uint8_t cmd = 0xff & command;
        spi_error = HAL_SPI_Transmit(tft_9341_object->config.spi_handle, &cmd, 1, TFT_9341_SPI_TIMEOUT);
        HAL_GPIO_WritePin(tft_9341_object->config.dc_gpio_port, tft_9341_object->config.dc_gpio_pin, GPIO_PIN_SET);
    }
    else
    {
        uint8_t cmd[2];
        cmd[0]=0x01;
        cmd[1] = command;
        spi_error = HAL_SPI_Transmit(tft_9341_object->config.spi_handle, (uint8_t*)&cmd, 2, TFT_9341_SPI_TIMEOUT);
    }
    if (spi_error != HAL_OK)
    {
        return SYSTEM_COMMS_ERROR;
    }
    return SYSTEM_OK;
}

static error_type_t writeInitRegisters(tft_9341_t *tft_9341_object, const uint8_t *init_cmd_buffer)
{

    error_type_t err = SYSTEM_OK;
    uint8_t* init_cmd_buff = (uint8_t*)init_cmd_buffer;
    uint8_t cmd = *init_cmd_buff;
    uint8_t cmd_size = *(init_cmd_buff + 1);
    uint8_t *cmd_pointer = init_cmd_buff + 2;
    while (cmd && err == SYSTEM_OK)
    {
        err = tft_9341SelectRegister(tft_9341_object,
                                     cmd);
        if(err != SYSTEM_OK){
            continue;
        }
        err = tft_9341WriteData(tft_9341_object, cmd_pointer, cmd_size);
        if(err != SYSTEM_OK){
            continue;
        }
        init_cmd_buff = init_cmd_buff + cmd_size + 2;
        if (cmd == 0x08)
        {
            HAL_Delay(150);
        }
        cmd = *init_cmd_buff;
        cmd_size = *(init_cmd_buff + 1);
        cmd_pointer = init_cmd_buff + 2;
    }
    return err;
}
tft_9341_t *tft_9341Create(const tft_9341_config_t *config)
{
    if (!config)
    {
        return NULL;
    }
    tft_9341_t *tft_9341_object = (tft_9341_t *)malloc(sizeof(tft_9341_t));
    if (!tft_9341_object)
    {
        return NULL;
    }
    memcpy(&(tft_9341_object->config), config, sizeof(tft_9341_config_t));
    tft_9341_object->initialized = false;
    return tft_9341_object;
}

error_type_t tft_9341Init(tft_9341_t *tft_9341_object)
{
    if (!tft_9341_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    HAL_GPIO_WritePin(tft_9341_object->config.dc_gpio_port, tft_9341_object->config.dc_gpio_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tft_9341_object->config.led_gpio_port, tft_9341_object->config.led_gpio_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(tft_9341_object->config.reset_gpio_port, tft_9341_object->config.reset_gpio_pin, GPIO_PIN_SET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(tft_9341_object->config.reset_gpio_port, tft_9341_object->config.reset_gpio_pin, GPIO_PIN_RESET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(tft_9341_object->config.reset_gpio_port, tft_9341_object->config.reset_gpio_pin, GPIO_PIN_SET);
    HAL_Delay(200);
    tft_9341_object->initialized = true;
    return tft_9341SetReset(tft_9341_object, GPIO_PIN_SET);
}

error_type_t tft_9341SetLed(tft_9341_t *tft_9341_object, GPIO_PinState pin_state)
{
    if (!tft_9341_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!tft_9341_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    HAL_GPIO_WritePin(tft_9341_object->config.led_gpio_port, tft_9341_object->config.led_gpio_pin, pin_state);
    return SYSTEM_OK;
}

error_type_t tft_9341SelectRegister(tft_9341_t *tft_9341_object, uint8_t register_val)
{
    error_type_t err;
    enableCSPin(tft_9341_object);
    err = sendCommand(tft_9341_object, register_val);
    if (err != SYSTEM_OK)
    {
        disableCSPin(tft_9341_object);
        return err;
    }
    err = SYSTEM_OK;
    disableCSPin(tft_9341_object);
    return err;
}
error_type_t tft_9341SelectAndReadRegister(tft_9341_t *tft_9341_object, uint16_t command,
                                                 uint8_t *response_buffer, size_t response_buffer_size)
{
    error_type_t err;
    HAL_StatusTypeDef spi_error;
    do
    {
        enableCSPin(tft_9341_object);
        err = sendCommand(tft_9341_object, command);
        if (err != SYSTEM_OK)
        {
            break;
        }
        spi_error = HAL_SPI_Receive(tft_9341_object->config.spi_handle, response_buffer, response_buffer_size, TFT_9341_SPI_TIMEOUT);
        if (spi_error != HAL_OK)
        {
            err = SYSTEM_COMMS_ERROR;
            break;
        }
        err = SYSTEM_OK;
    } while (0);

    disableCSPin(tft_9341_object);
    return err;
}

error_type_t tft_9341SetReset(tft_9341_t *tft_9341_object, GPIO_PinState pin_state)
{
    if (!tft_9341_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!tft_9341_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    HAL_GPIO_WritePin(tft_9341_object->config.reset_gpio_port, tft_9341_object->config.reset_gpio_pin, pin_state);
    return SYSTEM_OK;
}

error_type_t tft_9341InitializeRegisters(tft_9341_t *tft_9341_object, const uint8_t *cmd)
{
    if (!tft_9341_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    if (!tft_9341_object->initialized)
    {
        return SYSTEM_INVALID_STATE;
    }
    return writeInitRegisters(tft_9341_object, cmd);
}

error_type_t tft_9341WriteData(tft_9341_t *tft_9341_object, uint8_t *data, uint16_t data_size)
{
    error_type_t err = SYSTEM_OK;
    HAL_StatusTypeDef spi_error;
    enableCSPin(tft_9341_object);
    spi_error = HAL_SPI_Transmit(tft_9341_object->config.spi_handle, data, data_size, TFT_9341_SPI_TIMEOUT);
    if (spi_error != HAL_OK)
    {
        err = SYSTEM_COMMS_ERROR;
    }
    disableCSPin(tft_9341_object);
    return err;
}
error_type_t tft_9341WritePixelColor(tft_9341_t *tft_9341_object, uint16_t color)
{
    error_type_t err = SYSTEM_OK;
    HAL_StatusTypeDef spi_error;
    uint8_t lsb = color & 0xFF;
    uint8_t msb = color >> 8;
    enableCSPin(tft_9341_object);
    spi_error = HAL_SPI_Transmit(tft_9341_object->config.spi_handle, &msb, 1, TFT_9341_SPI_TIMEOUT);
    if (spi_error != HAL_OK)
    {
        err = SYSTEM_COMMS_ERROR;
    }
    spi_error = HAL_SPI_Transmit(tft_9341_object->config.spi_handle, &lsb, 1, TFT_9341_SPI_TIMEOUT);
    if (spi_error != HAL_OK)
    {
        err = SYSTEM_COMMS_ERROR;
    }
    disableCSPin(tft_9341_object);
    return err;
}
error_type_t tft_9341DeInit(tft_9341_t *tft_9341_object)
{
    if (!tft_9341_object)
    {
        return SYSTEM_NULL_PARAMETER;
    }
    error_type_t err = tft_9341SetReset(tft_9341_object, GPIO_PIN_RESET);
    if (err != SYSTEM_OK)
    {
        return err;
    }
    tft_9341_object->initialized = false;
    return SYSTEM_OK;
}
// error_type_t tft_9341Read(tft_9341_t *tft_9341_object, tft_9341_data_t *data)
// {
//     return SYSTEM_OK;
// }
error_type_t tft_9341Destroy(tft_9341_t **tft_9341_object)
{
    if (*tft_9341_object)
    {
        free(*tft_9341_object);
        *tft_9341_object = NULL;
        return SYSTEM_OK;
    }
    else
    {
        return SYSTEM_NULL_PARAMETER;
    }
    return SYSTEM_OK;
}