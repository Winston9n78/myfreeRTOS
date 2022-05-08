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

/*����PendSv�������������л�*/ 
#define portYIEIL() {portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT; __dsb( portSY_FULL_READ_WRITE ); __isb( portSY_FULL_READ_WRITE);}


#if( configUSE_16_BIT_TICKS == 1 ) /**/
typedef uint16_t TickType_t;
#define portMAX_ DELAY �� TickType_t ) Oxffff
#else
typedef uint32_t TickType_t;

#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

///*************************************************************************************************
//*���ж�ʵ�֣��������벻������
//**************************************************************************************************/
///* �������ع��жϣ��������ж���Ƕ��ʹ�� */
//#define portDISABLE_INTERRUPTS() vPortRaiseBASEPRI()
//void vPortRaiseBASEPRI( void ){
//	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
//	__asm{
//		msr basepri,ulNewBASEPRI
//		dsb
//		isb
//	}
//}
///* �����ع��жϣ��������ж���Ƕ��ʹ�� */
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
//*���ж�ʵ�֣��������벻������
//**************************************************************************************************/
///*�����жϱ����Ŀ��жϺ���*/
//#define portENABLE_INTERRUPTS() vPortSetBASEPRI(0)
///*���жϱ����Ŀ��жϺ���*/
//#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
//void vPortSetBASEPRI( uint32_t ulBASEPRI ){
//	__asm
//	{
//		msr basepri,ulBASEPRI
//	}
//}

/*===�����ٽ�Σ������жϱ����汾������Ƕ��====*/
#define portENTER_CRITICAL() vPortEnterCritical()
/*===�����ٽ�Σ������жϱ����汾������Ƕ��====*/
#define portDISABLE_INTERRUPTS() vPortRaiseBASEPRI()
/*===�����ٽ�Σ������жϱ����汾������Ƕ��====*/
static portFORCE_INLINE void vPortRaiseBASEPRI( void ){

	uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;
	__asm
	{
		msr basepri,ulNewBASEPRI
		dsb
		isb
	}
}

/*===�����ٽ�Σ����жϱ����汾������Ƕ��==== */
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

/*==�˳��ٽ�Σ������жϱ����汾������Ƕ��==*/
#define portEXIT_CRITICAL() vPortExitCritical()
#define portENABLE_INTERRUPTS() vPortSetBASEPRI(0)
/*==�˳��ٽ�Σ����жϱ����汾������Ƕ��==*/
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
static portFORCE_INLINE void vPortSetBASEPRI(uint32_t ulBASEPRI )
{
	__asm
	{
		msr basepri,ulBASEPRI
	}
}

#endif/*PORTMACRO_H*/
