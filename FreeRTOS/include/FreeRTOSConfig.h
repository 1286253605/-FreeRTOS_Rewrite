#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


#define configUSE_16_BIT_TICKS              0
#define configMAX_TASK_NAME_LEN             16          // 任务名字长度
#define configSUPPORT_STATIC_ALLOCATION     1
#define configMAX_PRIORITIES                5           // 任务优先级总数 默认5 也就是任务有5个优先级

#define configKERNEL_INTERRUPT_PRIORITY         255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    191     // 高四位有效 即0xb0 或11； 191 = 1011 1111

// idle 空闲任务
#define configMINIMAL_STACK_SIZE ( ( unsigned short )128 )

#define xPortPendSVHandler   PendSV_Handler
#define xPortSysTickHandler  SysTick_Handler
#define vPortSVCHandler      SVC_Handler

#define configASSERT( x ) if ((x) == 0) {for( ;; );}

// 去源码里偷的
#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             0
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1

#endif

