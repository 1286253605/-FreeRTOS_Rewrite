
#include "task.h"
#include "FreeRTOS.h"


// 空闲任务所需
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );
extern TCB_t IdleTaskTCB;

void prvCheckTasksWaitingTermination( void );
void prvIdleTask( void );
void xTaskIncrementTick( void );
void vTaskSwitchContext( void );
// 当前正在运行任务的任务控制块指针 TCB_t *
volatile TCB_t * pxCurrentTCB = NULL;
// 全局tick
static volatile UBaseType_t xTickCount = ( TickType_t )0U;
// 任务就绪链表
List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

// main.c中的任务控制块
extern TCB_t Task_1_TCB;
extern TCB_t Task_2_TCB;

#if( INCLUDE_vTaskDelete == 1 )
    static List_t xTaskWaitingTermination = {0};
    static volatile UBaseType_t uxDeletedTasksWaitingCleanUp = ( UBaseType_t )0U;
#endif

// 支持多优先级
#define tskIDLE_PRIORITY        ( ( UBaseType_t ) 0U )
// 用于表示创建的任务的最高优先级；默认为0
static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;
static volatile UBaseType_t uxCurrentNumberOfTasks = ( UBaseType_t ) 0U;
/*---------------------------------------------------*/
/* 查找最高优先级的就绪任务 通用方法 */
#if( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )
    #define taskRECORD_READY_PRIORITY( uxPriority )\
        {\
            if( ( uxPriority ) > uxTopReadyPriority )\
            {\
                uxTopReadyPriority = ( uxPriority );\
            }\
        }/* taskRECORD_READY_PRIORITY */
        
    #define taskSELECT_HIGHEST_PRIORITY_TASK()\
        {\
            UBaseType_t uxTopPriority = uxTopReadyPriority;\
            while( listLIST_IS_EMPTY( &pxReadyTasksLists[ uxTopPriority ] ) )\
            {\
                /* 寻找最高优先级任务 即查看列表元素是否为空 */\
                uxTopPriority--;\
            }\
            /* 获取最高优先级任务的TCB之后更新到pxCurrentTCB中 */\
            listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );\
        }
        /* 通用方法部分不需要这两个宏定义但是还是定义了 */
        #define taskRESET_READY_PRIORITY( uxPriority )
        #define portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#else
/* 基于Cortex-M3架构的 查找最高优先级任务 优化方法 基于CLZ指令 */
#define taskRECORD_READY_PRIORITY( uxPriority )\
    portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#if 0
#define taskSELECT_HIGHEST_PRIORITY_TASK()\
    {\
        UBaseType_t uxTopPriority;\
        /* 寻找最高优先级, 使用clz指令 */ \
        portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );\
        /* 找到最高优先级就绪任务TCB, 并将其更新到pxCurrentTCB中 */\
        listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );\
    }
#endif

#if 1

	#define taskSELECT_HIGHEST_PRIORITY_TASK()														    \
	{																								    \
	UBaseType_t uxTopPriority;																		    \
																									    \
		/* 寻找最高优先级 */								                            \
		portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );								    \
		/* 获取优先级最高的就绪任务的TCB，然后更新到pxCurrentTCB */                                       \
		listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );		    \
	} /* taskSELECT_HIGHEST_PRIORITY_TASK() */

    #define taskRESET_READY_PRIORITY( uxPriority )											            \
    {																							        \
            portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) );					        \
    }
#endif

#endif
#define prvAddTaskToReadyList( pxTCB )\
        taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );\
        vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xStateListItem ) );


/*---------------------------------------------------*/
#if( configSUPPORT_STATIC_ALLOCATION == 1 )

