#ifndef MAIN_H
#define MAIN_H

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "list.h"
#include "task.h"
#include "port.h"
#include "portmacro.h"
#include "portable.h"

void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer, 
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize );

#endif