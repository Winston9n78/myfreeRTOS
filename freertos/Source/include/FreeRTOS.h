#ifndef FREERTOS_H
#define FREERTOS_H

#include "portmacro.h"
#include "list.h"
#include "FreeRTOSConfig.h"

/*定义任务栈*/
#define TASK1_STACK_SIZE 128 /*128字*/
extern StackType_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE 128
extern StackType_t Task2Stack[TASK2_STACK_SIZE];

#define TASK3_STACK_SIZE 128
extern StackType_t Task3Stack[TASK3_STACK_SIZE];

/*任务控制块声明*/
typedef struct tskTaskControlBlock
{
	volatile StackType_t *pxTopOfStack;/*栈顶*/
	ListItem_t xStateListItem; /* 任务节点 */
	StackType_t *pxStack; /* 任务栈起始地址 */

	/* 任务名称，为字符串形式 */
	char pcTaskName[ configMAX_TASK_NAME_LEN ];
	
	TickType_t xTicksToDelay; /*用于阻塞延时*/
	
	UBaseType_t uxPriority;
}tskTCB;
typedef tskTCB TCB_t;

#endif