/*
pxTaskCode：指向任务入口函数的指针，也就是实现任务逻辑的函数，通常是一个无限循环，不能返回或退出。
pcName：任务的描述性名称，主要用于调试，也可以用于获取任务句柄。
ulStackDepth：指定任务堆栈的大小，单位是StackType_t类型的变量个数。
pvParameters：传递给任务入口函数的参数，可以是任意类型的指针。
uxPriority：指定任务的优先级，数值越大优先级越高。（暂无）
puxStackBuffer：指向一个StackType_t类型的数组的指针，用于提供任务堆栈所需的内存空间。
pxTaskBuffer：指向一个StaticTask_t类型的变量的指针，用于提供任务控制块（TCB）所需的内存空间。
xReturn：返回一个TaskHandle_t类型的值，表示创建成功的任务句柄，如果创建失败则返回NULL。
*/
// 创建静态任务 手动分配堆栈。首先判断TCB空间和栈空间指针是否为空，不为空则将地址传入TCB指针和任务栈指针
TaskHandle_t xTaskCreateStatic (
    TaskFunction_t pxTaskCode,
    const char * const pcName,
    const uint32_t ulStackDepth,
    void * const pvParameters,
    UBaseType_t uxPriority,
    StackType_t *const puxStackBuffer,
    TCB_t * const pxTaskBuffer
                                ) 
{
    TCB_t * pxNewTCB;
    TaskHandle_t xReturn;

    if ( ( NULL != pxTaskBuffer ) && ( NULL != puxStackBuffer ) ) 
        {
            pxNewTCB = ( TCB_t * ) pxTaskBuffer;
            pxNewTCB->pxStack = (StackType_t *) puxStackBuffer;

            // 创建新的任务
            prvInitialiseNewTask( 
                pxTaskCode,             // 任务的入口地址
                pcName,                 // 任务名
                ulStackDepth,           // 任务栈大小 栈从高到低分配
                pvParameters,           // 任务参数
                uxPriority,             // 10.支持多优先级 任务优先级
                &xReturn,               // 任务句柄
                pxNewTCB                // 任务控制块
             );
             /* 添加任务到就绪列表 */
             /* TODO prvAddNewTaskToReadyList */
             
        }
    else 
        {
            xReturn = NULL;
        }
    return xReturn;
}

#endif /* configSUPPORT_STATIC_ALLOCATION */

// 任务初始化
static void prvInitialiseNewTask ( 
            TaskFunction_t pxTaskCode,
            const char * const pcName,
            const uint32_t ulStackDepth,
            void * const pvParameters,
            UBaseType_t uxPriority,
            TaskHandle_t * const pxCreatedTask,
            TCB_t * pxNewTCB)
{
    StackType_t * pxTopOfStack;
    UBaseType_t x;

    // 获取栈顶地址
    pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t )1 );
    // 地址值向下8字节对齐，将地址值与7的取反相与，也就是清除二进制位的低3位，这样地址将会从8的倍数开始编排
    // 按照一定规则排列，提高数据访问效率和准确性
    pxTopOfStack = ( StackType_t * ) ( ( ( uint32_t ) pxTopOfStack ) & ( ~( ( uint32_t )0x0007 ) ) );

    // 将任务名字添加到TCB中
    for ( x = ( BaseType_t )0; x < ( BaseType_t ) configMAX_TASK_NAME_LEN; x++ )
    {
        pxNewTCB->pcTaskName[ x ] = pcName[ x ];
        if ( pcName[ x ] == 0x00 ) break;
    }
    // 任务名字不能超过最大值
    pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN - 1 ] = '\0';

    // 初始化TCB中的xStateListItem,也就是衣架钩子
    vListInitialiseItem( &( pxNewTCB->xStateListItem ) );

    // 初始化xStateListItem的拥有者
    listSET_LIST_ITEM_OWNER( &( pxNewTCB->xStateListItem ), pxNewTCB );
    /* 10.初始化优先级 */
    if( uxPriority >= ( BaseType_t ) configMAX_PRIORITIES )
    {
        uxPriority = ( BaseType_t ) configMAX_PRIORITIES - ( BaseType_t ) 1U;
    }
    pxNewTCB->uxPriority = uxPriority;

    // 初始化任务栈
    pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );

    // 让任务句柄指向任务块, 如果传入了任务句柄，就是那个指针 就赋值给它
    if ( ( void * ) pxCreatedTask != NULL )
        {
            *pxCreatedTask = ( TaskHandle_t ) pxNewTCB;
        }


}


// 就绪列表初始化， 有多少优先级就有多少条优先级链表， 比如此处MAX是5
void prvInitialiseTaskLists( void )
{
    UBaseType_t uxPriority;

    for ( ( UBaseType_t )0U; 
            uxPriority < ( UBaseType_t )configMAX_PRIORITIES;
            uxPriority++
        )
     {
        vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
     }
}




