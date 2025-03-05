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
//#include "leds.h"
#include "midi_helper.h"
#include "i2c-lcd.h"
#include "usb_tonex_one.h"
#include "tonex_params.h"

#define FOOTSWITCH_TASK_STACK_SIZE (3 * 1024)
#define FOOTSWITCH_SAMPLE_COUNT 5 // 20 msec per sample
#define BANK_MODE_BUTTONS 6
#define BANK_MAXIMUM (MAX_PRESETS / BANK_MODE_BUTTONS)
#define BUTTON_FACTORY_RESET_TIME 500 // * 20 msec = 10 secs

#define DEBOUNCE_TIME_MS 50       // Tempo de debounce (50ms)
#define DOUBLE_CLICK_TIME_MS 500  // Tempo máximo entre cliques para duplo clique (500ms)
#define LONG_PRESS_TIME_MS 2000   // Tempo mínimo para clique longo (2000ms)

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

// Função para detectar clique simples, duplo e longo
static uint8_t detect_button_click(uint8_t BUTTON_GPIO)
{
    static int64_t last_press_time = 0;
    static int click_count = 0;
    int64_t press_start_time = esp_timer_get_time() / 1000; // Tempo inicial em ms
    
    // Aguarda o botão ser solto ou detectar clique longo
    while (gpio_get_level(BUTTON_GPIO) == 0) {
        int64_t elapsed_time = (esp_timer_get_time() / 1000) - press_start_time;
        if (elapsed_time >= LONG_PRESS_TIME_MS) {
            while (gpio_get_level(BUTTON_GPIO) == 0); // Espera soltar
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS));
            click_count = 0;  // Reseta o contador para ignorar outros eventos
            return 3;
        }
    }
    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_TIME_MS)); // Tempo de debounce

    int64_t press_time = esp_timer_get_time() / 1000;

    // Verifica se é um clique duplo
    if ((press_time - last_press_time) <= DOUBLE_CLICK_TIME_MS) {
        click_count++;
    } else {
        click_count = 1;  // Reset para um novo clique
    }

    last_press_time = press_time;

    if (click_count == 2) {
        //printf("Duplo clique detectado!\n");        
        click_count = 0;  // Reseta para evitar triplo clique
        return 2;
    } else {
        // Aguardar para ver se um segundo clique ocorre
        int64_t wait_time = esp_timer_get_time() / 1000;
        while ((esp_timer_get_time() / 1000 - wait_time) < DOUBLE_CLICK_TIME_MS) {
            if (gpio_get_level(BUTTON_GPIO) == 0) {
                // Se outro clique ocorrer, incrementar o contador e sair do loop
                click_count++;
                
                last_press_time = esp_timer_get_time() / 1000;   
                if (click_count == 2) {
                  click_count = 0;
                  //printf("Clique duplo detectado!\n");
                  return 2;
                 }              
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(10)); // Pequeno delay para não travar a CPU
          }
          if (click_count == 1) {
              click_count = 0;
              //printf("Clique simples detectado!\n");
              return 1;
          }
    } 
    return 0;
}
 

