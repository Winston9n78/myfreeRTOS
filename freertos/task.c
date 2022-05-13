#include "task.h"

#if(configSUPPORT_STATIC_ALLOCATION == 1)

TCB_t * pxCurrentTCB;

List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

extern TCB_t Task1TCB;
extern TCB_t Task2TCB;
extern TCB_t Task3TCB;
extern TCB_t IdleTaskTCB;

extern unsigned int xTickCount;
/*����uxTopReadyPriority����task.c�ж���*/
static volatile UBaseType_t uxTopReadyPriority = tskIDLE_PRIORITY;

static List_t xDelayedTaskList1; //������ʱ�б�1�������
static List_t xDelayedTaskList2; //������ʱ�б�2�����
static List_t *volatile pxDelayedTaskList; //���������ʱ�б��ָ��
static List_t *volatile pxOverflowDelayedTaskList; //�������ʱ�б��ָ��

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
		/*��ǰ��ʱ�б�Ϊ�գ�������xNextTaskUnblockTime�������ֵ*/
		xNextTaskUnblockTime = portMAX_DELAY;
	}
	else
	{
		/*��ǰ�б�Ϊ�գ���ʾ����������ʱ�����ȡ��ǰ�б��µ�һ���ڵ������ֵȻ�󽫸ýڵ������ֵ���µ�xNextTaskUnblockTime */
		( pxTCB ) = ( TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE(&( ( pxTCB )->xStateListItem ) );
	}
	
}
/*����������ȼ��ľ�������ͨ�÷�����*/
#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )

	/*uxTopReadyPriority�洢���Ǿ��������������ȼ�*/
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
		/*Ѱ�Ұ������������������ȼ��Ķ���*/ \
		while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopPriority ] ) )) \
		{ \
			--uxTopPriority; \
		} \
		/*��ȡ���ȼ���ߵľ��������TCB��Ȼ����µ�pxCurrentTCB*/ \
		listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, &(pxReadyTasksLists[ uxTopPriority ])); \
		/*����uxTopReadyPriority */ \
		uxTopReadyPriority = uxTopPriority; \
	}/* taskSELECT_HIGHEST_PRIORITY_TASK */

	/*---------------------------------------------------------------*/
/*�������궨��ֻ����ѡ���Ż�����ʱ���ã����ﶨ��Ϊ��*/
#define taskRESET_READY_PRIORITY( uxPriority )
#define portRESET_READY_PRIORITY( uxPriority,uxTopReadyPriority )	
	
/*����������ȼ��ľ������񣨸��ݴ������ܹ��Ż���ķ�����*/
#else /* configUSE_PORT_OPTIMISED_TASK_SELECTION */
	#define taskRECORD_READY_PRIORITY(uxPriority ) \
						portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )
/*---------------------------------------------------------------*/
#define taskSELECT_HIGHEST_PRIORITY_TASK() { UBaseType_t uxTopPriority; /*Ѱ��������ȼ�*/portGET_HIGHEST_PRIORITY ( uxTopPriority,uxTopReadyPriority ); /*��ȡ���ȼ���ߵľ��������TCB��Ȼ����µ�pxCurrentTCB */ listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB,&( pxReadyTasksLists[ uxTopPriority ] ));}	/* taskSELECT_HIGHEST_PRIORITY_TASK()*/
	
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
	
#if 1 /* ������ʱ�б�ʵ�ַ��� */
#define taskRESET_READY_PRIORITY ( uxPriority ) \
{ \
	if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ])) == ( UBaseType_t ) 0 ) \
	{ \
		portRESET_READY_PRIORITY( ( uxPriority )��( uxTopReadyPriority ) ); \
	} \
}

#else /* ֮ǰ��ʵ�ַ��� */
#define taskRESET_READY_PRIORITY ( uxPriority ) \
{ \
	portRESET_READY_PRIORITY( ( uxPriority ),( uxTopReadyPriority ) );\
}
#endif


