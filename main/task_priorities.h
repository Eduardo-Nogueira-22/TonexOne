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


#ifndef _TASK_PRIO_H
#define _TASK_PRIO_H

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DAEMON_TASK_PRIORITY        (tskIDLE_PRIORITY + 4)
#define USB_CLASS_TASK_PRIORITY         (tskIDLE_PRIORITY + 4)
#define DISPLAY_TASK_PRIORITY           (tskIDLE_PRIORITY + 2)
#define CTRL_TASK_PRIORITY              (tskIDLE_PRIORITY + 3)
#define MIDI_SERIAL_TASK_PRIORITY       (tskIDLE_PRIORITY + 2)
#define FOOTSWITCH_TASK_PRIORITY        (tskIDLE_PRIORITY + 1)
#define WIFI_TASK_PRIORITY              (tskIDLE_PRIORITY + 1)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif