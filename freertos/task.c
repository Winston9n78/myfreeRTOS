#include "task.h"

#if(configSUPPORT_STATIC_ALLOCATION == 1)

TCB_t * pxCurrentTCB;

List_t pxReadyTasksLists[ configMAX_PRIORITIES ];

TCB_t Task1TCB;
TCB_t Task2TCB;

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


/* �����л� */
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
