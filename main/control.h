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

#pragma once

void control_init(void);
void control_load_config(void);

enum Skins
{
#if CONFIG_TONEX_CONTROLLER_SKINS_AMP    
    // Amps
    AMP_SKIN_JCM800,
    AMP_SKIN_TWIN_REVERB,
    AMP_SKIN_2001RB,
    AMP_SKIN_5150,
    AMP_SKIN_B18N,
    AMP_SKIN_BLUES_DELUXE,
    AMP_SKIN_DEVILLE,
    AMP_SKIN_DUAL_RECTIFIER,
    AMP_SKIN_GOLD_FINGER,
    AMP_SKIN_INVADER,
    AMP_SKIN_JAZZ_CHORUS,
    AMP_SKIN_OR_50,
    AMP_SKIN_POWERBALL,
    AMP_SKIN_PRINCETON,
    AMP_SKIN_SVTCL,
    AMP_SKIN_MAVERICK,
    AMP_SKIN_MK3,
    AMP_SKIN_SUPERBASS,
    AMP_SKIN_DUMBLE,
    AMP_SKIN_JETCITY,
    AMP_SKIN_AC30,
    AMP_SKIN_EVH5150,
    AMP_SKIN_2020,
    AMP_SKIN_PINK_TACO,
    AMP_SKIN_SUPRO_50,
    AMP_SKIN_DIEZEL,
#endif

#if CONFIG_TONEX_CONTROLLER_SKINS_PEDAL
    // Pedals
    PEDAL_SKIN_ARION,
    PEDAL_SKIN_BIGMUFF,
    PEDAL_SKIN_DARKGLASS,
    PEDAL_SKIN_DOD,
    PEDAL_SKIN_EHX,
    PEDAL_SKIN_FENDER,
    PEDAL_SKIN_FULLTONE,
    PEDAL_SKIN_FZS,
    PEDAL_SKIN_JHS,
    PEDAL_SKIN_KLON,
    PEDAL_SKIN_LANDGRAF,
    PEDAL_SKIN_MXR,
    PEDAL_SKIN_MXR2,
    PEDAL_SKIN_OD1,
    PEDAL_SKIN_PLIMSOUL,
    PEDAL_SKIN_ROGERMAYER,
    PEDAL_SKIN_SEYMOUR,
    PEDAL_SKIN_STRYMON,
    PEDAL_SKIN_TREX,
    PEDAL_SKIN_TUBESCREAMER,
    PEDAL_SKIN_WAMPLER,
    PEDAL_SKIN_ZVEX,
#endif 

    SKIN_MAX        // must be last
};

enum BluetoothModes
{
    BT_MODE_DISABLED,
    BT_MODE_CENTRAL,
    BT_MODE_PERIPHERAL,
};

enum FootswitchModes
{
    FOOTSWITCH_MODE_DUAL_UP_DOWN,       // next/previous
    FOOTSWITCH_MODE_QUAD_BANKED,        // like Mvave Choc with bank select from 1+2 and 3+4
    FOOTSWITCH_MODE_QUAD_BINARY,        // direct binary selection from 4 switches
    FOOTSWITCH_MODE_LAST
};

enum WiFiModes
{
    WIFI_MODE_ACCESS_POINT_TIMED,       // access point for 1 minute on boot
    WIFI_MODE_STATION,                  // station mode
    WIFI_MODE_ACCESS_POINT              // access point, no timeout
};

enum ScreenRotation
{
    SCREEN_ROTATION_0,
    SCREEN_ROTATION_180,
    // 90 and 270 one day maybe but needs big UI changes
    //SCREEN_ROTATION_90,
    //SCREEN_ROTATION_270,
    SCREEN_ROTATION_MAX,
};

#define MAX_WIFI_SSID_PW       65   

// thread safe public API
void control_request_preset_up(void);
void control_request_preset_down(void);
void control_request_preset_index(uint8_t index);
void control_set_usb_status(uint32_t status);
void control_set_bt_status(uint32_t status);
void control_set_wifi_status(uint32_t status);
void control_set_amp_skin_index(uint32_t status);
void control_set_skin_next(void);
void control_set_skin_previous(void);
void control_save_user_data(uint8_t reboot);
void control_sync_preset_details(uint16_t index, char* name);
void control_set_user_text(char* text);

// config API
void control_set_default_config(void);
void control_set_config_btmode(uint32_t status);
void control_set_config_mv_choc_enable(uint32_t status);
void control_set_config_xv_md1_enable(uint32_t status);
void control_set_config_bt_custom_enable(uint32_t status);
void control_set_config_custom_bt_name(char* name);
void control_set_config_serial_midi_enable(uint32_t status);
void control_set_config_serial_midi_channel(uint32_t status);
void control_set_config_toggle_bypass(uint32_t status);
void control_set_config_footswitch_mode(uint32_t mode);
void control_set_config_enable_bt_midi_CC(uint32_t status);
void control_set_config_wifi_mode(uint32_t mode);
void control_set_config_wifi_ssid(char* name);
void control_set_config_wifi_password(char* name);
void control_set_screen_rotation(uint32_t rot);

uint8_t control_get_config_bt_mode(void);
uint8_t control_get_config_bt_mvave_choc_enable(void);
uint8_t control_get_config_bt_xvive_md1_enable(void);
uint8_t control_get_config_bt_custom_enable(void);
void control_get_config_custom_bt_name(char* name);
uint8_t control_get_config_double_toggle(void);
uint8_t control_get_config_midi_serial_enable(void);
uint8_t control_get_config_midi_channel(void);
uint8_t control_get_config_footswitch_mode(void);
uint8_t control_get_config_enable_bt_midi_CC(void);
uint8_t control_get_config_wifi_mode(void);
void control_get_config_wifi_ssid(char* name);
void control_get_config_wifi_password(char* name);
uint8_t control_get_config_screen_rotation(void);