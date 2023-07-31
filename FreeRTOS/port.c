#include "port.h"

#define portINITIAL_XPSR            ( 0x01000000 )
#define portSTART_ADDRESS_MASK      ( ( StackType_t ) 0xfffffffeUL  )

//  在Cortex-M中，内核外设SCB中SHPR3寄存器用于设置SysTick和PendSV的异常优先级
// STM32内核手册 PM0056 Page140 SCB_SHPR3 寄存器
#define portNVIC_SYSPRI2_REG                        ( * ( ( volatile uint32_t *)0xe000ed20 ) )
#define portNVIC_PENDSV_PRI                         ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 16UL )
#define portNVIC_SYSTICK_PRI                        ( ( ( uint32_t ) configKERNEL_INTERRUPT_PRIORITY ) << 24UL )

// 当前运行的任务块指针
extern volatile TCB_t * pxCurrentTCB;
extern TCB_t Task_1_TCB;
extern TCB_t Task_2_TCB;


static void prvTaskExitError( void ) 
{
    // 函数停止于此
    for(;;);
}

// 这个函数的作用 每个任务都被分配一段栈空间，传入的参数pxTopOcStack就是栈顶地址
// 初始化任务时调用这个函数分配栈空间，(看图)首先把一些寄存器的值放入栈中，然后返回新的栈顶地址给任务保存局部变量
// 这一些寄存器又分为两部分，其中R11~R4为一部分，暂且称为第二部分，第二部分8个字节，这个函数执行完之后返回R4的地址，也就是空闲栈的起始地址

// R15(也就是程序计数器PC) R14(也就是链接寄存器, 用于保存调用函数要退出时的返回地址, 进入函数->执行->退出函数) 
// 再加上R12 3 2 1 0 寄存器，构成了第一部分。第一部分是发生异常(比如中断)时，CPU自动压入栈中的寄存器，第二部分寄存器需要手动压入

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOcStack,  
                                TaskFunction_t pxCode,
                                void *pvParameters )
{
    pxTopOcStack--;
    // 从0开始，XPSR寄存器的值初始化为0x100 0000 也就是地24位为1，表示使用Thumb指令集，提高代码密度，节省空间
    *pxTopOcStack = portINITIAL_XPSR;

    pxTopOcStack--;
    // 任务的入口地址，也就是将任务入口地址写入PC寄存器
    // 使用portSTART_ADDRESS_MASK屏蔽掉地址最后1位，因为Thumb指令要求PC寄存器必须是偶数，Thumb或Thumb-2才能正确识别指令
    // 任务切换后，PC寄存器会从堆栈中弹出，并开始执行pxCode指向的任务函数
    *pxTopOcStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;

    // FreeRTOS任务都是死循环，不会返回， 如果有返回，地址则为prvTaskExitError函数的地址
    pxTopOcStack--;
    *pxTopOcStack = ( StackType_t ) prvTaskExitError;

    // R0寄存器被初始化为 pvParameters 任务参数的内存地址
    pxTopOcStack -= 5;
    *pxTopOcStack = ( StackType_t ) pvParameters;

    // 异常发生时，手动加载到CPU寄存器的内容？
    pxTopOcStack -= 8;
    return pxTopOcStack;

}

BaseType_t xPortStartScheduler( void ) 
{
    // 配置 PendSV 和 Systick 为优先级最低
    portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
    portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;

    // 启动第一个任务，不再返回
    prvStartFirstTask();

    // 不应该运行到这里
    return 0;
}

__asm void prvStartFirstTask( void )
{
	PRESERVE8

	/* 在Cortex-M中，0xE000ED08是SCB_VTOR这个寄存器的地址，
       里面存放的是向量表的起始地址，即MSP的地址 */
	ldr r0, =0xE000ED08
	ldr r0, [r0]
	ldr r0, [r0] 

	/* 设置主堆栈指针msp的值 这个值是向量表的起始地址*/
	msr msp, r0
    
	/* 使能全局中断 */
	cpsie i
	cpsie f
    // 清流水线
	dsb
	isb
	
    /* 调用SVC去启动第一个任务 韦东山文档里有写，服务号0表示SVC中断接下来会执行SVC中断服务函数*/
	svc 0  
	nop
	nop
}

// SVC中断服务函数， 也就是SVC 0 指令中 0 指向的函数
__asm void vPortSVCHandler ( void ) 
{
    extern pxCurrentTCB;

    PRESERVE8

    // 将指针地址赋值给R3, [R3]得到地址指向的内存区域中保存的内容, 也是一个地址, 也就是r1中也是地址
    // [r1]得到r1寄存器中保存的地址指向的内存区域保存的内容, 这次得到的就是任务的栈顶地址（【结合韦东山的那张任务栈图看看】
    // TCB控制块结构体的第一个成员就是栈顶地址
    ldr r3,=pxCurrentTCB           // 加载pxCurrentTCB,也就是指针地址到r3中
    ldr r1, [r3]                    // 从指针地址取出pxCurrentTCB的地址
    ldr r0, [r1]
    ldmia r0!, {r4-r11}             // <1>此处将r0保存的内存地址即任务栈向上增长的8个字(注意是字不是字节) 加载到CPU的寄存器中，要运行任务 把数据加载到寄存器中
    msr psp, r0                     // <2>r0已经自增到新的位置， 也就是栈顶的位置已经改变, 同时也要修改系统的堆栈指针psp
    isb                             // 刷新流水线，防止指令读取出错
    mov r0, #0
    msr basepri, r0                 // basepri寄存器配置为0 说明不屏蔽任何中断
    orr r14, #0xd                   // 看笔记
    bx r14

}


__asm void xPortPendSVHandler( void )
{
    extern pxCurrentTCB;
    extern vTaskSwitchContext;

    PRESERVE8

    mrs r0,psp
    isb

    ldr r3,=pxCurrentTCB
    ldr r2,[r3]

    stmdb r0!,{r4-r11}
    str r0,[r2]

    stmdb sp!,{r3,r4}

    mov r0,#configMAX_SYSCALL_INTERRUPT_PRIORITY
    msr basepri,r0
    dsb
    isb
    bl vTaskSwitchContext
    mov r0,#0
    msr basepri,r0
    ldmia sp!,{r3,r4}

    ldr r1,[r3]
    ldr r0,[r1]
    ldmia r0!,{r4-r11}
    msr psp,r0
    isb
    bx r14

    nop
    
}


void vTaskSwitchContext( void ) 
{
    if ( pxCurrentTCB == &Task_1_TCB )
    {
        pxCurrentTCB = &Task_2_TCB;
    }
    else
    {
        pxCurrentTCB = &Task_1_TCB;
    }
}


