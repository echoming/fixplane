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
#include "wifiiot_pwm.h"

#include "mes_que.h"
#include "data_handle.h"

#define CORE_TASK_STACK_SIZE 512
#define CORE_TASK_PRIO 25


#define Thres_PIN_NAME  WIFI_IOT_GPIO_PB_01
#define ROLL_PIN_NAME   WIFI_IOT_GPIO_PB_02
#define PITCH_PIN_NAME  WIFI_IOT_GPIO_PB_03

#define PWM_PORT_Thres   WIFI_IOT_PWM_PORT_PWM1
#define PWM_PORT_ROLL    WIFI_IOT_PWM_PORT_PWM2
#define PWM_PORT_PITCH   WIFI_IOT_PWM_PORT_PWM3
#define PWM_FREQ_DIVITION 200
#define PWM_Thres_Base  0
#define PWM_ROLL_Base   75
#define PWM_PITCH_Base  75

extern osMessageQueueId_t MesQue_Ctl;

extern int SocketStatus;


static void* CoreTask(const char* arg)
{
    (void)arg;
    
    MSGQUEUE_CTL_t mesg;
    char Buffer[10]="";
    unsigned short pwm_value_temp=0;

    // printf("Thread fixplane started!\r\n");
    // printf("unsigned %d ok !",PWM_FREQ_DIVITION);
    PwmInit(PWM_PORT_Thres); // Thrs
    PwmInit(PWM_PORT_ROLL); // ROLL
    PwmInit(PWM_PORT_PITCH); // YAW

    PwmStart(PWM_PORT_Thres, PWM_Thres_Base, PWM_FREQ_DIVITION);
    PwmStart(PWM_PORT_ROLL, PWM_ROLL_Base, PWM_FREQ_DIVITION);
    PwmStart(PWM_PORT_PITCH, PWM_PITCH_Base, PWM_FREQ_DIVITION);

    while (1)
    {

            if(osMessageQueueGet(MesQue_Ctl,&mesg,NULL,0U) == osOK)
            {
                Buffer[0]=mesg.Buf[0];
                Buffer[1]=mesg.Buf[1];
                Buffer[2]=mesg.Buf[2];
                Buffer[3]=mesg.Buf[3];
                Buffer[4]=mesg.Buf[4];
                Buffer[5]=mesg.Buf[5];
                switch ((unsigned short)Buffer[1]-48)
                {
                case 1:
                    pwm_value_temp=DatatoNumber(Buffer);
                    // printf("pwm duty thres is %u !",pwm_value_temp);
                    PwmStart(PWM_PORT_Thres, pwm_value_temp, PWM_FREQ_DIVITION);
                    break;
                case 2:
                    pwm_value_temp=DatatoNumber(Buffer);
                    // printf("pwm duty roll is %u !",pwm_value_temp);

                    PwmStart(PWM_PORT_ROLL, 25+pwm_value_temp, PWM_FREQ_DIVITION);   

                    break;
                case 3:
                    pwm_value_temp=DatatoNumber(Buffer);
                    // printf("pwm duty pitch is %u !",pwm_value_temp);
                    PwmStart(PWM_PORT_PITCH,125-pwm_value_temp, PWM_FREQ_DIVITION);                 /* code */
                    break;       
                default:
                    // printf("switch failed.");
                    break;
                }

            //printf("recv message{%s} from queue done!\r\n",Buffer);
            
            }

            if (SocketStatus == osError)
            {
                PwmStart(PWM_PORT_Thres, 0, PWM_FREQ_DIVITION);
                PwmStart(PWM_PORT_ROLL, 25+20, PWM_FREQ_DIVITION);  
                PwmStart(PWM_PORT_PITCH,125-20, PWM_FREQ_DIVITION);                 /* code */ 
            }
            
        
        osDelay(1);

    }
    


    return NULL;
}


static void FixplaneEntry(void)
{
    osThreadAttr_t attr;

    MesQue_Ctl=osMessageQueueNew(MSGQUEUE_OBJECTS,sizeof(MSGQUEUE_CTL_t),NULL);


    GpioInit();

    // set Red/Green/Blue LED pin as output
    GpioSetDir(Thres_PIN_NAME, WIFI_IOT_GPIO_DIR_OUTPUT);
    GpioSetDir(ROLL_PIN_NAME, WIFI_IOT_GPIO_DIR_OUTPUT);
    GpioSetDir(PITCH_PIN_NAME, WIFI_IOT_GPIO_DIR_OUTPUT);



    attr.name = "CoreTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = CORE_TASK_STACK_SIZE;
    attr.priority = CORE_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)CoreTask, NULL, &attr) == NULL) {
        printf("[GpioExample] Falied to create CoreTask!\n");
    }
}

SYS_RUN(FixplaneEntry); // if test add it
