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
TCB_t Task1TCB;

TaskHandle_t Task2_Handle;
//#define TASK2_STACK_SIZE 128
StackType_t Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;

/*������������ջ*/
#define configMINIMAL_STACK_SIZE (( unsigned short ) 128 )
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
/*������������������ƿ�*/
TCB_t IdleTaskTCB;
TaskHandle_t xIdleTaskHandle;
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
											
	/* ==������������start== */
//	TCB_t *pxIdleTaskTCBBuffer = NULL;	/*����ָ�����������ƿ�*/
//	StackType_t *pxIdleTaskStackBuffer = NULL;/*���ڿ�������ջ��ʼ��ַ*/
//	uint32_t ulIdleTaskStackSize;
	
//	vApplicationGetIdleTaskMemory(&pxIdleTaskTCBBuffer,
//																&pxIdleTaskStackBuffer,
//																&ulIdleTaskStackSize);
	
	xIdleTaskHandle = /* ������ */
	xTaskCreateStatic((TaskFunction_t)prvIdleTask,		/*�������*/
										(char *)"IDLE",								/*�������ƣ��ַ�����ʽ*/
										(uint32_t)configMINIMAL_STACK_SIZE,		/*����ջ��С����λΪ��*/
										(void *)NULL,										/*�����β�*/
										(StackType_t *)IdleTaskStack,  		/*����ջ��ʼ��ַ*/
										(TCB_t *)&IdleTaskTCB); 					/*������ƿ�*/
	/*��������ӵ������б�*/
	vListInsertEnd(&( pxReadyTasksLists[0]),
										&(((TCB_t *)(&IdleTaskTCB))->xStateListItem));
	/* ===������������end=== */
	
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
#if 0
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		/*�����л����������ֶ��л�*//*����PendSv�������������л�*/ 
		taskYIELD();
#else
		flag1 = 1;
		vTaskDelay(2);		
		flag1 = 0;
		vTaskDelay(2);
#endif
	}
}

void Task2_Entry(void *p_arg)
{
	for(;;)
	{
#if 0
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		taskYIELD();
#else
		flag2 = 1;
		vTaskDelay(2);		
		flag2 = 0;
		vTaskDelay(2);
#endif	
	}
}

void prvIdleTask(void *p_arg){
	for(;;) taskYIELD();
}

/* ����������Ĳ��� */ /* ����֮���������⣬�����޷����ã���ʱûŪ���ԭ�� */
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize)
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
