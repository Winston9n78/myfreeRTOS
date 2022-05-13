#ifndef TASK_H
#define TASK_H

#include "portmacro.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "projdefs.h"
#include "list.h"

#define taskYIELD() portYIEIL()

/*�����������ȼ��궨�壬��task.h�ж���*/
#define tskIDLE_PRIORITY ((UBaseType_t ) 0U )


void vTaskDelay( const TickType_t xTicksToDelay );
/* ������ */
typedef void *TaskHandle_t;

TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
																const char *const pcName,
																const uint32_t ulStackDepth,
															  void * const pvParameters,
																/*�������ȼ�����ֵԽ�����ȼ�Խ��*/
																UBaseType_t uxPriority,
																StackType_t * const puxStackBuffer,
																TCB_t * const pxTaskBuffer );
static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB );																
void prvInitialiseTaskLists( void );
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack,
																		TaskFunction_t pxCode,
																		void *pvParameters );
void vTaskStartScheduler( void );
BaseType_t xPortStartScheduler(void);
void prvStartFirstTask (void);

void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize);
void prvIdleTask(void *p_arg);																
void xTaskIncrementTick( void );

void vPortEnterCritical( void );
void vPortExitCritical( void );	
static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait );
/*===�����ٽ�Σ������жϱ����汾������Ƕ��==== */								
#define taskENTER_CRITICAL()	portENTER_CRITICAL()															
/*===�����ٽ�Σ����жϱ����汾������Ƕ��==== */		
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()																
/*==�˳��ٽ�Σ������жϱ����汾������Ƕ��==*/	
#define taskEXIT_CRITICAL() portEXIT_CRITICAL()										
/*==�˳��ٽ�Σ����жϱ����汾������Ƕ��==*/
#define taskEXIT_CRITICAL_FROM_ISR(x) portCLEAR_INTERRUPT_MASK_FROM_ISR(x)
																
#endif
