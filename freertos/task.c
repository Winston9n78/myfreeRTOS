#include "task.h"

#if(configSUPPORT_STATIC_ALLOCATION == 1)

TCB_t * pxCurrentTCB;

List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
extern TCB_t Task3TCB;
extern TCB_t IdleTaskTCB;

extern unsigned int xTickCount;
/*定义uxTopReadyPriority，在task.c中定义*/
static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;

static List_t xDelayedTaskList1; //任务延时列表1，非溢出
static List_t xDelayedTaskList2; //任务延时列表2，溢出
static List_t *volatile pxDelayedTaskList; //非溢出的延时列表的指针
static List_t *volatile pxOverflowDelayedTaskList; //溢出的延时列表的指针

static unsigned int xNextTaskUnblockTime;

static unsigned int xNumOfOverflows;

#define taskSWITCH_DELAYED_LISTS() \
{ \
	List_t *pxTemp; \
	pxTemp = pxDelayedTaskList ; \
	pxDelayedTaskList = pxOverflowDelayedTaskList; \
	pxOverflowDelayedTaskList = pxTemp ; \
	xNumOfOverflows++; \
	prvResetNextTaskUnblockTime(); \
}

static void prvResetNextTaskUnblockTime( void )
{
	TCB_t *pxTCB;
	
	if( listLIST_IS_EMPTY ( pxDelayedTaskList ) != pdFALSE )
	{
		/*当前延时列表为空，则设置xNextTaskUnblockTime等于最大值*/
		xNextTaskUnblockTime = portMAX_DELAY;
	}
	else
	{
		/*当前列表不为空，表示有任务在延时，则获取当前列表下第一个节点的排序值然后将该节点的排序值更新到xNextTaskUnblockTime */
		( pxTCB ) = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE(&( ( pxTCB )->xStateListItem ) );
	}
	
}
/*查找最高优先级的就绪任务（通用方法）*/
#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )

	/*uxTopReadyPriority存储的是就绪任务的最高优先级*/
	#define taskRECORD_READY_PRIORITY ( uxPriority ) \
	{ \
		if( ( uxPriority ) >uxTopReadyPriority )\
			{ \
				uxTopReadyPriority = ( uxPriority ); \
			} \
	} /*taskRECORD_READY_PRIORITY */

	/*---------------------------------------------------------------*/
	#define taskSELECT_HIGHEST_PRIORITY_TASK() \
	{ \
		UBaseType_t uxTopPriority = uxTopReadyPriority; \
		/*寻找包含就绪任务的最高优先级的队列*/ \
		while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopPriority ] ) )) \
		{ \
			--uxTopPriority; \
		} \
		/*获取优先级最高的就绪任务的TCB，然后更新到pxCurrentTCB*/ \
		listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, &(pxReadyTasksLists[ uxTopPriority ])); \
		/*更新uxTopReadyPriority */ \
		uxTopReadyPriority = uxTopPriority; \
	}/* taskSELECT_HIGHEST_PRIORITY_TASK */

	/*---------------------------------------------------------------*/
/*这两个宏定义只有在选择优化方法时才用，这里定义为空*/
#define taskRESET_READY_PRIORITY( uxPriority )
#define portRESET_READY_PRIORITY( uxPriority,uxTopReadyPriority )	
	
/*查找最高优先级的就绪任务（根据处理器架构优化后的方法）*/
#else /* configUSE_PORT_OPTIMISED_TASK_SELECTION */
	#define taskRECORD_READY_PRIORITY(uxPriority ) \
						portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )
/*---------------------------------------------------------------*/
#define taskSELECT_HIGHEST_PRIORITY_TASK() { UBaseType_t uxTopPriority; /*寻找最高优先级*/portGET_HIGHEST_PRIORITY ( uxTopPriority,uxTopReadyPriority ); /*获取优先级最高的就绪任务的TCB，然后更新到pxCurrentTCB */ listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB,&( pxReadyTasksLists[ uxTopPriority ] ));}	/* taskSELECT_HIGHEST_PRIORITY_TASK()*/
	
	/*---------------------------------------------------------------*/
//#if 0
//	#define taskRESET_READY_PRIORITY( uxPriority ) \
//	{	\
//		if(listCURRENT_LIST_LENGTH(&(pxReadyTasksLists[( uxPriority)]))==(UBaseType_t)) \
//		{ \
//			portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) ); \
//		} \
//	}

//#else

//	#define taskRESET_READY_PRIORITY(uxPriority) { portRESET_READY_PRIORITY((uxPriority ), (uxTopReadyPriority));}

//#endif
	
