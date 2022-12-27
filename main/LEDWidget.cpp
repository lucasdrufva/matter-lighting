/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *
 *    Modifications copyright (c) 2022 Lucas Drufva
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

#include "LEDWidget.h"

#define RELAY_PIN GPIO_NUM_27

static const char * TAG = "LEDWidget";

void LEDWidget::Init(void)
{
    mState      = false;
    gpio_set_direction(RELAY_PIN, GPIO_MODE_OUTPUT);
}

void LEDWidget::Set(bool state)
{
    ESP_LOGI(TAG, "Setting state to %d", state ? 1 : 0);
    if (state == mState)
        return;

    mState = state;

    DoSet();
}

void LEDWidget::Toggle()
{
    ESP_LOGI(TAG, "Toggling state to %d", !mState);
    mState = !mState;

    DoSet();
}

bool LEDWidget::IsTurnedOn()
{
    return this->mState;
}


void LEDWidget::DoSet(void)
{
    gpio_set_level(RELAY_PIN, mState);
}