void vTaskStartScheduler( void ) 
{

    // ————————————————————————创建空闲任务————————————————————————
    TCB_t *pxIdleTaskTCBBuffer = NULL;
    StackType_t *pxIdleTaskStackBuffer = NULL;
    uint32_t ulIdleTaskStackSize;

    vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, 
                                   &pxIdleTaskStackBuffer, 
                                   &ulIdleTaskStackSize );

    xTaskCreateStatic( (TaskFunction_t)prvIdleTask, 
                        ( char* )"IDLE",
                        ( uint32_t)ulIdleTaskStackSize,
                        ( void* )NULL,
                        ( UBaseType_t )tskIDLE_PRIORITY,
                        ( StackType_t * )pxIdleTaskStackBuffer,
                        ( TCB_t* )pxIdleTaskTCBBuffer );

    // 将空闲任务插入 就绪列表中的第一条
    vListInsertEnd( &( pxReadyTasksLists[0] ),
                    &( ( ( TCB_t* )pxIdleTaskTCBBuffer )->xStateListItem ) );

    // ————————————————————————创建空闲任务————————————————————————
    // 手动指定启动调度器后运行的第一个任务
    pxCurrentTCB = &Task_1_TCB;

    // 启动调度器
    if ( xPortStartScheduler() != pdFALSE ){
        // 不会执行到这里
    }
}
/*  ARM Compiler 6 版本嵌入ARMASM的方法
void prvStartFirstTask( void ) 
{
    __asm(".eabi_attribute Tag_ABI_align_preserved, 1");                // 等价于 Compiler5 PRESERVE8
    __asm("ldr r0, [r0]");

}
*/


void prvIdleTask( void )
{
    for(;;)
    {
        prvCheckTasksWaitingTermination();

    }

}


void prvCheckTasksWaitingTermination( void )
{
    #if( INCLUDE_vTaskDelete == 1 )

        TCB_t *pxTCB;
        // 检查是否有删除了的任务
        while( uxDeletedTasksWaitingCleanUp > ( UBaseType_t )0U )
        {
            taskENTER_CRITICAL();
            {
                pxTCB = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( ( &xTaskWaitingTermination ) );
                uxListRemove( &( pxTCB->xStateListItem ) );
                --uxDeletedTasksWaitingCleanUp;
            }
            taskEXIT_CRITICAL();
            // prvDeleteTCB( pxTCB );
        }
    
    #endif

}


void vTaskDelay( const TickType_t xTicksToDelay )
{
    TCB_t *pxTCB = NULL;

    // 获取当前任务TCB 因为只有当前任务才能运行 并 调用这个函数- -||
    pxTCB = pxCurrentTCB;

    pxTCB->xTicksToDelay = xTicksToDelay;

    /* 将任务从就绪列表中删除 */
    taskRESET_READY_PRIORITY( pxTCB->uxPriority );

    taskYIELD();

}

/* +1s */
void xTaskIncrementTick( void )
{
    TCB_t *pxTCB = NULL;
    BaseType_t i = 0;

    const TickType_t xConstTickCount = xTickCount + 1;
    xTickCount = xConstTickCount;

    /* 扫描 就绪链表中所有任务的 xTicksToDelay 如果不是0 则-1 */
    for( i = 0; i < configMAX_PRIORITIES; i++ )
    {
        pxTCB = ( TCB_t* ) listGET_OWNER_OF_HEAD_ENTRY( ( &pxReadyTasksLists[ i ] ) );
        if( pxTCB->xTicksToDelay > 0 )
        {
            pxTCB->xTicksToDelay--;
            /* 任务延时时间结束 将任务添加到就绪列表中 */
            if( pxTCB->xTicksToDelay == 0 )
            {
                taskRECORD_READY_PRIORITY( pxTCB->uxPriority );
            }
        }
    }

    /* 任务切换 */
    portYIELD();
}

static void prvAddNewTaskToReadyList( TCB_t* pxNewTCB )
{
    /* 进入临界段 */
    taskENTER_CRITICAL();
    {
        uxCurrentNumberOfTasks++;
        if( pxCurrentTCB == NULL )
        {
            /* 如果pxCurrentTCB为空  则将pxCurrentTCB指向新创建的任务 */
            pxCurrentTCB = pxNewTCB;
        }
        /* 如果是第一次创建任务则需要初始化列表 */
        if( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
        {
            prvInitialiseTaskLists();
        }
        /* 如果不为空 */
        else
        {
            if( pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority )
            {
                /* 如果新创建的任务优先级高于当前任务 则切换pxCurrentTCB */
                pxCurrentTCB = pxNewTCB;
            }
        }
        prvAddTaskToReadyList( pxNewTCB );
    }
    taskEXIT_CRITICAL();
}


void vTaskSwitchContext( void ) 
{
    taskSELECT_HIGHEST_PRIORITY_TASK();
}