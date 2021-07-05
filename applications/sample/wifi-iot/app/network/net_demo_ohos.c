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
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "net_params.h"
//#include "wifi_connecter.h"
#include "wifi_hotspotm.h"
#include "net_demo_common.h"
#include "net_common.h"



    

static void NetDemoTask(void *arg)
{
    (void)arg;
printf("Thread network started!\r\n");
    while(1)
    {
        // WifiDeviceConfig config = {0};

        // // 准备AP的配置参数
        // strcpy(config.ssid, PARAM_HOTSPOT_SSID);
        // strcpy(config.preSharedKey, PARAM_HOTSPOT_PSK);
        // config.securityType = PARAM_HOTSPOT_TYPE;

        CreatHotspot();

        // osDelay(10);

        // int netId = ConnectToHotspot(&config);

        int timeout = 5;
        while (timeout--) {
            printf("After %d seconds, I will start %s test!\r\n", timeout, GetNetDemoName());
            osDelay(100);
        }

        while(1)
        {
            //NetDemoTest(PARAM_SERVER_PORT, PARAM_SERVER_ADDR);
        }
        



        printf("stop AP ...");
        StopHotspot();
        printf("stop AP done!");

        osDelay(2000);
    }

}

    
static void NetDemoEntry(void)
{
    
    osThreadId_t osThreadId_wifi;
    osThreadAttr_t attrN;

    attrN.name = "NetDemoTask";
    attrN.attr_bits = 0U;
    attrN.cb_mem = NULL;
    attrN.cb_size = 0U;
    attrN.stack_mem = NULL;
    attrN.stack_size = 10240;
    attrN.priority = osPriorityNormal;

    osThreadId_wifi=osThreadNew(NetDemoTask, NULL, &attrN);
    if ( osThreadId_wifi== NULL) {
        printf("[NetDemoEntry] Falied to create NetDemoTask!\n");
    }



}

SYS_RUN(NetDemoEntry);



