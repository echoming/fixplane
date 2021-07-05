#include <stdio.h>
#include "wifiiot_i2c.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include <unistd.h>
#include "wifiiot_errno.h"
#include "wifiiot_gpio_w800.h"


//检测按键低电平并亮灯两秒
static void *TrafficLightTask(const char* arg)
{
    (void)arg;
    printf("[user_key] user_key_task...\n");

    GpioInit();

    GpioSetDir(WIFI_IOT_GPIO_PB_05, WIFI_IOT_GPIO_DIR_INPUT);//key
    IoSetPull(WIFI_IOT_GPIO_PB_05, WIFI_IOT_GPIO_ATTR_PULLHIGH);

    GpioSetDir(WIFI_IOT_GPIO_PB_10, WIFI_IOT_GPIO_DIR_OUTPUT);//red
    GpioSetOutputVal(WIFI_IOT_GPIO_PB_10, WIFI_IOT_GPIO_VALUE0);

    GpioSetDir(WIFI_IOT_GPIO_PA_01, WIFI_IOT_GPIO_DIR_OUTPUT);//green
    GpioSetOutputVal(WIFI_IOT_GPIO_PA_01, WIFI_IOT_GPIO_VALUE0);

    GpioSetDir(WIFI_IOT_GPIO_PB_01, WIFI_IOT_GPIO_DIR_OUTPUT);//yellow
    GpioSetOutputVal(WIFI_IOT_GPIO_PB_01, WIFI_IOT_GPIO_VALUE0);

    GpioSetDir(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_DIR_OUTPUT);//led
    GpioSetOutputVal(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_VALUE1);

    WifiIotGpioValue val;

    uint32_t startPressTicks = 0;
    WifiIotGpioValue lastValue;

    GpioGetInputVal(WIFI_IOT_GPIO_PB_05, &lastValue);

    while (1) {
        uint32_t ticks = osKernelGetTickCount();

        WifiIotGpioValue value;
        GpioGetInputVal(WIFI_IOT_GPIO_PB_05, &value);
        if (value == WIFI_IOT_GPIO_VALUE0) { // 低电平
            if (value != lastValue) {      // 边缘
                startPressTicks = ticks;
            }
            uint32_t pressSeconds = (ticks - startPressTicks) / osKernelGetTickFreq();

            if (pressSeconds >= 3) {
                printf("[user_key] pressed 3s!\n");
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_VALUE0);
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_10, WIFI_IOT_GPIO_VALUE1);
                GpioSetOutputVal(WIFI_IOT_GPIO_PA_01, WIFI_IOT_GPIO_VALUE1);
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_01, WIFI_IOT_GPIO_VALUE1);
                osDelay(1000);
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_08, WIFI_IOT_GPIO_VALUE1);
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_10, WIFI_IOT_GPIO_VALUE0);
                GpioSetOutputVal(WIFI_IOT_GPIO_PA_01, WIFI_IOT_GPIO_VALUE0);
                GpioSetOutputVal(WIFI_IOT_GPIO_PB_01, WIFI_IOT_GPIO_VALUE0);
            }
        }
        lastValue = value;

        osDelay(100);
    }

    return NULL;
}



static void User_Key(void)
{
    osThreadAttr_t attr = {0};

    printf("[TrafficLightTask] TrafficLightTask...\n");

    attr.name = "TrafficLightTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)TrafficLightTask, NULL, &attr) == NULL)
    {
        printf("[user_key] Falied to create TrafficLightTask!\n");
    }
}

SYS_RUN(User_Key);
