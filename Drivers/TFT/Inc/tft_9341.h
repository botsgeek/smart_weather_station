#ifndef __TFT_9341_H__
#define __TFT_9341_H__
#include <common_headers.h>
#include <stm32f411xe.h>
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define ILI9341_TFTWIDTH 240  ///< ILI9341 max TFT width
#define ILI9341_TFTHEIGHT 320 ///< ILI9341 max TFT height

#define ILI9341_NOP 0x00     ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID 0x04   ///< Read display identification information
#define ILI9341_RDDST 0x09   ///< Read Display Status

#define ILI9341_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON 0x12  ///< Partial Mode ON
#define ILI9341_NORON 0x13  ///< Normal Display Mode ON

#define ILI9341_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9341_INVON 0x21    ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF 0x28  ///< Display OFF
#define ILI9341_DISPON 0x29   ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR 0x30    ///< Partial Area
#define ILI9341_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   ///< Memory Access Control
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1                                                        \
  0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3                                                        \
  0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 ///< Negative Gamma Correction
//#define ILI9341_PWCTR6     0xFC

// Color definitions
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198
typedef struct tft_9341_t tft_9341_t;
typedef enum {
    TFT_9341_MODE_3_LINE,
    TFT_9341_MODE_4_LINE,
} tft_9341_mode_t;

typedef struct {
    SPI_HandleTypeDef *spi_handle;
    GPIO_TypeDef *cs_gpio_port;
    uint16_t cs_gpio_pin;
    GPIO_TypeDef *reset_gpio_port;
    uint16_t reset_gpio_pin;
    GPIO_TypeDef *led_gpio_port;
    uint16_t led_gpio_pin;
    tft_9341_mode_t mode;
    GPIO_TypeDef *dc_gpio_port;
    uint16_t dc_gpio_pin;
} tft_9341_config_t;

extern const uint8_t TFT_9341_INIT_REGISTERS[];
tft_9341_t *tft_9341Create(const tft_9341_config_t *config);
error_type_t tft_9341Init(tft_9341_t *tft_9341_object);
error_type_t tft_9341DeInit(tft_9341_t *tft_9341_object);
error_type_t tft_9341SetLed(tft_9341_t *tft_9341_object,GPIO_PinState pin_state);
error_type_t tft_9341SetReset(tft_9341_t *tft_9341_object,GPIO_PinState pin_state);
error_type_t tft_9341SelectRegister(tft_9341_t *tft_9341_object,uint8_t register_val);
error_type_t tft_9341SelectAndReadRegister(tft_9341_t *tft_9341_object, uint16_t command,
                                                    uint8_t* response_buffer,size_t response_buffer_size);
error_type_t tft_9341InitializeRegisters(tft_9341_t *tft_9341_object,const uint8_t* cmd);
error_type_t tft_9341WriteRegister(tft_9341_t *tft_9341_object, uint8_t command);
error_type_t tft_9341WritePixelColor(tft_9341_t *tft_9341_object, uint16_t color);
error_type_t tft_9341WriteData(tft_9341_t *tft_9341_object, uint8_t *data, uint16_t data_size);
//error_type_t tft_9341Read(tft_9341_t *tft_9341_object, tft_9341_data_t *data);
error_type_t tft_9341Destroy(tft_9341_t **tft_9341_object);
#ifdef __cplusplus
}
#endif

#endif


