#ifndef PORT_H
#define PORT_H


#include "list.h"
#include "portmacro.h"
#include "projdefs.h"
#include "task.h"
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOcStack,  
                                TaskFunction_t pxCode,
                                void *pvParameters );


BaseType_t xPortStartScheduler( void );
void prvStartFirstTask( void );
void vPortSVCHandler( void );
void vTaskSwitchContext( void );
#endif

