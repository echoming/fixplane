#ifndef _MES_QUE_H
#define _MES_QUE_H

#include "cmsis_os2.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

    #define msg_Idx 0x03

    #define MSGQUEUE_OBJECTS 30                     // number of Message Queue Objects
    typedef struct {                                // object data type
    char Buf[6];
    uint8_t Idx;
    } MSGQUEUE_CTL_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _MES_QUE_H */
