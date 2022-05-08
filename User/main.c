#include "FreeRTOS.h"
#include "task.h"

/* ԭ������main.c�ļ�������������Ϊ�����ֶ�ȷ�������л��Ľ׶� */
/*
*************************************************************
*������ƿ��STACK
*************************************************************
*/
TaskHandle_t Task1_Handle;
//#define TASK1_STACK_SIZE 128
StackType_t Task1Stack[TASK1_STACK_SIZE];
extern TCB_t Task1TCB;

TaskHandle_t Task2_Handle;
//#define TASK2_STACK_SIZE 128
StackType_t Task2Stack[TASK2_STACK_SIZE];
extern TCB_t Task2TCB;

/*������������ջ*/
#define configMINIMAL_STACK_SIZE (( unsigned short ) 128 )
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
/*������������������ƿ�*/
TCB_t IdleTaskTCB;
/*
*************************************************************
*ȫ�ֱ���
*************************************************************
*/
portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];
/*
*************************************************************
*��������
*************************************************************
*/
void delay (uint32_t count);
void Task1_Entry( void *p_arg );
void Task2_Entry( void *p_arg );

/*
*************************************************************
*main����
*************************************************************
*/

int main(){

	/*��ʼ����������ص��б�������б�*/
	prvInitialiseTaskLists();
	
	Task1_Handle = /* ������ */
		xTaskCreateStatic((TaskFunction_t)Task1_Entry,		/*�������*/
											(char *)"Task1",								/*�������ƣ��ַ�����ʽ*/
											(uint32_t)TASK1_STACK_SIZE ,		/*����ջ��С����λΪ��*/
											(void *)NULL,										/*�����β�*/
											(StackType_t *)Task1Stack,  		/*����ջ��ʼ��ַ*/
											(TCB_t *)&Task1TCB ); 					/*������ƿ�*/
	
	/*��������ӵ������б�*/
	vListInsertEnd( &( pxReadyTasksLists[1] ),
		&(((TCB_t *)(&Task1TCB))->xStateListItem ) );
	
	Task2_Handle = /* ������ */
		xTaskCreateStatic((TaskFunction_t)Task2_Entry,		/*�������*/
											(char *)"Task2",								/*�������ƣ��ַ�����ʽ*/
											(uint32_t)TASK2_STACK_SIZE ,		/*����ջ��С����λΪ��*/
											(void *)NULL,										/*�����β�*/
											(StackType_t *)Task2Stack,  		/*����ջ��ʼ��ַ*/
											(TCB_t *)&Task2TCB ); 					/*������ƿ�*/
	
	/*��������ӵ������б�*/
	vListInsertEnd( &( pxReadyTasksLists[2]),
		&(((TCB_t *)(&Task2TCB))->xStateListItem));
	
	/*��������������ʼ��������ȣ������ɹ��򲻷���*/
	vTaskStartScheduler();

}



/*
*************************************************************
*����ʵ��
*************************************************************
*/
void delay(uint32_t count)
{
	for(; count!=0; count--);
}

void Task1_Entry(void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		/*�����л����������ֶ��л�*/
		taskYIELD();
	}
}

void Task2_Entry(void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		taskYIELD();
	}
}


/* ����������Ĳ��� */
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize )
{

	*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
	*ppxIdleTaskStackBuffer=IdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;

}

///*����������ڵ�*/
//struct xLIST List_Test;

///*����ڵ�*/
//struct xLIST_ITEM List_Item1;
//struct xLIST_ITEM List_Item2;
//struct xLIST_ITEM List_Item3;

//int main(void){

//	/*������ڵ��ʼ��*/
//	vListInitialise( &List_Test );
//	
//	/*�ڵ�1��ʼ��*/
//	vListInitialiseItem( &List_Item1 );
//	List_Item1.xItemValue = 1;
//	
//	/*�ڵ�2��ʼ��*/
//	vListInitialiseItem( &List_Item2 );
//	List_Item2.xItemValue = 2;
//	
//	/*�ڵ�3��ʼ��*/
//	vListInitialiseItem(&List_Item3 );
//	List_Item3.xItemValue = 3;

///*���ڵ��������������������*/
//	vListInsert( &List_Test, &List_Item2 );
//	vListInsert( &List_Test, &List_Item1 );
//	vListInsert(&List_Test, &List_Item3 );	
//	
//	for(;;)
//	{
//	
//	}
//	
//}
