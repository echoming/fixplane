/*
 * Copyright (c) 2020, HiHope Community.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>

#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_gpio_w800.h"

#define LED_TASK_STACK_SIZE 512
#define LED_TASK_PRIO 25

enum LedState {
    LED_ON = 0,
    LED_OFF,
    LED_SPARK,
};

enum LedState g_ledState = LED_SPARK;

static void* GpioTask(const char* arg)
{
    (void)arg;
    while (1) {
        switch (g_ledState) {
            case LED_ON:
                //printf(" LED_ON! \n");
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_VALUE0);
                osDelay(500);
                break;
            case LED_OFF:
                //printf(" LED_OFF! \n");
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_VALUE1);
                osDelay(500);
                break;
            case LED_SPARK:
                printf(" LED_SPARK! \n");
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_VALUE0);
                osDelay(500);
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_VALUE1);
                osDelay(500);
                break;
            default:
                osDelay(500);
                break;
        }
    }

    return NULL;
}

static void GpioIsr(char* arg)
{
    (void)arg;

    enum LedState nextState = LED_SPARK;

    printf(" GpioIsr entry\n");

    GpioSetIsrMask(WIFI_IOT_GPIO_PB_09, 0);
    switch (g_ledState) {
        case LED_ON:
            nextState = LED_OFF;
            break;
        case LED_OFF:
            nextState = LED_ON;
            break;
        case LED_SPARK:
            nextState = LED_OFF;
            break;
        default:
            break;
    }

    g_ledState = nextState;
}

static void GpioExampleEntry(void)
{
    osThreadAttr_t attr;

    GpioInit();
    GpioSetDir(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_DIR_OUTPUT); // output is 0 PB08 control led

//    GpioSetDir(WIFI_IOT_GPIO_PB_09, WIFI_IOT_GPIO_DIR_INPUT); // input is PB09
//    IoSetPull(WIFI_IOT_GPIO_PB_09, WIFI_IOT_GPIO_ATTR_PULLHIGH);
//    GpioRegisterIsrFunc(WIFI_IOT_GPIO_PB_09, WIFI_IOT_INT_TYPE_EDGE, WIFI_IOT_GPIO_EDGE_FALL_LEVEL_LOW, GpioIsr, NULL);

    attr.name = "GpioTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = LED_TASK_STACK_SIZE;
    attr.priority = LED_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)GpioTask, NULL, &attr) == NULL) {
        printf("[GpioExample] Falied to create GpioTask!\n");
    }
}

SYS_RUN(GpioExampleEntry); // if test add it