/*��������ӵ������б�*/
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
																	/*�������ȼ�����ֵԽ�����ȼ�Խ��*/
																	UBaseType_t uxPriority,
																	TaskHandle_t *const pxCreatedTask,
																	TCB_t *pxNewTCB )
{
	StackType_t *pxTopOfStack;
	UBaseType_t x;
	/* ��ȡջ����ַ */
	pxTopOfStack = pxNewTCB->pxStack + ( ulStackDepth - ( uint32_t ) 1);
	/* ������8�ֽڶ��� */
	pxTopOfStack = ( StackType_t * ) \
									(( ( uint32_t ) pxTopOfStack )&(~( ( uint32_t ) 0x0007 ) ));
	
	/*���������洢��TCB�� */
	for ( x = ( UBaseType_t )0; x < ( UBaseType_t ) configMAX_TASK_NAME_LEN; x++ )
	{
		pxNewTCB->pcTaskName[ x ] = pcName[ x ];
		
		if ( pcName[ x ] == 0x00 )
		{
			break;
		}
	}
	
	/*�������ĳ��Ȳ��ܳ���configMAX_TASK_NAME_LEN */
	pxNewTCB->pcTaskName[ configMAX_TASK_NAME_LEN -1 ]= '\0';
	
	/*��ʼ��TCB�е�xStateListItem�ڵ�*/
	vListInitialiseItem( &( pxNewTCB->xStateListItem ) );
	
	/*����xStateListItem�ڵ��ӵ����*/
	listSET_LIST_ITEM_OWNER(&( pxNewTCB->xStateListItem ), pxNewTCB );
	
	/*��ʼ������ջ*/
	pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack,
																									pxTaskCode,
																									pvParameters);
	
	/*��ʼ�����ȼ�*/
	if ( uxPriority >= ( UBaseType_t ) configMAX_PRIORITIES )
	{
		uxPriority = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) 1U;
	}
	pxNewTCB->uxPriority = uxPriority;
	/*��ʼ������ջ*/
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
																/*�������ȼ�����ֵԽ�����ȼ�Խ��*/
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
		
		/* �����µ����� */
		prvInitialiseNewTask( pxTaskCode,  		/*�������*/
													pcName,			 		/*�������ƣ��ַ�����ʽ*/
													ulStackDepth,		/*����ջ��С����λΪ��*/
													pvParameters,		/*�����β�*/
													uxPriority,
													&xReturn,		 		/*������*/
													pxNewTCB);	 		/*����ջ��ʼ��ַ*/
		
		prvAddNewTaskToReadyList(pxNewTCB);
	}
	else
	{
		xReturn = NULL;
	}

	/*������������������񴴽��ɹ�����ʱxReturnӦ��ָ��������ƿ�*/
	return xReturn;
}


/*��������б��ʼ��*/
void prvInitialiseTaskLists( void )
{
	UBaseType_t uxPriority;
	for ( uxPriority = (UBaseType_t ) 0U;
				uxPriority <(UBaseType_t ) configMAX_PRIORITIES;uxPriority++ )
	{
		vListInitialise( &( pxReadyTasksLists[ uxPriority ] ) );
	}
	
	//��ʼ����ʱ�б�
	vListInitialise(&xDelayedTaskList1 );
	vListInitialise( &xDelayedTaskList2 );
	pxDelayedTaskList = &xDelayedTaskList1;
	pxOverflowDelayedTaskList = &xDelayedTaskList2;
}

/* ���������� */
void vTaskStartScheduler( void )
{
	
	/* �ֶ�ָ����һ�����е����� */
	//pxCurrentTCB = &Task1TCB;
	
	/* ��ʼ��xNextTaskUnblockTime */
	xNextTaskUnblockTime = portMAX_DELAY;
	xTickCount = ( TickType_t ) 0U;
	/* ���������� */
	if ( xPortStartScheduler() != pdFALSE )
	{
		/* �����������ɹ����򲻻᷵�أ��������������� */
	}
}

/* ������ʱ���� */
void vTaskDelay(const TickType_t xTicksToDelay)
{
	//TCB_t *pxTCB = NULL;
	/*��ȡ��ǰ�����TCB*/
	//pxTCB = pxCurrentTCB;
	
	/*������ʱʱ��*/
	//pxTCB->xTicksToDelay = xTicksToDelay;
	
	/*�����������ʱ�б�*/
	prvAddCurrentTaskToDelayedList( xTicksToDelay );
	
	/*������Ӿ����б����Ƴ�*/
	// uxListRemove( &( pxTCB->xStateListItem ));
	// taskRESET_READY_PRIORITY( pxTCB->uxPriority );
	
	/*�����л�*/
	taskYIELD(); /*����PendSv�������������л�*/ 
}

/* �����л� */
#if 1
void vTaskSwitchContext ( void )
{
	/*�������������л�*/
//	if ( pxCurrentTCB == &Task1TCB )
//	{
//		pxCurrentTCB = &Task2TCB;
//	}
//	
//	else
//	{
//		pxCurrentTCB = &Task1TCB;
//	}
	/*��ȡ���ȼ���ߵľ��������TCB��Ȼ����µ�pxCurrentTCB */
	taskSELECT_HIGHEST_PRIORITY_TASK();
}

