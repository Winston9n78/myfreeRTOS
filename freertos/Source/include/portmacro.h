#ifndef PORTMACRO_H
#define PORTMACRO_H

#include "stdint.h"
#include "stddef.h"
#include "FreeRTOSConfig.h"

#ifndef portFORCE_INLINE
	#define portFORCE_INLINE inline __attribute__((always_inline))
#endif

#define portCHAR char
#define portFLOAT float
#define portDOUBLE double
#define portLONG long
#define portSHORT short
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#define portNVIC_INT_CTRL_REG (*(( volatile uint32_t *) 0xe000ed04))
#define portNVIC_PENDSVSET_BIT ( 1UL<<28UL )
#define portSY_FULL_READ_WRITE (15)

/*触发PendSv，产生上下文切换*/ 
#define portYIEIL() {portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; __dsb( portSY_FULL_READ_WRITE ); __isb( portSY_FULL_READ_WRITE);}


#if( configUSE_16_BIT_TICKS == 1 ) /**/
typedef uint16_t TickType_t;
#define portMAX_ DELAY （ TickType_t ) Oxffff
#else
typedef uint32_t TickType_t;

#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

///*************************************************************************************************
//*关中断实现，带返回与不带返回
//**************************************************************************************************/
///* 不带返回关中断，不能在中断中嵌套使用 */
//#define portDISABLE_INTERRUPTS() vPortRaiseBASEPRI()
//void vPortRaiseBASEPRI( void ){
//	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
//	__asm{
//		msr basepri,ulNewBASEPRI
//		dsb
//		isb
//	}
//}
///* 带返回关中断，可以在中断中嵌套使用 */
//#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortRaiseBASEPRI()
// uint32_t ulPortRaiseBASEPRI( void )
//{
//	uint32_t ulReturn,ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
//	__asm
//	{
//		mrs ulReturn,basepri
//		msr basepri,ulNewBASEPRI
//		dsb
//		isb
//	}
//	return ulReturn;
//}

///*************************************************************************************************
//*开中断实现，带保护与不带保护
//**************************************************************************************************/
///*不带中断保护的开中断函数*/
//#define portENABLE_INTERRUPTS() vPortSetBASEPRI(0)
///*带中断保护的开中断函数*/
//#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
//void vPortSetBASEPRI( uint32_t ulBASEPRI ){
//	__asm
//	{
//		msr basepri,ulBASEPRI
//	}
//}

/*===进入临界段，不带中断保护版本，不能嵌套====*/
#define portENTER_CRITICAL() vPortEnterCritical()
/*===进入临界段，不带中断保护版本，不能嵌套====*/
#define portDISABLE_INTERRUPTS() vPortRaiseBASEPRI()
/*===进入临界段，不带中断保护版本，不能嵌套====*/
static portFORCE_INLINE void vPortRaiseBASEPRI( void ){

	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm
	{
		msr basepri,ulNewBASEPRI
		dsb
		isb
	}
}

/*===进入临界段，带中断保护版本，可以嵌套==== */
#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortRaiseBASEPRI()
static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{

	uint32_t ulReturn,ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	
	__asm
	{
		mrs ulReturn,basepri
		msr basepri,ulNewBASEPRI
		dsb
		isb		
	}
	return ulReturn; 
}

/*==退出临界段，不带中断保护版本，不能嵌套==*/
#define portEXIT_CRITICAL() vPortExitCritical()
#define portENABLE_INTERRUPTS() vPortSetBASEPRI(0)
/*==退出临界段，带中断保护版本，可以嵌套==*/
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
static portFORCE_INLINE void vPortSetBASEPRI(uint32_t ulBASEPRI )
{
	__asm
	{
		msr basepri,ulBASEPRI
	}
}

#endif/*PORTMACRO_H*/