#if 1 /* 加入延时列表实现方法 */
#define taskRESET_READY_PRIORITY ( uxPriority ) \
{ \
	if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ])) == ( UBaseType_t ) 0 ) \
	{ \
		portRESET_READY_PRIORITY( ( uxPriority )，( uxTopReadyPriority ) ); \
	} \
}

#else /* 之前的实现方法 */
#define taskRESET_READY_PRIORITY ( uxPriority ) \
{ \
	portRESET_READY_PRIORITY( ( uxPriority ),( uxTopReadyPriority ) );\
}
#endif


/*将任务添加到就绪列表*/
#define prvAddTaskToReadyList( pxTCB ) \
	taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority ); \
	vListInsertEnd( &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), \
	&( ( pxTCB )->xStateListItem ) );

static unsigned int uxCurrentNumberOfTasks;
//void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
//																		StackType_t **ppxIdleTaskStackBuffer,
//																		uint32_t *pulIdleTaskStackSize );

static void prvInitialiseNewTask(	TaskFunction_t pxTaskCode,
																	const char *const pcName,
																	const uint32_t ulStackDepth,
																	void * const pvParameters,
																	/*任务优先级，数值越大，优先级越高*/
																	UBaseType_t uxPriority,
																	TaskHandle_t *const pxCreatedTask,
																	TCB_t *pxNewTCB )
{
	StackType_t *pxTopOfStack;
	UBaseType_t x;
	/* 获取栈顶地址 */
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1);
	/* 向下做8字节对齐 */
	pxTopOfStack = ( StackType_t * ) \
									(( ( uint32_t ) pxTopOfStack )&(~( ( uint32_t ) 0x0007 ) ));
	
	/*将任务名存储在TCB中 */
	for ( x = ( UBaseType_t )0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];
		
		if ( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	
	/*任务名的长度不能超过configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN -1 ]= '\0';
	
	/*初始化TCB中的xStateListItem节点*/
	vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
	
	/*设置xStateListItem节点的拥有者*/
	listSET_LIST_ITEM_OWNER(&( pxNewTCB->xStateListItem ), pxNewTCB );
	
	/*初始化任务栈*/
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack,
																									pxTaskCode,
																									pvParameters);
	
	/*初始化优先级*/
	if ( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
	{
		uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
	}
	pxNewTCB->uxPriority = uxPriority;
	/*初始化任务栈*/
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack,pxTaskCode,pvParameters );
	
	if( ( void * ) pxCreatedTask != NULL)
	{
		*pxCreatedTask = (TaskHandle_t ) pxNewTCB;
	}
}

	


TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
																const char *const pcName,
																const uint32_t ulStackDepth,
															  void * const pvParameters,
																/*任务优先级，数值越大，优先级越高*/
																UBaseType_t uxPriority,
																StackType_t * const puxStackBuffer,
																TCB_t * const pxTaskBuffer )
{
	TCB_t *pxNewTCB;
	TaskHandle_t xReturn;
	
	if((pxTaskBuffer != NULL) && (puxStackBuffer !=NULL))
	{
		pxNewTCB = (TCB_t * ) pxTaskBuffer;
		pxNewTCB->pxStack = ( StackType_t * ) puxStackBuffer;
		
		/* 创建新的任务 */
		prvInitialiseNewTask( pxTaskCode,  		/*任务入口*/
													pcName,			 		/*任务名称，字符串形式*/
													ulStackDepth,		/*任务栈大小，单位为字*/
													pvParameters,		/*任务形参*/
													uxPriority,
													&xReturn,		 		/*任务句柄*/
													pxNewTCB);	 		/*任务栈起始地址*/
		
		prvAddNewTaskToReadyList(pxNewTCB);
	}
	else
	{
		xReturn = NULL;
	}

	/*返回任务句柄，如果任务创建成功，此时xReturn应该指向任务控制块*/
	return xReturn;
}


/*任务就绪列表初始化*/
void prvInitialiseTaskLists( void )
{
	UBaseType_t uxPriority;
	for ( uxPriority = (UBaseType_t ) 0U;
				uxPriority <(UBaseType_t ) configMAX_PRIORITIES;uxPriority++ )
	{
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}
	
	//初始化延时列表
	vListInitialise(&xDelayedTaskList1 );
	vListInitialise( &xDelayedTaskList2 );
	pxDelayedTaskList = &xDelayedTaskList1;
	pxOverflowDelayedTaskList = &xDelayedTaskList2;
}

/* 启动调度器 */
void vTaskStartScheduler( void )
{
	
	/* 手动指定第一个运行的任务 */
	//pxCurrentTCB = &Task1TCB;
	
	/* 初始化xNextTaskUnblockTime */
	xNextTaskUnblockTime = portMAX_DELAY;
	xTickCount = ( TickType_t ) 0U;
	/* 启动调度器 */
	if ( xPortStartScheduler() != pdFALSE )
	{
		/* 调度器启动成功，则不会返回，即不会来到这里 */
	}
}

