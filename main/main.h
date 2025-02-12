/*
 Copyright (C) 2024  Greg Smith

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 
*/


#ifndef _MAIN_H
#define _MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define APP_VERSION		"1.0.7.2"

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B
    // I2C bus
    #define I2C_MASTER_SCL_IO  GPIO_NUM_9       
    #define I2C_MASTER_SDA_IO  GPIO_NUM_8       

    // IO expander
    #define FOOTSWITCH_1		IO_EXPANDER_PIN_NUM_0
    #define TOUCH_RESET 		IO_EXPANDER_PIN_NUM_1
    #define LCD_BACKLIGHT		IO_EXPANDER_PIN_NUM_2
    #define LCD_RESET    		IO_EXPANDER_PIN_NUM_3
    #define SD_CS       		IO_EXPANDER_PIN_NUM_4
    #define FOOTSWITCH_2		IO_EXPANDER_PIN_NUM_5

    // these 2 unsupported
    #define FOOTSWITCH_3		-1
    #define FOOTSWITCH_4		-1

    // Micro pins
    #define TOUCH_INT           GPIO_NUM_4    // touch panel interrupt

    // Midi: RS485 port
    #define UART_RX_PIN         GPIO_NUM_43
    #define UART_TX_PIN         GPIO_NUM_44 

    // Midi on Waveshare 7" using RS485 port (dev only)
    //#define UART_RX_PIN       GPIO_NUM_15 
    //#define UART_TX_PIN       GPIO_NUM_16 

    // Midi: ADC port on 4.3 dev board (dev only)
    //#define UART_RX_PIN         GPIO_NUM_6
    //#define UART_TX_PIN         -1

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define DISPLAY_PIN_NUM_BK_LIGHT       -1
    #define DISPLAY_PIN_NUM_HSYNC          46
    #define DISPLAY_PIN_NUM_VSYNC          3
    #define DISPLAY_PIN_NUM_DE             5
    #define DISPLAY_PIN_NUM_PCLK           7
    #define DISPLAY_PIN_NUM_DATA0          14 // B3
    #define DISPLAY_PIN_NUM_DATA1          38 // B4
    #define DISPLAY_PIN_NUM_DATA2          18 // B5
    #define DISPLAY_PIN_NUM_DATA3          17 // B6
    #define DISPLAY_PIN_NUM_DATA4          10 // B7
    #define DISPLAY_PIN_NUM_DATA5          39 // G2
    #define DISPLAY_PIN_NUM_DATA6          0 // G3
    #define DISPLAY_PIN_NUM_DATA7          45 // G4
    #define DISPLAY_PIN_NUM_DATA8          48 // G5
    #define DISPLAY_PIN_NUM_DATA9          47 // G6
    #define DISPLAY_PIN_NUM_DATA10         21 // G7
    #define DISPLAY_PIN_NUM_DATA11         1  // R3
    #define DISPLAY_PIN_NUM_DATA12         2  // R4
    #define DISPLAY_PIN_NUM_DATA13         42 // R5
    #define DISPLAY_PIN_NUM_DATA14         41 // R6
    #define DISPLAY_PIN_NUM_DATA15         40 // R7
    #define DISPLAY_PIN_NUM_DISP_EN        -1

    // Pin assignments for SD Card
    #define PIN_NUM_MISO        13
    #define PIN_NUM_MOSI        11
    #define PIN_NUM_CLK         12
    #define PIN_NUM_CS          -1

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169
    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_16
    #define FOOTSWITCH_2		GPIO_NUM_3
    #define FOOTSWITCH_3		GPIO_NUM_2
    #define FOOTSWITCH_4		GPIO_NUM_44     // same as UART RX

    // Midi
    #define UART_RX_PIN         GPIO_NUM_18 
    #define UART_TX_PIN         GPIO_NUM_17 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define WAVESHARE_240_280_LCD_GPIO_SCLK           (GPIO_NUM_6)
    #define WAVESHARE_240_280_LCD_GPIO_MOSI           (GPIO_NUM_7)
    #define WAVESHARE_240_280_LCD_GPIO_RST            (GPIO_NUM_8)
    #define WAVESHARE_240_280_LCD_GPIO_DC             (GPIO_NUM_4)
    #define WAVESHARE_240_280_LCD_GPIO_CS             (GPIO_NUM_5)
    #define WAVESHARE_240_280_LCD_GPIO_BL             (GPIO_NUM_15)

    // Buzzer is GPIO42 on PCB V2. GPIO33 on PCV V1.
    // note here: GPIO33 used on V1 PCB conflicts with Octal mode PSRAM, so can't use that
    #define WAVESHARE_240_280_BUZZER                  GPIO_NUM_42

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_ZERO
    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_2
    #define FOOTSWITCH_2		GPIO_NUM_3
    #define FOOTSWITCH_3		GPIO_NUM_4
    #define FOOTSWITCH_4		GPIO_NUM_1 
    #define FOOTSWITCH_5		GPIO_NUM_13
    #define FOOTSWITCH_6		GPIO_NUM_12

    // Midi
    #define UART_RX_PIN         GPIO_NUM_5
    #define UART_TX_PIN         GPIO_NUM_7

    // leds
    #define LED_OUTPUT_GPIO_NUM    GPIO_NUM_48

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_DEVKITC
    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_4
    #define FOOTSWITCH_2		GPIO_NUM_6
    #define FOOTSWITCH_3		GPIO_NUM_2
    #define FOOTSWITCH_4		GPIO_NUM_1 

    // Midi
    #define UART_RX_PIN         GPIO_NUM_5
    #define UART_TX_PIN         GPIO_NUM_7

    // leds
    #define LED_OUTPUT_GPIO_NUM   GPIO_NUM_48

#elif CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R
    // I2C bus
    #define I2C_MASTER_SCL_IO  GPIO_NUM_0       
    #define I2C_MASTER_SDA_IO  GPIO_NUM_45       

    // direct IO pins
    #define FOOTSWITCH_1		GPIO_NUM_5
    #define FOOTSWITCH_2		GPIO_NUM_6
    #define FOOTSWITCH_3		GPIO_NUM_7
    #define FOOTSWITCH_4		GPIO_NUM_8

    // Midi
    #define UART_RX_PIN         GPIO_NUM_38 
    #define UART_TX_PIN         GPIO_NUM_39 

    // leds
    #define LED_OUTPUT_GPIO_NUM          -1

    // LCD pins
    #define ATOM3SR_LCD_GPIO_SCLK           (GPIO_NUM_15)
    #define ATOM3SR_LCD_GPIO_MOSI           (GPIO_NUM_21)
    #define ATOM3SR_LCD_GPIO_RST            (GPIO_NUM_48)
    #define ATOM3SR_LCD_GPIO_DC             (GPIO_NUM_42)
    #define ATOM3SR_LCD_GPIO_CS             (GPIO_NUM_14)
    // Note LCD Backlight uses led driver on I2C

#else
    #error "Unknown hardware platform!"
#endif

esp_err_t i2c_master_reset(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif