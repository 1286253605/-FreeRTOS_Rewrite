#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


#define configUSE_16_BIT_TICKS              0
#define configMAX_TASK_NAME_LEN             16          // 任务名字长度
#define configSUPPORT_STATIC_ALLOCATION     1
#define configMAX_PRIORITIES                5           // 任务优先级总数 默认5 也就是任务有5个优先级

#define configKERNEL_INTERRUPT_PRIORITY         255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    191     // 高四位有效 即0xb0 或11； 191 = 1011 1111


#define xPortPendSVHandler   PendSV_Handler
#define xPortSysTickHandler  SysTick_Handler
#define vPortSVCHandler      SVC_Handler

#endif

