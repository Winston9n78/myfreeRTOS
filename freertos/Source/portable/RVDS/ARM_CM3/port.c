#include "portmacro.h"
#include "task.h"

#define portINITIAL_XPSR ( 0x01000000 )
#define portSTART_ADDRESS_MASK (( StackType_t ) 0xfffffffeUL)

static unsigned long int uxCriticalNesting = 0xaaaaaaaaUL;

/* ���Դ��������䣬�����ǹر������жϺ������ѭ�� */
#define configASSERT(x) if(x==0){for(;;);} 

#define portVECTACTIVE_MASK 0x000000ffUL

static void prvTaskExitError ( void )
{
	/*����ֹͣ������*/
	for (;;);

}

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack,
																		TaskFunction_t pxCode,
																		void *pvParameters )
{
	/*�쳣����ʱ���Զ����ص�CPU�Ĵ���������*/
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;
	pxTopOfStack--;
	*pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) prvTaskExitError ;
	pxTopOfStack -= 5; /* r12��r3��r2��r1Ĭ�ϳ�ʼ��Ϊ0 */
	*pxTopOfStack = ( StackType_t ) pvParameters;
	
	/*�쳣����ʱ���ֶ����ص�CPU�Ĵ���������*/
	pxTopOfStack -= 8;
	
	/*����ջ��ָ�룬��ʱpxTopOfStackָ�����ջ*/
	return pxTopOfStack;
	
}

__asm void prvStartFirstTask ( void )
{

		PRESERVE8																	
		/*��Cortex-M�У�OxEOOOEDO8��SCB_VTOR����Ĵ����ĵ�ַ��
		(2)�����ŵ�������������ʼ��ַ����msp�ĵ�ַ*/
		ldr r0,=0xE000ED08												
		ldr r0,[r0]                              
		ldr r0,[r0]                              
		/*������ջָ��msp��ֵ*/               
		msr msp,r0                               
		                                         
		/*����ȫ���ж�*/
		cpsie i                                  
		cpsie f                                  
		dsb                                      
		isb                                      
		                                         
		/*����SVC������һ������*/                
		svc 0                                    
		nop                                      
		nop                                      
	
}

#define portNVIC_SYSPRI2_REG (*(( volatile uint32_t *) 0xe000ed20))
	
#define portNVIC_PENDSV_PRI (((uint32_t) configKERNEL_INTERRUPT_PRIORITY ) << 16UL)
#define portNVIC_SYSTICK_PRI (((uint32_t) configKERNEL_INTERRUPT_PRIORITY ) << 24UL)

BaseType_t xPortStartScheduler( void )
{	
	uxCriticalNesting = 0;
	/*����PendSV�� SysTick ���ж����ȼ�Ϊ���*/
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	
	/*������һ�����񣬲��ٷ���*/
	prvStartFirstTask();

	/*��Ӧ�����е�����*/
	return 0;	
}

/*===�����ٽ�Σ������жϱ����汾������Ƕ��====*/
void vPortEnterCritical( void )
{

	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
	if ( uxCriticalNesting == 1 )
	{
		configASSERT(( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0 );
	}

}

/*==�˳��ٽ�Σ������жϱ����汾������Ƕ��==*/
void vPortExitCritical( void )
{

	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if ( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}

}





