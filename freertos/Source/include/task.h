#ifndef TASK_H
#define TASK_H

#include "portmacro.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "list.h"

#define taskYIELD() portYIEIL()

/* 任务句柄 */
typedef void *TaskHandle_t;

TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
																const char *const pcName,
																const uint32_t ulStackDepth,
															  void * const pvParameters,
																StackType_t * const puxStackBuffer,
																TCB_t * const pxTaskBuffer );
																
void prvInitialiseTaskLists( void );
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack,
																		TaskFunction_t pxCode,
																		void *pvParameters );
void vTaskStartScheduler( void );
BaseType_t xPortStartScheduler(void);
void prvStartFirstTask (void);
																
/*===进入临界段，不带中断保护版本，不能嵌套==== */								
#define taskENTER_CRITICAL()	portENTER_CRITICAL()															
/*===进入临界段，带中断保护版本，可以嵌套==== */		
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()																
/*==退出临界段，不带中断保护版本，不能嵌套==*/	
#define taskEXIT_CRITICAL() portEXIT_CRITICAL()										
/*==退出临界段，带中断保护版本，可以嵌套==*/
#define taskEXIT_CRITICAL_FROM_ISR(x) portCLEAR_INTERRUPT_MASK_FROM_ISR(x)
																
#endif
