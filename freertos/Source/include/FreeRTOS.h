#ifndef FREERTOS_H
#define FREERTOS_H

#include "portmacro.h"
#include "list.h"
#include "FreeRTOSConfig.h"

/*��������ջ*/
#define TASK1_STACK_SIZE 128 /*128��*/
extern StackType_t Task1Stack[TASK1_STACK_SIZE];

#define TASK2_STACK_SIZE 128
extern StackType_t Task2Stack[TASK2_STACK_SIZE];

#define TASK3_STACK_SIZE 128
extern StackType_t Task3Stack[TASK3_STACK_SIZE];

/*������ƿ�����*/
typedef struct tskTaskControlBlock
{
	volatile StackType_t *pxTopOfStack;/*ջ��*/
	ListItem_t xStateListItem; /* ����ڵ� */
	StackType_t *pxStack; /* ����ջ��ʼ��ַ */

	/* �������ƣ�Ϊ�ַ�����ʽ */
	char pcTaskName[ configMAX_TASK_NAME_LEN ];
	
	TickType_t xTicksToDelay; /*����������ʱ*/
	
	UBaseType_t uxPriority;
}tskTCB;
typedef tskTCB TCB_t;

#endif
