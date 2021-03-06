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
TCB_t Task1TCB;

TaskHandle_t Task2_Handle;
//#define TASK2_STACK_SIZE 128
StackType_t Task2Stack[TASK2_STACK_SIZE];
TCB_t Task2TCB;

TaskHandle_t Task3_Handle;
//#define TASK2_STACK_SIZE 128
StackType_t Task3Stack[TASK3_STACK_SIZE];
TCB_t Task3TCB;

/*定义空闲任务的栈*/
#define configMINIMAL_STACK_SIZE (( unsigned short ) 128 )
StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
/*定义空闲任务的任务控制块*/
TCB_t IdleTaskTCB;
TaskHandle_t xIdleTaskHandle;
/*
*************************************************************
*全局变量
*************************************************************
*/
portCHAR flag1;
portCHAR flag2;
portCHAR flag3;

extern List_t pxReadyTasksLists[ configMAX_PRIORITIES ];
/*
*************************************************************
*函数声明
*************************************************************
*/
void delay (uint32_t count);
void Task1_Entry( void *p_arg );
void Task2_Entry( void *p_arg );
void Task3_Entry( void *p_arg );
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
											(UBaseType_t)2,
											(StackType_t *)Task1Stack,  		/*任务栈起始地址*/
											(TCB_t *)&Task1TCB ); 					/*任务控制块*/
	
	/*将任务添加到就绪列表*/
//	vListInsertEnd( &( pxReadyTasksLists[1] ),
//		&(((TCB_t *)(&Task1TCB))->xStateListItem ) );
	
	Task2_Handle = /* 任务句柄 */
		xTaskCreateStatic((TaskFunction_t)Task2_Entry,		/*任务入口*/
											(char *)"Task2",								/*任务名称，字符串形式*/
											(uint32_t)TASK2_STACK_SIZE ,		/*任务栈大小，单位为字*/
											(void *)NULL,										/*任务形参*/
											(UBaseType_t)2,
											(StackType_t *)Task2Stack,  		/*任务栈起始地址*/
											(TCB_t *)&Task2TCB ); 					/*任务控制块*/
											
	Task3_Handle = /* 任务句柄 */
		xTaskCreateStatic((TaskFunction_t)Task3_Entry,		/*任务入口*/
											(char *)"Task3",								/*任务名称，字符串形式*/
											(uint32_t)TASK3_STACK_SIZE ,		/*任务栈大小，单位为字*/
											(void *)NULL,										/*任务形参*/
											(UBaseType_t)3,
											(StackType_t *)Task3Stack,  		/*任务栈起始地址*/
											(TCB_t *)&Task3TCB ); 					/*任务控制块*/
	
	/*将任务添加到就绪列表*/
//	vListInsertEnd( &( pxReadyTasksLists[2]),
//		&(((TCB_t *)(&Task2TCB))->xStateListItem));
											
	/* ==创建空闲任务start== */
//	TCB_t *pxIdleTaskTCBBuffer = NULL;	/*用于指向空闲任务控制块*/
//	StackType_t *pxIdleTaskStackBuffer = NULL;/*用于空闲任务栈起始地址*/
//	uint32_t ulIdleTaskStackSize;
	
//	vApplicationGetIdleTaskMemory(&pxIdleTaskTCBBuffer,
//																&pxIdleTaskStackBuffer,
//																&ulIdleTaskStackSize);
	
	xIdleTaskHandle = /* 任务句柄 */
	xTaskCreateStatic((TaskFunction_t)prvIdleTask,		/*任务入口*/
										(char *)"IDLE",								/*任务名称，字符串形式*/
										(uint32_t)configMINIMAL_STACK_SIZE,		/*任务栈大小，单位为字*/
										(void *)NULL,										/*任务形参*/
										(UBaseType_t)0,
										(StackType_t *)IdleTaskStack,  		/*任务栈起始地址*/
										(TCB_t *)&IdleTaskTCB); 					/*任务控制块*/
	/*将任务添加到就绪列表*/
//	vListInsertEnd(&( pxReadyTasksLists[0]),
//										&(((TCB_t *)(&IdleTaskTCB))->xStateListItem));
	/* ===创建空闲任务end=== */
	
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
#if 1
		flag1 = 1;
		delay(100);
		flag1 = 0;
		delay(100);
		/*任务切换，这里是手动切换*//*触发PendSv，产生上下文切换*/ 
		//taskYIELD();
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
#if 1
		flag2 = 1;
		delay(100);
		flag2 = 0;
		delay(100);
		//taskYIELD();
#else
		flag2 = 1;
		vTaskDelay(2);		
		flag2 = 0;
		vTaskDelay(2);
#endif	
	}
}

void Task3_Entry(void *p_arg)
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
		flag3 = 1;
		vTaskDelay(1);		
		flag3 = 0;
		vTaskDelay(1);
#endif	
	}
}

void prvIdleTask(void *p_arg){
	for(;;) taskYIELD();
}

/* 传空闲任务的参数 */ /* 传参之后会出现问题，函数无法调用，暂时没弄清楚原因 */
void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize)
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
