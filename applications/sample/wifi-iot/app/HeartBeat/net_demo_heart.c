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

//#include "net_params.h"
//#include "wifi_connecter.h"
//#include "wifi_hotspotm.h"
//#include "net_demo_common.h"
int HotspotStatus=osError;
int HeartBeatFlag=osError;
int HeartStatus=osOK;
int StartBeatFlag=osError;

static void NetDemoHeartTask(void *arg)
{
    (void)arg;

    uint32_t osKernelTickCountS;
    uint32_t osKernelTickCountE;



    while (1)
    {
        while(HotspotStatus == osError){osDelay(200);};
 
        if(HeartBeatFlag == osOK)
        {
            osKernelTickCountS = osKernelGetTickCount();
            osKernelTickCountE = osKernelTickCountS;
            HeartBeatFlag=osError;
            StartBeatFlag=osOK;
            printf("Beat Start\r\n");
        }

        if (StartBeatFlag==osOK)
        {
            osKernelTickCountE = osKernelGetTickCount();
            printf("tickS is %ld ,tickE is %ld !\r\n!",osKernelTickCountS,osKernelTickCountE);
            if(  (osKernelTickCountE-osKernelTickCountS)  > 2000  )
            {
                StartBeatFlag = osError;
                HeartStatus = osError;
                printf("Heart Beat fail\r\n");
            }
        }
        
        osDelay(200);

    }
    

}

static void NetDemoHeartEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "NetDemoHeartTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 5120;
    attr.priority = osPriorityNormal;

    if (osThreadNew(NetDemoHeartTask, NULL, &attr) == NULL) {
        printf("[NetDemoHeartEntry] Falied to create NetDemoHeartTask!\n");
    }
}

SYS_RUN(NetDemoHeartEntry);

