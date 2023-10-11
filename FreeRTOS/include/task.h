#ifndef TASK_H
#define TASK_H

#include "list.h"
#include "port.h"

typedef void * TaskHandle_t;

#define taskYIELD()             portYIELD()

/* 不带保护的保本 不能嵌套 */
#define taskENTER_CRITICAL()    portENTER_CRITICAL()
#define taskEXIT_CRITICAL()     protEXIT_CRITICAL()
/* 带中断保护的版本 */
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()
#define taskEXIT_CRITICAL_FROM_ISR( x )     portCLEAR_INTERRUPT_MASK_FROM_ISR( x )

// 任务控制块 TCB //为什么韦东山的代码把这个写到FreeRTOS.h中？
// 包含了任务的堆栈控制，就绪链表节点，任务名字符串
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
    // 延时时间
    TickType_t xTicksToDelay;
    // 任务优先级
    UBaseType_t uxPriority;

} tskTCB;
typedef tskTCB TCB_t;


TaskHandle_t xTaskCreateStatic (
    TaskFunction_t pxTaskCode,
    const char * const pcName,
    const uint32_t ulStackDepth,
    void * const pvParameters,
    UBaseType_t uxPriority,
    StackType_t *const puxStackBuffer,
    TCB_t * const pxTaskBuffer
                                );

static void prvInitialiseNewTask ( 
            TaskFunction_t pxTaskCode,
            const char * const pcName,
            const uint32_t ulStackDepth,
            void * const pvParameters,
            UBaseType_t uxPriority,
            TaskHandle_t * const pxCreatedTask,
            TCB_t * pxNewTCB
                                );


void prvInitialiseTaskLists( void );
void vTaskStartScheduler( void ) ;
void vTaskDelay( const TickType_t xTicksToDelay );
#endif
						
						