#include "portmacro.h"
#include "task.h"

#define portINITIAL_XPSR ( 0x01000000 )
#define portSTART_ADDRESS_MASK (( StackType_t ) 0xfffffffeUL)

static unsigned long int uxCriticalNesting = 0xaaaaaaaaUL;

/* 断言处理待补充，可以是关闭所以中断后进入死循环 */
#define configASSERT(x) if(x==0){for(;;);} 

#define portVECTACTIVE_MASK 0x000000ffUL

static void prvTaskExitError ( void )
{
	/*函数停止在这里*/
	for (;;);

}

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack,
																		TaskFunction_t pxCode,
																		void *pvParameters )
{
	/*异常发生时，自动加载到CPU寄存器的内容*/
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;
	pxTopOfStack--;
	*pxTopOfStack = ( ( StackType_t ) pxCode ) & portSTART_ADDRESS_MASK;
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) prvTaskExitError ;
	pxTopOfStack -= 5; /* r12、r3、r2和r1默认初始化为0 */
	*pxTopOfStack = ( StackType_t ) pvParameters;
	
	/*异常发生时，手动加载到CPU寄存器的内容*/
	pxTopOfStack -= 8;
	
	/*返回栈顶指针，此时pxTopOfStack指向空闲栈*/
	return pxTopOfStack;
	
}

__asm void prvStartFirstTask ( void )
{

		PRESERVE8																	
		/*在Cortex-M中，OxEOOOEDO8是SCB_VTOR这个寄存器的地址，
		(2)里面存放的是向量表的起始地址，即msp的地址*/
		ldr r0,=0xE000ED08												
		ldr r0,[r0]                              
		ldr r0,[r0]                              
		/*设置主栈指针msp的值*/               
		msr msp,r0                               
		                                         
		/*启用全局中断*/
		cpsie i                                  
		cpsie f                                  
		dsb                                      
		isb                                      
		                                         
		/*调用SVC启动第一个任务*/                
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
	/*配置PendSV和 SysTick 的中断优先级为最低*/
	portNVIC_SYSPRI2_REG |= portNVIC_PENDSV_PRI;
	portNVIC_SYSPRI2_REG |= portNVIC_SYSTICK_PRI;
	
	/*启动第一个任务，不再返回*/
	prvStartFirstTask();

	/*不应该运行到这里*/
	return 0;	
}

/*===进入临界段，不带中断保护版本，不能嵌套====*/
void vPortEnterCritical( void )
{

	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
	if ( uxCriticalNesting == 1 )
	{
		configASSERT(( portNVIC_INT_CTRL_REG & portVECTACTIVE_MASK) == 0 );
	}

}

/*==退出临界段，不带中断保护版本，不能嵌套==*/
void vPortExitCritical( void )
{

	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if ( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}

}






