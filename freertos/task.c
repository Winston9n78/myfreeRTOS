#include "task.h"

#if(configSUPPORT_STATIC_ALLOCATION == 1)

TCB_t * pxCurrentTCB;

List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

extern TCB_t Task1TCB;
extern TCB_t Task2TCB;

extern TCB_t IdleTaskTCB;



//void vApplicationGetIdleTaskMemory( TCB_t **ppxIdleTaskTCBBuffer,
//																		StackType_t **ppxIdleTaskStackBuffer,
//																		uint32_t *pulIdleTaskStackSize );

static void prvInitialiseNewTask(	TaskFunction_t pxTaskCode,
																	const char *const pcName,
																	const uint32_t ulStackDepth,
																	void * const pvParameters,
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
	
	if( ( void * ) pxCreatedTask != NULL)
	{
		*pxCreatedTask = (TaskHandle_t ) pxNewTCB;
	}
}

	


TaskHandle_t xTaskCreateStatic(TaskFunction_t pxTaskCode,
																const char *const pcName,
																const uint32_t ulStackDepth,
															  void * const pvParameters,
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
													&xReturn,		 		/*������*/
													pxNewTCB);	 		/*����ջ��ʼ��ַ*/
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

}

/* ���������� */
void vTaskStartScheduler( void )
{
	

	
	/* �ֶ�ָ����һ�����е����� */
	pxCurrentTCB = &Task1TCB;
	
	/* ���������� */
	if ( xPortStartScheduler() != pdFALSE )
	{
		/* �����������ɹ����򲻻᷵�أ��������������� */
	}
}

/* ������ʱ���� */
void vTaskDelay(const TickType_t xTicksToDelay)
{
	TCB_t *pxTCB = NULL;
	/*��ȡ��ǰ�����TCB*/
	pxTCB = pxCurrentTCB;
	
	/*������ʱʱ��*/
	pxTCB->xTicksToDelay = xTicksToDelay;
	
	/*�����л�*/
	taskYIELD(); /*����PendSv�������������л�*/ 
}

/* �����л� */
#if 0
void vTaskSwitchContext ( void )
{
	/*�������������л�*/
	if ( pxCurrentTCB == &Task1TCB )
	{
		pxCurrentTCB = &Task2TCB;
	}
	
	else
	{
		pxCurrentTCB = &Task1TCB;
	}
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

extern unsigned int xTickCount;
void xTaskIncrementTick( void )
{
	TCB_t*pxTCB = NULL;
	BaseType_t i = 0;
	/*����ϵͳʱ��������xTickCount��xTickCount��һ����port.c�ж����ȫ�ֱ���*/
	const TickType_t xConstTickCount = xTickCount + 1;
	xTickCount = xConstTickCount;
	
	/*ɨ������б������������xTicksToDelay�������Ϊ0�����1 */
	for (i=0; i<configMAX_PRIORITIES; i++)
	{
	
		pxTCB = (TCB_t *)listGET_OWNER_OF_HEAD_ENTRY((&pxReadyTasksLists[i]));/* �¼���list.h�� */
		if (pxTCB->xTicksToDelay > 0)
		{
			pxTCB->xTicksToDelay --;
		}
	}
	
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
