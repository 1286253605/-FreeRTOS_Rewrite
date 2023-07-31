#ifndef PROJDEFS_H
#define PROJDEFS_H

// 创建任务使用到的函数指针，返回值为void，参数为void指针
typedef void (*TaskFunction_t)( void* );                        

#define pdFALSE                     ( ( BaseType_t ) 0)
#define pdTRUE                      ( ( BaseType_t ) 1)

#define pdPASS                      ( pdTRUE )
#define pdFAIL                      ( pdFAIL )
#endif /* PROJDEFS_H */

