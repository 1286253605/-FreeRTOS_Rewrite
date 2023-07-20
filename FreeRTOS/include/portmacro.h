/*
 * @Author: Banned 1286253605@qq.com
 * @Date: 2023-07-20 13:46:54
 * @LastEditors: Banned 1286253605@qq.com
 * @LastEditTime: 2023-07-20 14:02:27
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

#if( configUSE_16_BIT_TICKS == 1 )
    typedef uint16_t TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffff
#else
    typedef uint32_t TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

#endif /* PORTMACRO_H */