/* 阻塞延时函数 */
void vTaskDelay(const TickType_t xTicksToDelay)
{
	//TCB_t *pxTCB = NULL;
	/*获取当前任务的TCB*/
	//pxTCB = pxCurrentTCB;
	
	/*设置延时时间*/
	//pxTCB->xTicksToDelay = xTicksToDelay;
	
	/*将任务插入延时列表*/
	prvAddCurrentTaskToDelayedList( xTicksToDelay );
	
	/*将任务从就绪列表中移除*/
	// uxListRemove( &( pxTCB->xStateListItem ));
	// taskRESET_READY_PRIORITY( pxTCB->uxPriority );
	
	/*任务切换*/
	taskYIELD(); /*触发PendSv，产生上下文切换*/ 
}

/* 任务切换 */
#if 1
void vTaskSwitchContext ( void )
{
	/*两个任务轮流切换*/
//	if ( pxCurrentTCB == &Task1TCB )
//	{
//		pxCurrentTCB = &Task2TCB;
//	}
//	
//	else
//	{
//		pxCurrentTCB = &Task1TCB;
//	}
	/*获取优先级最高的就绪任务的TCB，然后更新到pxCurrentTCB */
	taskSELECT_HIGHEST_PRIORITY_TASK();
}

#else
void vTaskSwitchContext ( void )
{
	/*如果当前任务是空闲任务，那么就去尝试执行任务1或者任务2，
	看看它们的延时是否结束，如果任务的延时均没有到期，则返回，继续执行空闲任务*/
	if ( pxCurrentTCB == &IdleTaskTCB )
	{
		if ( Task1TCB.xTicksToDelay == 0)
		{
			pxCurrentTCB = &Task1TCB;
		}
		
		else if( Task2TCB.xTicksToDelay  == 0)
		{
			pxCurrentTCB = &Task2TCB;
		}		
		else
		{
			return; /*任务延时均没有到期则返回，继续执行空闲任务*/
		}
	}
	
	else /*当前任务不是空闲任务则会执行到这里*/
	{
		/*如果当前任务是任务1或者任务2，检查另外一个任务，
		如果另外的任务不在延时中，就切换到该任务，
		否则，判断当前任务是否应该进入延时状态，
		如果是，就切换到空闲任务，否则不进行任何切换*/
		if (pxCurrentTCB ==&Task1TCB)
		{
			if (Task2TCB.xTicksToDelay ==0)
			{
				pxCurrentTCB =&Task2TCB;
			}
			else if (pxCurrentTCB->xTicksToDelay != 0)
			{
				pxCurrentTCB = &IdleTaskTCB;
			}
			else
			{
				return; /*返回，不进行切换,因为两个任务都处于延时中*/
			}		
		}
		
		else if (pxCurrentTCB ==&Task2TCB)
		{
			if (Task1TCB.xTicksToDelay ==0)
			{
				pxCurrentTCB =&Task1TCB;
			}
			else if (pxCurrentTCB->xTicksToDelay != 0)
			{
				pxCurrentTCB = &IdleTaskTCB;
			}
			else
			{
				return; /*返回，不进行切换,因为两个任务都处于延时中*/
			}
		
		}
		
	}

}

#endif

void xTaskIncrementTick( void )
{
	TCB_t *pxTCB;
	TickType_t xItemValue;
	// BaseType_t i = 0;
	/*更新系统时基计数器xTickCount，xTickCount是一个在port.c中定义的全局变量*/
	const TickType_t xConstTickCount = xTickCount + 1;
	xTickCount = xConstTickCount;
	
	/*如果xConstTickCount溢出，则切换延时列表*/
	if ( xConstTickCount == ( TickType_t ) 0U )
	{
		taskSWITCH_DELAYED_LISTS();
	}
	
	/*最近的延时任务延时到期*/
	if ( xConstTickCount >= xNextTaskUnblockTime )
	{
		for(;;)
		{
			if ( listLIST_IS_EMPTY ( pxDelayedTaskList ) != pdFALSE )
			{
				/*延时列表为空，设置xNextTaskUnblockTime为可能的最大值*/
				xNextTaskUnblockTime = portMAX_DELAY;
				break;
			}
			else
			{
				pxTCB = (TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList);
				xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );
				
				/*直到将延时列表中所有延时到期的任务移除才跳出for循环*/
				if ( xConstTickCount< xItemValue )
				{
					xNextTaskUnblockTime = xItemValue;
					break ;
				}
				
				/*将任务从延时列表中移除，解除等待状态*/
				( void ) uxListRemove(&( pxTCB->xStateListItem ) );
				/*将解除等待的任务添加到就绪列表*/
				prvAddTaskToReadyList( pxTCB );
			}
		}
	}/*xConstTickCount >=xNextTaskUnblockTime */
	
	/*扫描就绪列表中所有任务的xTicksToDelay，如果不为0，则减1 */