static void footswitch_handle_quad_banked(void)
{
    uint8_t value;
    uint8_t binary_val = 0;
    tTonexParameter* param_ptr;
    float param;
    u_int8_t st_button;
   
    // read all 4 switches (and swap so 1 is pressed)
    read_footswitch_input(FOOTSWITCH_1, &value);
    if (value == 0)
    {
        st_button=detect_button_click(FOOTSWITCH_1);
        if (st_button == 1){
            binary_val |= 1;

        }else if (st_button == 2){
            if (FootswitchControl.current_bank > 0)
                {
                    // bank down
                    FootswitchControl.current_bank--;
                    ESP_LOGI(TAG, "Footswitch banked down %d", FootswitchControl.current_bank);
                }          
        }else if (st_button == 3){
            tonex_params_get_locked_access(&param_ptr);
            param=param_ptr[TONEX_PARAM_DELAY_ENABLE].Value;
            if (param == 1.0f){
                param = 0.0f;
            }else{
                param = 1.0f;
            }
            usb_modify_parameter(TONEX_PARAM_DELAY_ENABLE, param); 
            tonex_params_release_locked_access();
        }
        st_button=0;
    }

    read_footswitch_input(FOOTSWITCH_2, &value);
    if (value == 0)
    {
        st_button=detect_button_click(FOOTSWITCH_2);
        if (st_button == 1){
            binary_val |= 2;

        }else if (st_button == 2){
            lcd_put_cur(0, 0);
            lcd_send_string("EDuardo");
        }else if (st_button == 3){
            tonex_params_get_locked_access(&param_ptr);
            param=param_ptr[TONEX_PARAM_COMP_ENABLE].Value;
            if (param == 1.0f){
                param = 0.0f;
            }else{
                param = 1.0f;
            }
            usb_modify_parameter(TONEX_PARAM_COMP_ENABLE, param); 
            tonex_params_release_locked_access();
        }
        st_button=0;
    }

    read_footswitch_input(FOOTSWITCH_3, &value);
    if (value == 0)
    {
        st_button=detect_button_click(FOOTSWITCH_3);
        if (st_button == 1){
            binary_val |= 4;
        }else if (st_button == 2){
            if (FootswitchControl.current_bank < BANK_MAXIMUM)
            {
                // bank up
                FootswitchControl.current_bank++;
                ESP_LOGI(TAG, "Footswitch banked up %d", FootswitchControl.current_bank);
            }         
        }else if (st_button == 3){
            tonex_params_get_locked_access(&param_ptr);
            param=param_ptr[TONEX_PARAM_MODULATION_ENABLE].Value;
            if (param == 1.0f){
                param = 0.0f;
            }else{
                param = 1.0f;
            }
            usb_modify_parameter(TONEX_PARAM_MODULATION_ENABLE, param); 
            tonex_params_release_locked_access();
        }
        st_button=0;
    }

    read_footswitch_input(FOOTSWITCH_4, &value);
    if (value == 0)
    {
        st_button=detect_button_click(FOOTSWITCH_4);
        if (st_button == 1){
            binary_val |= 8;

        }else if (st_button == 2){
            //NOTHING        
        }else if (st_button == 3){
            tonex_params_get_locked_access(&param_ptr);
            param=param_ptr[TONEX_PARAM_REVERB_ENABLE].Value;
            if (param == 1.0f){
                param = 0.0f;
            }else{
                param = 1.0f;
            }
            usb_modify_parameter(TONEX_PARAM_REVERB_ENABLE, param); 
            tonex_params_release_locked_access();
        }
        st_button=0;
    }

    read_footswitch_input(FOOTSWITCH_5, &value);
    if (value == 0)
    {
        st_button=detect_button_click(FOOTSWITCH_5);
        if (st_button == 1){
            binary_val |= 16;

        }else if (st_button == 2){
            if (FootswitchControl.current_bank > 0)
                {
                    // bank down
                    FootswitchControl.current_bank--;
                    ESP_LOGI(TAG, "Footswitch banked down %d", FootswitchControl.current_bank);
                }          
        }else if (st_button == 3){
            tonex_params_get_locked_access(&param_ptr);
            param=param_ptr[TONEX_PARAM_NOISE_GATE_ENABLE].Value;
            if (param == 1.0f){
                param = 0.0f;
            }else{
                param = 1.0f;
            }
            usb_modify_parameter(TONEX_PARAM_NOISE_GATE_ENABLE, param); 
            tonex_params_release_locked_access();
        }
        st_button=0;
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
            // check if 1+2 is pressed
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
            // check if 2+3 is pressed
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

    // others, get the currently configured mode from web config
    mode = control_get_config_footswitch_mode();

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
        //leds_handle();

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

   // init GPIO
    gpio_config_t gpio_config_struct;

    gpio_config_struct.pin_bit_mask = (((uint64_t)1 << FOOTSWITCH_1) | ((uint64_t)1 << FOOTSWITCH_2) | ((uint64_t)1 << FOOTSWITCH_3) | ((uint64_t)1 << FOOTSWITCH_4) | ((uint64_t)1 << FOOTSWITCH_5) | ((uint64_t)1 << FOOTSWITCH_6));
    gpio_config_struct.mode = GPIO_MODE_INPUT;
    gpio_config_struct.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_struct.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&gpio_config_struct);


    // init leds
    //leds_init();

    // create task
    xTaskCreatePinnedToCore(footswitch_task, "FOOT", FOOTSWITCH_TASK_STACK_SIZE, NULL, FOOTSWITCH_TASK_PRIORITY, NULL, 1);
}
