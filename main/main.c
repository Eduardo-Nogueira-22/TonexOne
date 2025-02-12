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

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_ota_ops.h"
#include "sys/param.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_crc.h"
#include "esp_now.h"
#include "driver/i2c.h"
#include "esp_intr_alloc.h"
#include "usb/usb_host.h"
#include "esp_private/periph_ctrl.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

#include "main.h"
#include "task_priorities.h"
#include "usb_comms.h"
#include "usb/usb_host.h"
#include "usb_tonex_one.h"
#include "footswitches.h"
#include "control.h"
#include "midi_control.h"
#include "midi_serial.h"
#include "wifi_config.h"
#include "leds.h"
#include "tonex_params.h"
#include "i2c-lcd.h"

// #define I2C_MASTER_NUM                  0       /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
// #define I2C_MASTER_FREQ_HZ              400000                     /*!< I2C master clock frequency */
// #define I2C_MASTER_TX_BUF_DISABLE       0                          /*!< I2C master doesn't need buffer */
// #define I2C_MASTER_RX_BUF_DISABLE       0                          /*!< I2C master doesn't need buffer */

// #define I2C_CLR_BUS_SCL_NUM            (9)
// #define I2C_CLR_BUS_HALF_PERIOD_US     (5)

static const char *TAG = "app_main";

static __attribute__((unused)) SemaphoreHandle_t I2CMutex;

/****************************************************************************
* NAME:        
* DESCRIPTION: 
* PARAMETERS:  
* RETURN:      
* NOTES:       
*****************************************************************************/
void app_main(void)
{
    ESP_LOGI(TAG, "ToneX One Controller App start");

    // load the config first
    control_load_config();


    // init parameters
    ESP_LOGI(TAG, "Init Params");
    tonex_params_init();

    // init control task
    ESP_LOGI(TAG, "Init Control");
    control_init();

    // init Footswitches
    ESP_LOGI(TAG, "Init footswitches");
    footswitches_init();

    if (control_get_config_bt_mode() != BT_MODE_DISABLED)
    {
        // init Midi Bluetooth
        ESP_LOGI(TAG, "Init MIDI BT");
        midi_init();
    }
    else
    {
        ESP_LOGI(TAG, "MIDI BT disabled");
    }

    if (control_get_config_midi_serial_enable())
    {
        // init Midi serial
        ESP_LOGI(TAG, "Init MIDI Serial");
        midi_serial_init();
    }
    else
    {    
        ESP_LOGI(TAG, "Serial MIDI disabled");
    }

    // init USB
    ESP_LOGI(TAG, "Init USB");
    init_usb_comms();

    // init WiFi config
    wifi_config_init();

    //lcd_function();
}
