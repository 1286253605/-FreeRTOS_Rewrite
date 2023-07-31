/*
 * @Author: Banned 1286253605@qq.com
 * @Date: 2023-07-20 13:46:54
 * @LastEditors: Banned 1286253605@qq.com
 * @LastEditTime: 2023-07-20 14:47:08
 * @FilePath: \FreeRTOS_Rewrite\FreeRTOS\include\portmacro.h
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "stdint.h"
#include "stddef.h"

// 数据类型重定义
#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        long
#define portSHORT       short
#define portSTACK_TYPE  uint32_t
#define portBASE_TYPE   long            // 32位 long 和 int 一样是4字节


typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
    typedef uint16_t TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffff
#else
    typedef uint32_t TickType_t;
    /* 8个f 4x8=32 32个1 */
    #define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

#define portNVIC_INT_CTRL_REG           ( *( ( volatile uint32_t *)0xe000ed04 ) )   
#define portNVIC_PENDSVSET_BIT          ( 1UL << 28UL )

#define portSY_FULL_READ_WRITE          ( 15 )

// portYIELD的原理就是， 将PendSV的寄存器位置1， 等到所有中断执行完之后就调用PendSV中断执行上下文切换
#define portYIELD()                                     \
{                                                       \
    /* 触发PendSV， 产生上下文切换，写寄存器位为1 */          \
    portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;     \
    __dsb( portSY_FULL_READ_WRITE );                    \
    __isb( portSY_FULL_READ_WRITE );                    \
}


#endif /* PORTMACRO_H */

