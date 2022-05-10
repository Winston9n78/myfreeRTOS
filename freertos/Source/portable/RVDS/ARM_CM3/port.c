#include "portmacro.h"
#include "task.h"

#define portINITIAL_XPSR ( 0x01000000 )
#define portSTART_ADDRESS_MASK (( StackType_t ) 0xfffffffeUL)

static unsigned long int uxCriticalNesting = 0xaaaaaaaaUL;

/* 断言处理待补充，可以是关闭所以中断后进入死循环 */
#define configASSERT(x) if(x==0){for(;;);} 

#define portVECTACTIVE_MASK 0x000000ffUL

void vPortSetupTimerInterrupt( void );

unsigned int xTickCount;

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
	
	/* 初始化SysTick */
	vPortSetupTimerInterrupt();
	
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

/*SysTick控制寄存器*/
#define portNVIC_SYSTICK_CTRL_REG (*((volatile uint32_t *)0xe000e010 ))
/*SysTick重装载寄存器*/
#define portNVIC_SYSTICK_LOAD_REG (*((volatile uint32_t *) 0xe000e014 ))

/* SysTick 时钟源选择 */
#ifndef configSYSTICK_CLOCK_HZ
	#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
	/*确保SysTick的时钟与内核时钟一致*/
	#define portNVIC_SYSTICK_CLK_BIT ( 1UL<<2UL )
#else
	#define portNVIC_SYSTICK_CLK_BIT (0)
#endif

#define portNVIC_SYSTICK_INT_BIT ( 1UL<<1UL)
#define portNVIC_SYSTICK_ENABLE_BIT (1UL<<0UL )

void vPortSetupTimerInterrupt( void )
{
	/*设置重装载寄存器的值*/
	portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1;
	
	/*设置系统定时器的时钟等于内核时钟
	使能SysTick定时器中断
	使能SysTick定时器*/
	portNVIC_SYSTICK_CTRL_REG= ( portNVIC_SYSTICK_CLK_BIT |
																portNVIC_SYSTICK_INT_BIT |
																portNVIC_SYSTICK_ENABLE_BIT );
}

/* SysTick中断服务函数 */
void xPortSysTickHandler( void )
{
//	/*关中断*/
//	vPortRaiseBASEPRI();
//	/*更新系统时基*/
//	xTaskIncrementTick();
//	/*开中断*/
//	vPortClearBASEPRIFromISR();
	uint32_t ulReturn;
	/*进入临界段，临界段可以嵌套*/
	ulReturn = taskENTER_CRITICAL_FROM_ISR();
	/*临界段代码*/
	xTaskIncrementTick();
	/*退出临界段*/
	taskEXIT_CRITICAL_FROM_ISR(ulReturn);
}






