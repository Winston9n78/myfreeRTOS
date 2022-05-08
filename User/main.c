#include "FreeRTOS.h"
#include "task.h"

/* 原来放在main.c文件，放在这是因为还在手动确定任务切换的阶段 */
/*
*************************************************************
*任务控制块和STACK
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

/*定义空闲任务的栈*/
#define configMINIMAL_STACK_SIZE (( unsigned short ) 128 )
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
/*定义空闲任务的任务控制块*/
TCB_t IdleTaskTCB;
/*
*************************************************************
*全局变量
*************************************************************
*/
portCHAR flag1;
portCHAR flag2;

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];
/*
*************************************************************
*函数声明
*************************************************************
*/
void delay (uint32_t count);
void Task1_Entry( void *p_arg );
void Task2_Entry( void *p_arg );

/*
*************************************************************
*main函数
*************************************************************
*/

int main(){

	/*初始化与任务相关的列表，如就绪列表*/
	prvInitialiseTaskLists();
	
	Task1_Handle = /* 任务句柄 */
		xTaskCreateStatic((TaskFunction_t)Task1_Entry,		/*任务入口*/
											(char *)"Task1",								/*任务名称，字符串形式*/
											(uint32_t)TASK1_STACK_SIZE ,		/*任务栈大小，单位为字*/
											(void *)NULL,										/*任务形参*/
											(StackType_t *)Task1Stack,  		/*任务栈起始地址*/
											(TCB_t *)&Task1TCB ); 					/*任务控制块*/
	
	/*将任务添加到就绪列表*/
	vListInsertEnd( &( pxReadyTasksLists[1] ),
		&(((TCB_t *)(&Task1TCB))->xStateListItem ) );
	
	Task2_Handle = /* 任务句柄 */
		xTaskCreateStatic((TaskFunction_t)Task2_Entry,		/*任务入口*/
											(char *)"Task2",								/*任务名称，字符串形式*/
											(uint32_t)TASK2_STACK_SIZE ,		/*任务栈大小，单位为字*/
											(void *)NULL,										/*任务形参*/
											(StackType_t *)Task2Stack,  		/*任务栈起始地址*/
											(TCB_t *)&Task2TCB ); 					/*任务控制块*/
	
	/*将任务添加到就绪列表*/
	vListInsertEnd( &( pxReadyTasksLists[2]),
		&(((TCB_t *)(&Task2TCB))->xStateListItem));
	
	/*启动调度器，开始多任务调度，启动成功则不返回*/
	vTaskStartScheduler();

}



/*
*************************************************************
*函数实现
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
		/*任务切换，这里是手动切换*/
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


/* 传空闲任务的参数 */
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize )
{

	*ppxIdleTaskTCBBuffer=&IdleTaskTCB;
	*ppxIdleTaskStackBuffer=IdleTaskStack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;

}

///*定义链表根节点*/
//struct xLIST List_Test;

///*定义节点*/
//struct xLIST_ITEM List_Item1;
//struct xLIST_ITEM List_Item2;
//struct xLIST_ITEM List_Item3;

//int main(void){

//	/*链表根节点初始化*/
//	vListInitialise( &List_Test );
//	
//	/*节点1初始化*/
//	vListInitialiseItem( &List_Item1 );
//	List_Item1.xItemValue = 1;
//	
//	/*节点2初始化*/
//	vListInitialiseItem( &List_Item2 );
//	List_Item2.xItemValue = 2;
//	
//	/*节点3初始化*/
//	vListInitialiseItem(&List_Item3 );
//	List_Item3.xItemValue = 3;

///*将节点插入链表，按照升序排列*/
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
