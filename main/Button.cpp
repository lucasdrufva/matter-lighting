/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "Button.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include <sys/_stdint.h>

#define BUTTON_PIN GPIO_NUM_21

#define GPIO_INPUT_PIN_SEL (1ULL << BUTTON_PIN)
#define ESP_INTR_FLAG_DEFAULT 0

static const char * TAG = "Button";

static Button::ButtonPressCallback button_press_short_handler = nullptr;
static Button::ButtonPressCallback button_press_reset_handler = nullptr;

volatile int64_t debounce_time = 0;

static void IRAM_ATTR gpio_isr_handler(void * arg)
{
    if (button_press_short_handler != nullptr and esp_timer_get_time() - debounce_time > 1'000'000)
    {
        debounce_time = esp_timer_get_time();
        button_press_short_handler();
    }
}

void Button::Init()
{
    /* Initialize button interrupt*/
    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    // install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(static_cast<gpio_num_t>(BUTTON_PIN), gpio_isr_handler, (void *) BUTTON_PIN);

    ESP_LOGI(TAG, "Button initialized..");

    if(gpio_get_level(BUTTON_PIN) == 1)
    {
        button_press_reset_handler();
    }
}

void Button::SetButtonPressShortCallback(ButtonPressCallback button_callback)
{
    if (button_callback != nullptr)
    {
        ESP_LOGI(TAG, "Button short callback is now set");
        button_press_short_handler = button_callback;
    }
}

void Button::SetButtonPressResetCallback(ButtonPressCallback button_callback)
{
    if (button_callback != nullptr)
    {
        ESP_LOGI(TAG, "Button reset callback is now set");
        button_press_reset_handler = button_callback;
    }
}
