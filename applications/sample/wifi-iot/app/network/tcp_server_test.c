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

#include "net_demo_common.h"
#include "net_common.h"

#include "mes_que.h"

static char request[128] = "";

int HeartStatus = osOK;
int sockfd; // TCP socket
int connfd;
int SocketStatus = osOK;


static MSGQUEUE_CTL_t mes;
osMessageQueueId_t MesQue_Ctl;


void TcpServerTest(unsigned short port)
{
    int retval = 0;
    int backlog = 1;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    int connfd = -1;



    struct sockaddr_in clientAddr = {0};
    socklen_t clientAddrlen = sizeof(clientAddr);

    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //accept any getin

    retval = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));//bing port
    if (retval<0) {
        printf("bind failed!\r\n");
        goto do_cleanup;
    }
    printf("bind %s success!\r\n", port);



    retval = listen(sockfd,backlog);
    if (retval < 0)
    {
        printf("listen failed!\r\n");
        goto do_cleanup;
    }
    printf("listen with  %d backlog success!\r\n", backlog);

    connfd = accept(sockfd,(struct sockaddr *)&clientAddr, sizeof(clientAddr));
    if (connfd <0)
    {
        printf("accept faild,connfd = %d!\r\n", connfd);
        goto do_cleanup;
    }
    printf("accept success,connfd = %d!\r\n", connfd);
    printf("client info : host = %s, port = %d\r\n",inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    HeartStatus=osOK;
    while (HeartStatus == osOK)
    {    

        retval = recv(connfd, request, sizeof(request), 0);
        if (retval <= 0) {
            printf("recv request failed %ld!\r\n", retval);
            goto do_disconnect;
        }
        printf("recv request{%s} from client done!\r\n", request);

        //put message into queue
        mes.Buf[0]=request[0];
        mes.Buf[1]=request[1];
        mes.Buf[2]=request[2];
        mes.Buf[3]=request[3];
        mes.Buf[4]=request[4];
        mes.Buf[5]=request[5];
        mes.Idx = msg_Idx;


            if( osMessageQueuePut(MesQue_Ctl,&mes,0U,100) == osOK){printf("msg to queue\r\n");}
            else{printf("message into queue fail\r\n");};

            retval = send(connfd, request, strlen(request), 0);
            if (retval < 0) {
                printf("send response failed!\r\n");
                goto do_disconnect;
            }
            printf("send response{%s} %ld to client done!\r\n", request, retval);

    }    


do_disconnect:
    SocketStatus=osError;
    osDelay(500);
    close(connfd);
    osDelay(500);
    printf("do_disconnect...\r\n");


do_cleanup:
    printf("do_cleanup...\r\n");
    close(sockfd);
    osDelay(2000);
}

SERVER_TEST_DEMO(TcpServerTest);