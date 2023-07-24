#ifndef TASK_H
#define TASK_H

#include "FreeRTOS.h"
#include "projdefs.h"
#include "list.h"
#include "port.h"

typedef void * TaskHandle_t;

// 任务控制块 TCB //为什么韦东山的代码把这个写到FreeRTOS.h中？
typedef struct tskTaskControlBlock
{
    // 栈顶
    volatile StackType_t *pxTopOfStack;
    // 任务节点，就是TCB中的一个链表节点，能将任务挂到任务列表上，这个任务节点相当于衣架钩子，TCB就是衣服
    ListItem_t xStateListItem;
    // 任务栈的起始地址
    StackType_t *pxStack;
    // 任务名字，字符串
    char pcTaskName[ configMAX_TASK_NAME_LEN  ];
} tskTCB;
typedef tskTCB TCB_t;


TaskHandle_t xTaskCreateStatic (
    TaskFunction_t pxTaskCode,
    const char * const pcName,
    const uint32_t ulStackDepth,
    void * const pvParameters,
    StackType_t *const puxStackBuffer,
    TCB_t * const pxTaskBuffer
                                );

static void prvInitialiseNewTask ( 
            TaskFunction_t pxTaskCode,
            const char * const pcName,
            const uint32_t ulStackDepth,
            void * const pvParameters,
            TaskHandle_t * const pxCreatedTask,
            TCB_t * pxNewTCB
                                );


#endif