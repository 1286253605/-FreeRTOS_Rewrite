/*
 * @Author: banned 1286253605@qq.com
 * @Date: 2023-07-23 16:44:11
 * @LastEditors: banned 1286253605@qq.com
 * @LastEditTime: 2023-10-11 01:57:00
 * @FilePath: \FreeRTOS_Rewrite\FreeRTOS\include\port.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef PORT_H
#define PORT_H


#include "list.h"
#include "portmacro.h"
#include "projdefs.h"
#include "task.h"
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOcStack,  
                                TaskFunction_t pxCode,
                                void *pvParameters );
void vPortSetupTimerInterrupt( void );


BaseType_t xPortStartScheduler( void );
void prvStartFirstTask( void );
void vPortSVCHandler( void );
void vTaskSwitchContext( void );
#endif

