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
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sys/param.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "main.h"
#include "control.h"
#include "task_priorities.h"
#include "usb/usb_host.h"
#include "usb_comms.h"
#include "usb_tonex_one.h"
#include "leds.h"
#include "midi_helper.h"

#define FOOTSWITCH_TASK_STACK_SIZE (3 * 1024)
#define FOOTSWITCH_SAMPLE_COUNT 5 // 20 msec per sample
#define BANK_MODE_BUTTONS 6
#define BANK_MAXIMUM (MAX_PRESETS / BANK_MODE_BUTTONS)
#define BUTTON_FACTORY_RESET_TIME 500 // * 20 msec = 10 secs

enum FootswitchStates
{
    FOOTSWITCH_IDLE,
    FOOTSWITCH_WAIT_RELEASE_1,
    FOOTSWITCH_WAIT_RELEASE_2
};

static const char *TAG = "app_footswitches";

typedef struct
{
    uint8_t state;
    uint32_t sample_counter;
    uint8_t last_binary_val;
    uint8_t current_bank;
    uint8_t index_pending;
} tFootswitchControl;

static tFootswitchControl FootswitchControl;

/****************************************************************************
 * NAME:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN:
 * NOTES:
 *****************************************************************************/
static uint8_t read_footswitch_input(uint8_t number, uint8_t *switch_state)
{
    uint8_t result = false;

    // other boards can use direct IO pin
    *switch_state = gpio_get_level(number);

    result = true;

    return result;
}

/****************************************************************************
 * NAME:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN:
 * NOTES:
 *****************************************************************************/
static void footswitch_handle_dual_mode(void)
{
    uint8_t value;

    switch (FootswitchControl.state)
    {
    case FOOTSWITCH_IDLE:
    default:
    {
        // read footswitches
        if (read_footswitch_input(FOOTSWITCH_1, &value))
        {
            if (value == 0)
            {
                ESP_LOGI(TAG, "Footswitch 1 pressed");

                // foot switch 1 pressed
                control_request_preset_down();

                // wait release
                FootswitchControl.sample_counter = 0;
                FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_1;
            }
        }

        if (FootswitchControl.state == FOOTSWITCH_IDLE)
        {
            if (read_footswitch_input(FOOTSWITCH_2, &value))
            {
                if (value == 0)
                {
                    ESP_LOGI(TAG, "Footswitch 2 pressed");

                    // foot switch 2 pressed, send event
                    control_request_preset_up();

                    // wait release
                    FootswitchControl.sample_counter = 0;
                    FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_2;
                }
            }
        }
    }
    break;

    case FOOTSWITCH_WAIT_RELEASE_1:
    {
        // read footswitch 1
        if (read_footswitch_input(FOOTSWITCH_1, &value))
        {
            if (value != 0)
            {
                FootswitchControl.sample_counter++;
                if (FootswitchControl.sample_counter == FOOTSWITCH_SAMPLE_COUNT)
                {
                    // foot switch released
                    FootswitchControl.state = FOOTSWITCH_IDLE;
                }
            }
            else
            {
                // reset counter
                FootswitchControl.sample_counter = 0;
            }
        }
    }
    break;

    case FOOTSWITCH_WAIT_RELEASE_2:
    {
        // read footswitch 2
        if (read_footswitch_input(FOOTSWITCH_2, &value))
        {
            if (value != 0)
            {
                FootswitchControl.sample_counter++;
                if (FootswitchControl.sample_counter == FOOTSWITCH_SAMPLE_COUNT)
                {
                    // foot switch released
                    FootswitchControl.state = FOOTSWITCH_IDLE;
                }
            }
            else
            {
                // reset counter
                FootswitchControl.sample_counter = 0;
            }
        }
    }
    break;
    }
}

/****************************************************************************
 * NAME:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN:
 * NOTES:
 *****************************************************************************/
