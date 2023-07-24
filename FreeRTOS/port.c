#include "port.h"

#define portINITIAL_XPSR            ( 0x01000000 )
#define portSTART_ADDRESS_MASK      ( ( StackType_t ) 0xfffffffeUL  )

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