#else
void vTaskSwitchContext ( void )
{
	/*�����ǰ�����ǿ���������ô��ȥ����ִ������1��������2��
	�������ǵ���ʱ�Ƿ����������������ʱ��û�е��ڣ��򷵻أ�����ִ�п�������*/
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
			return; /*������ʱ��û�е����򷵻أ�����ִ�п�������*/
		}
	}
	
	else /*��ǰ�����ǿ����������ִ�е�����*/
	{
		/*�����ǰ����������1��������2���������һ������
		����������������ʱ�У����л���������
		�����жϵ�ǰ�����Ƿ�Ӧ�ý�����ʱ״̬��
		����ǣ����л����������񣬷��򲻽����κ��л�*/
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
				return; /*���أ��������л�,��Ϊ�������񶼴�����ʱ��*/
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
				return; /*���أ��������л�,��Ϊ�������񶼴�����ʱ��*/
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
	/*����ϵͳʱ��������xTickCount��xTickCount��һ����port.c�ж����ȫ�ֱ���*/
	const TickType_t xConstTickCount = xTickCount + 1;
	xTickCount = xConstTickCount;
	
	/*���xConstTickCount��������л���ʱ�б�*/
	if ( xConstTickCount == ( TickType_t ) 0U )
	{
		taskSWITCH_DELAYED_LISTS();
	}
	
	/*�������ʱ������ʱ����*/
	if ( xConstTickCount >= xNextTaskUnblockTime )
	{
		for(;;)
		{
			if ( listLIST_IS_EMPTY ( pxDelayedTaskList ) != pdFALSE )
			{
				/*��ʱ�б�Ϊ�գ�����xNextTaskUnblockTimeΪ���ܵ����ֵ*/
				xNextTaskUnblockTime = portMAX_DELAY;
				break;
			}
			else
			{
				pxTCB = (TCB_t * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList);
				xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );
				
				/*ֱ������ʱ�б���������ʱ���ڵ������Ƴ�������forѭ��*/
				if ( xConstTickCount< xItemValue )
				{
					xNextTaskUnblockTime = xItemValue;
					break ;
				}
				
				/*���������ʱ�б����Ƴ�������ȴ�״̬*/
				( void ) uxListRemove(&( pxTCB->xStateListItem ) );
				/*������ȴ���������ӵ������б�*/
				prvAddTaskToReadyList( pxTCB );
			}
		}
	}/*xConstTickCount >=xNextTaskUnblockTime */
	
	/*ɨ������б������������xTicksToDelay�������Ϊ0�����1 */
//	for (i=0; i<configMAX_PRIORITIES; i++)
//	{
//	
//		pxTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY((&pxReadyTasksLists[i]));/* �¼���list.h�� */
//		if (pxTCB->xTicksToDelay > 0)
//		{
//			pxTCB->xTicksToDelay --;
//			
//			/*��ʱʱ�䵽�����������*/
//			if ( pxTCB->xTicksToDelay ==0 )
//			{
//				taskRECORD_READY_PRIORITY( pxTCB->uxPriority );
//			}
//		}
//	}
//	
	/*�����л�*/
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
	/*�����ٽ��*/
	taskENTER_CRITICAL();
	{
		/*ȫ������ʱ����1����*/
		uxCurrentNumberOfTasks++;
		/*���pxCurrentTCBΪ�գ���pxCurrentTCBָ���´���������*/
		if ( pxCurrentTCB == NULL )
		{
			pxCurrentTCB = pxNewTCB;
			/*����ǵ�һ�δ�����������Ҫ��ʼ�����������б�*/
			if ( uxCurrentNumberOfTasks == ( UBaseType_t ) 1 )
			{
				/*��ʼ�����������б�*/
				prvInitialiseTaskLists();
			}
		}	
		else/*���pxCurrentTCB��Ϊ���������������ȼ���pxCurrentTCBָ��������ȼ������TCB */
		{
			if ( pxCurrentTCB->uxPriority <= pxNewTCB->uxPriority )
			{
				pxCurrentTCB = pxNewTCB;
			}
		}
		
		/*��������ӵ������б�*/
		prvAddTaskToReadyList( pxNewTCB );
		
	}
	/*�˳��ٽ��*/
	taskEXIT_CRITICAL();
}

static void prvAddCurrentTaskToDelayedList( TickType_t xTicksToWait )
{
	TickType_t xTimeToWake;
	
	/*��ȡϵͳʱ��������xTickCount��ֵ*/
	const TickType_t xConstTickCount = xTickCount;
	
	/*������Ӿ����б����Ƴ�*/
	if(uxListRemove( &( pxCurrentTCB->xStateListItem )) == ( UBaseType_t )0 )
	{
		/* �����������ȼ�λͼ�ж�Ӧ��λ��� */
		portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority,uxTopReadyPriority );
	}
	
	/*����������ʱ����ʱ��ϵͳʱ��������xTickCount��ֵ�Ƕ���*/
	xTimeToWake = xConstTickCount + xTicksToWait;
	
	/*����ʱ���ڵ�ֵ����Ϊ�ڵ������ֵ*/
	listSET_LIST_ITEM_VALUE(&( pxCurrentTCB->xStateListItem ), xTimeToWake);
	
	/*���*/
	if ( xTimeToWake < xConstTickCount )
	{
		vListInsert( pxOverflowDelayedTaskList,&( pxCurrentTCB->xStateListItem ) );
	}
	else/* û�����*/
	{
		vListInsert( pxDelayedTaskList,&( pxCurrentTCB->xStateListItem ) );
		/*������һ���������ʱ�̱���xNextTaskUnblockTime��ֵ*/
		if ( xTimeToWake < xNextTaskUnblockTime )
		{
			xNextTaskUnblockTime = xTimeToWake;
		}
	}
}

#endif