//	for (i=0; i<configMAX_PRIORITIES; i++)
//	{
//	
//		pxTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY((&pxReadyTasksLists[i]));/* 新加入list.h中 */
//		if (pxTCB->xTicksToDelay > 0)
//		{
//			pxTCB->xTicksToDelay --;
//			
//			/*延时时间到，将任务就绪*/
//			if ( pxTCB->xTicksToDelay ==0 )
//			{
//				taskRECORD_READY_PRIORITY( pxTCB->uxPriority );
//			}
//		}
//	}
//	
	/*任务切换*/
	taskYIELD();
	
}

__asm void vPortSVCHandler( void )
{	

		extern pxCurrentTCB;
	
		PRESERVE8	
	
		ldr r3, =pxCurrentTCB 
		ldr r1,[r3]					  
		ldr r0,[r1]					  
		ldmia r0!, {r4-r11}   
		msr psp,r0            
		isb                   
		mov r0,#0             
		msr basepri,r0        
		orr r14,#0xd          
		bx r14                
			
}

__asm void xPortPendSVHandler( void )
{

		extern vTaskSwitchContext;
		extern pxCurrentTCB;	
		                                              
		PRESERVE8                                     
		                                              
		mrs r0, psp                                   
		isb                                           
		                                              
		ldr r3, = pxCurrentTCB                        
		ldr r2,[r3] 

		stmdb r0!,{r4-r11}
		str r0,[r2]
		                                              
		stmdb sp!, {r3, r14}                          
		mov r0, #configMAX_SYSCALL_INTERRUPT_PRIORITY 
		msr basepri,r0                                
		dsb                                           
		isb                                           
		bl vTaskSwitchContext                         
		mov r0,#0                                     
		msr basepri,r0                                
		ldmia sp!, {r3, r14}                          
		                                              
		ldr r1,[r3]                                   
		ldr r0,[r1]                                   
		ldmia r0!,{r4-r11}                            
		msr psp,r0                                    
		isb                                           
		bx r14                                        
		nop                                           
	
}

#endif


	
static void prvAddNewTaskToReadyList( TCB_t *pxNewTCB )
{
	/*进入临界段*/
	taskENTER_CRITICAL();
	{
		/*全局任务定时器加1操作*/
		uxCurrentNumberOfTasks++;
		/*如果pxCurrentTCB为空，则将pxCurrentTCB指向新创建的任务*/
		if ( pxCurrentTCB == NULL )
		{
			pxCurrentTCB = pxNewTCB;
			/*如果是第一次创建任务，则需要初始化任务的相关列表*/
			if ( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
			{
				/*初始化任务的相关列表*/
				prvInitialiseTaskLists();
			}
		}	
		else/*如果pxCurrentTCB不为空则根据任务的优先级将pxCurrentTCB指向最高优先级任务的TCB */
		{
			if ( pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority )
			{
				pxCurrentTCB = pxNewTCB;
			}
		}
		
		/*将任务添加到就绪列表*/
		prvAddTaskToReadyList( pxNewTCB );
		
	}
	/*退出临界段*/
	taskEXIT_CRITICAL();
}

static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait )
{
	TickType_t xTimeToWake;
	
	/*获取系统时基计数器xTickCount的值*/
	const TickType_t xConstTickCount = xTickCount;
	
	/*将任务从就绪列表中移除*/
	if(uxListRemove( &( pxCurrentTCB->xStateListItem )) == ( UBaseType_t )0 )
	{
		/* 将任务在优先级位图中对应的位清除 */
		portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority,uxTopReadyPriority );
	}
	
	/*计算任务延时到期时，系统时基计数器xTickCount的值是多少*/
	xTimeToWake = xConstTickCount + xTicksToWait;
	
	/*将延时到期的值设置为节点的排序值*/
	listSET_LIST_ITEM_VALUE(&( pxCurrentTCB->xStateListItem ), xTimeToWake);
	
	/*溢出*/
	if ( xTimeToWake < xConstTickCount )
	{
		vListInsert( pxOverflowDelayedTaskList,&( pxCurrentTCB->xStateListItem ) );
	}
	else/* 没有溢出*/
	{
		vListInsert( pxDelayedTaskList,&( pxCurrentTCB->xStateListItem ) );
		/*更新下一个任务解锁时刻变量xNextTaskUnblockTime的值*/
		if ( xTimeToWake < xNextTaskUnblockTime )
		{
			xNextTaskUnblockTime = xTimeToWake;
		}
	}
}

#endif