static void footswitch_handle_quad_banked(void)
{
    uint8_t value;
    uint8_t binary_val = 0;
    uint8_t state_dly = 127;
    uint8_t state_comp = 127;
    uint8_t state_mod = 127;
    // static int8_t LONG_PRESS_TIME = 1000;
    // static uint8_t SHORT_PRESS_TIME = 200;
    // static int64_t press_start_time = 0;
    // int64_t press_duration;

    // read all 4 switches (and swap so 1 is pressed)
    read_footswitch_input(FOOTSWITCH_1, &value);
    if (value == 0)
    {
        //     press_start_time = esp_timer_get_time() / 1000;  // Tempo em ms

        // } else {
        //     press_duration = (esp_timer_get_time() / 1000) - press_start_time;
        //     if (press_duration < SHORT_PRESS_TIME) {
        binary_val |= 1;
        // } else if (press_duration >= LONG_PRESS_TIME) {
        //     midi_helper_adjust_param_via_midi(2, state_dly);
        //     if(state_dly==0){
        //         state_dly = 127;
        //     } else {
        //         state_dly = 0;
        //     }
        // }
    }

    read_footswitch_input(FOOTSWITCH_2, &value);
    if (value == 0)
    {
        binary_val |= 2;
    }

    read_footswitch_input(FOOTSWITCH_3, &value);
    if (value == 0)
    {
        binary_val |= 4;
    }

    read_footswitch_input(FOOTSWITCH_4, &value);
    if (value == 0)
    {
        binary_val |= 8;
    }

    read_footswitch_input(FOOTSWITCH_5, &value);
    if (value == 0)
    {
        // usb_set_preset(5);
        binary_val |= 16;
    }

    read_footswitch_input(FOOTSWITCH_6, &value);
    if (value == 0)
    {
        // usb_set_preset(6);
        binary_val |= 32;
    }

    // handle state
    switch (FootswitchControl.state)
    {
    case FOOTSWITCH_IDLE:
    {
        // any buttons pressed?
        if (binary_val != 0)
        {
            // check if A+B is pressed
            if (binary_val == 0x03)
            {
                if (FootswitchControl.current_bank > 0)
                {
                    // bank down
                    FootswitchControl.current_bank--;
                    ESP_LOGI(TAG, "Footswitch banked down %d", FootswitchControl.current_bank);
                }

                FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_1;
            }
            // check if C+D is pressed
            else if (binary_val == 0x06)
            {
                if (FootswitchControl.current_bank < BANK_MAXIMUM)
                {
                    // bank up
                    FootswitchControl.current_bank++;
                    ESP_LOGI(TAG, "Footswitch banked up %d", FootswitchControl.current_bank);
                }

                FootswitchControl.state = FOOTSWITCH_WAIT_RELEASE_1;
            }
            else if (binary_val == 0x09) // 1-4
            {
                midi_helper_adjust_param_via_midi(2, state_dly);
                if (state_dly == 0)
                {
                    state_dly = 127;
                }
                else if (state_dly == 127)
                {
                    state_dly = 0;
                }
            }
            else if (binary_val == 0x24) // 3-6
            {
                midi_helper_adjust_param_via_midi(32, state_mod);
                if (state_mod == 0)
                {
                    state_mod = 127;
                }
                else if (state_mod == 127)
                {
                    state_mod = 0;
                }
            }
               else if (binary_val == 0x12) // 2-5
            {
                midi_helper_adjust_param_via_midi(18, state_comp);
                if (state_comp == 0)
                {
                    state_comp = 127;
                }
                else if (state_comp == 127)
                {
                    state_comp = 0;
                }
            }
            else
            {
                // single button pressed, just store it. Preset select only happens on button release
                FootswitchControl.index_pending = binary_val;
            }
        }
        else
        {
            if (FootswitchControl.index_pending != 0)
            {
                uint8_t new_preset = FootswitchControl.current_bank * BANK_MODE_BUTTONS;

                // get the index from the bit set
                if ((FootswitchControl.index_pending & 0x01) != 0)
                {
                    // nothing needed
                }
                else if ((FootswitchControl.index_pending & 0x02) != 0)
                {
                    new_preset += 1;
                }
                else if ((FootswitchControl.index_pending & 0x04) != 0)
                {
                    new_preset += 2;
                }
                else if ((FootswitchControl.index_pending & 0x08) != 0)
                {
                    new_preset += 3;
                }
                else if ((FootswitchControl.index_pending & 0x10) != 0)
                {
                    new_preset += 4;
                }
                else if ((FootswitchControl.index_pending & 0x20) != 0)
                {
                    new_preset += 5;
                }

                // set the preset
                control_request_preset_index(new_preset);
                FootswitchControl.index_pending = 0;

                // give a little debounce time
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }
    }
    break;

    case FOOTSWITCH_WAIT_RELEASE_1:
    {
        // check if all buttons released
        if (binary_val == 0)
        {
            FootswitchControl.state = FOOTSWITCH_IDLE;
            FootswitchControl.index_pending = 0;

            // give a little debounce time
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    break;
    }
}

/****************************************************************************
 * NAME:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN:
 * NOTES:
 *****************************************************************************/
static void footswitch_handle_quad_binary(void)
{
    uint8_t value;
    uint8_t binary_val = 0;

    // read all 4 switches (and swap so 1 is pressed)
    read_footswitch_input(FOOTSWITCH_1, &value);
    if (value == 0)
    {
        binary_val |= 1;
    }

    read_footswitch_input(FOOTSWITCH_2, &value);
    if (value == 0)
    {
        binary_val |= 2;
    }

    read_footswitch_input(FOOTSWITCH_3, &value);
    if (value == 0)
    {
        binary_val |= 4;
    }

    read_footswitch_input(FOOTSWITCH_4, &value);
    if (value == 0)
    {
        binary_val |= 8;
    }

    // has it changed?
    if (binary_val != FootswitchControl.last_binary_val)
    {
        FootswitchControl.last_binary_val = binary_val;

        // set preset
        control_request_preset_index(binary_val);

        ESP_LOGI(TAG, "Footswitch binary set preset %d", binary_val);
    }

    // wait a little longer, so we don't jump around presets while inputs are being set
    vTaskDelay(pdMS_TO_TICKS(180));
}

/****************************************************************************
 * NAME:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN:
 * NOTES:
 *****************************************************************************/
void footswitch_task(void *arg)
{
    uint8_t mode;
    uint8_t value;
    uint32_t reset_timer = 0;

    ESP_LOGI(TAG, "Footswitch task start");

    // let things settle
    vTaskDelay(pdMS_TO_TICKS(1000));

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_43B
    // 4.3B doesn't have enough IO, only supports dual mode
    mode = FOOTSWITCH_MODE_DUAL_UP_DOWN;
#else
    // others, get the currently configured mode from web config
    mode = control_get_config_footswitch_mode();
#endif

    while (1)
    {
        switch (mode)
        {
        case FOOTSWITCH_MODE_DUAL_UP_DOWN:
        default:
        {
            // run dual mode next/previous
            footswitch_handle_dual_mode();
        }
        break;

        case FOOTSWITCH_MODE_QUAD_BANKED:
        {
            // run 4 switch with bank up/down
            footswitch_handle_quad_banked();
        }
        break;

        case FOOTSWITCH_MODE_QUAD_BINARY:
        {
            // run 4 switch binary mode
            footswitch_handle_quad_binary();
        }
        break;
        }

        // check for button held for data reset
        if (read_footswitch_input(FOOTSWITCH_1, &value))
        {
            if (value == 0)
            {
                reset_timer++;

                if (reset_timer > BUTTON_FACTORY_RESET_TIME)
                {
                    ESP_LOGI(TAG, "Config Reset to default");
                    control_set_default_config();

                    // save and reboot
                    control_save_user_data(1);
                }
            }
            else
            {
                reset_timer = 0;
            }
        }

        // handle leds from this task, to save wasting ram on another task for it
        leds_handle();

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/****************************************************************************
 * NAME:
 * DESCRIPTION:
 * PARAMETERS:
 * RETURN:
 * NOTES:
 *****************************************************************************/
void footswitches_init(void)
{
    memset((void *)&FootswitchControl, 0, sizeof(FootswitchControl));
    FootswitchControl.state = FOOTSWITCH_IDLE;

#if CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_ZERO || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_WAVESHARE_169 || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_DEVKITC || CONFIG_TONEX_CONTROLLER_HARDWARE_PLATFORM_M5ATOMS3R
    // init GPIO
    gpio_config_t gpio_config_struct;

    gpio_config_struct.pin_bit_mask = (((uint64_t)1 << FOOTSWITCH_1) | ((uint64_t)1 << FOOTSWITCH_2) | ((uint64_t)1 << FOOTSWITCH_3) | ((uint64_t)1 << FOOTSWITCH_4) | ((uint64_t)1 << FOOTSWITCH_5) | ((uint64_t)1 << FOOTSWITCH_6));
    gpio_config_struct.mode = GPIO_MODE_INPUT;
    gpio_config_struct.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpio_config_struct);
#endif

    // init leds
    leds_init();

    // create task
    xTaskCreatePinnedToCore(footswitch_task, "FOOT", FOOTSWITCH_TASK_STACK_SIZE, NULL, FOOTSWITCH_TASK_PRIORITY, NULL, 1);
}
