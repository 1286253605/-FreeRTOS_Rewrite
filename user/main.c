#include "main.h"

// 全局变量
portCHAR Flag_1;
portCHAR Flag_2;

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];


// 任务控制块 & STACK
#define TASK_1_STACK_SIZE 20
#define TASK_2_STACK_SIZE 20
TCB_t Task_1_TCB;
TCB_t Task_2_TCB;
TaskHandle_t Task_1_Handle;
TaskHandle_t Task_2_Handle;
//* 任务栈实际上就是一个全局数组
StackType_t Task_1_Stack[ TASK_1_STACK_SIZE ];
StackType_t Task_2_Stack[ TASK_2_STACK_SIZE ];

// 函数声明
void Task1_Entry( void *p_arg ) ;
void Task2_Entry( void *p_arg ) ;
int main(void) {

    // 硬件初始化

    // 初始化与任务相关的链表
    prvInitialiseTaskLists();

    // 创建任务
    Task_1_Handle = xTaskCreateStatic(  ( TaskFunction_t )Task1_Entry,
                                        ( char * )"Task1",
                                        ( uint32_t )TASK_1_STACK_SIZE,
                                        ( void * ) NULL,
                                        ( StackType_t * )Task_1_Stack,
                                        ( TCB_t * )&Task_1_TCB );
    // 任务添加到就绪列表
    vListInsertEnd( &( pxReadyTasksLists[ 1 ] ), 
                    &( ( ( TCB_t *)( &Task_1_TCB ) )->xStateListItem ) );

    Task_2_Handle = xTaskCreateStatic(  ( TaskFunction_t )Task2_Entry,
                                        ( char * )"Task2",
                                        ( uint32_t )TASK_2_STACK_SIZE,
                                        ( void * ) NULL,
                                        ( StackType_t * )Task_2_Stack,
                                        ( TCB_t * )&Task_2_TCB );
    vListInsertEnd( &( pxReadyTasksLists[ 2 ] ), 
                    &( ( ( TCB_t *)( &Task_2_TCB ) )->xStateListItem ) );

    

    for (;;) {
        /* 6 */
    }

}


// 软件延时
void delay ( uint32_t count ) 
{
    for (; count != 0; count--);
}

// 任务1
void Task1_Entry( void *p_arg ) 
{
    for ( ;; ) 
    {
        Flag_1 = 1;
        delay( 100 );
        Flag_1 = 0;

        // 触发任务切换，手动任务切换
        // taskYIELD()
    }
}

// 任务2
void Task2_Entry( void *p_arg ) 
{
    for ( ;; ) 
    {
        Flag_2 = 1;
        delay( 100 );
        Flag_2 = 0;

        // 触发任务切换，手动任务切换
        // taskYIELD()
    }
}