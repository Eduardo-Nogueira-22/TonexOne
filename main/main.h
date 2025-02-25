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



esp_err_t i2c_master_reset(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif