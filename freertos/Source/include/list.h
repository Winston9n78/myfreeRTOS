#ifndef LIST_H
#define LIST_H

#include "portmacro.h"

struct xLIST_ITEM
{
	/*
		�����漰�������͵ĵط���freertos������׼C������������һ����������
		�����ض�����������ͷ���portmacro.h
	*/
	TickType_t xItemValue; /*����ֵ�����ڰ����ڵ����˳������*/
	
	struct xLIST_ITEM * pxNext;/*ָ��������һ�ڵ�Ľṹ��ָ��*/
	struct xLIST_ITEM * pxPrevious;/*ָ��������һ�ڵ�Ľṹ��ָ��*/
	
	void * pvOwner;/*ָ��ӵ�иýڵ���ں˶��������ĸ����ݽṹ��Ա��ͨ����TCB (�������)*/
	void * pvContainer;/*ָ��ýڵ���������ͨ��ָ��������ڵ�*/
	
};

typedef struct xLIST_ITEM ListItem_t;/*�ڵ����������ض���*/

/*�����ߣ���������δ����Ľڵ�*/
struct xMINI_LIST_ITEM{
	
	TickType_t xItemValue; /*����ֵ,���ڰ����ڵ������������*/
	struct xLIST_ITEM *pxNext;/*ָ��������һ���ڵ�*/
	struct xLIST_ITEM * pxPrevious;/*ָ��������һ���ڵ�*/
	
};
typedef struct xMINI_LIST_ITEM MiniListItem_t; /*����ڵ����������ض���*/

/*������ڵ����ݽṹ����*/
typedef struct xLIST{
	
	UBaseType_t uxNumberOfItems; /*����ڵ������*/
	ListItem_t * pxIndex; /*����ڵ�����ָ��*/
	MiniListItem_t xListEnd; /*�������һ���ڵ�*/
	
}List_t;

void vListInitialiseItem( ListItem_t *const pxItem );
void vListInitialise( List_t *const pxList );
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem );
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem);
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove );

/* �ڵ���κ꺯�� */

/*��ʼ���ڵ��ӵ����*/
#define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner )\
	( ( pxListItem )->pvOwner = ( void * ) ( pxOwner ) )
		
/*��ȡ�ڵ�ӵ����*/
#define listGET_LIST_ITEM_OWNER( pxListItem )\
	( ( pxListItem )->pvOwner )
	
/*��ʼ���ڵ�������ֵ*/
#define listSET_LIST_ITEM_VALUE( pxListItem, xValue )\
	( ( pxListItem )->xItemValue = ( xValue ) )
	
/*��ȡ�ڵ�������ֵ*/
#define listGET_LIST_ITEM_VALUE( pxListItem )\
	( ( pxListItem )->xItemValue )
	
/*��ȡ������ڵ�Ľڵ��������ֵ*/
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxList )\
	( (( pxList )->xListEnd ).pxNext->xItemValue )
	
/*��ȡ�������ڽڵ�*/
#define listGET_HEAD_ENTRY( pxList )\
	(( ( pxList )->xListEnd ).pxNext )
	
/*��ȡ�ڵ����һ���ڵ�*/
#define listGET_NEXT( pxListItem )\
	(( pxListItem )->pxNext )
	
/*��ȡ��������һ���ڵ�*/
#define listGET_END_MARKER( pxList )\
	(( ListItem_t const * )( &(( pxList )->xListEnd ) ))
		
/*�ж������Ƿ�Ϊ��*/
#define listLIST_IS_EMPTY( pxList )\
	( ( BaseType_t )( ( pxList )->uxNumberOfItems == ( UBaseType_t )0 ))
	
/*��ȡ����Ľڵ���*/
#define listCURRENT_LIST_LENGTH( pxList )\
	(( pxList )->uxNumberOfItems )
	
/*��ȡ�����һ���ڵ��OWNER����TCB */
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB, pxList) {List_t * const pxConstList = (pxList); (pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext; if((void *) (pxConstList)->pxIndex == (void *) &((pxConstList)->xListEnd)) (pxConstList)->pxIndex = (pxConstList)->pxIndex->pxNext; (pxTCB) = (pxConstList)->pxIndex->pvOwner;}
#define listGET_OWNER_OF_HEAD_ENTRY( pxList )  ( (&( ( pxList )->xListEnd ))->pxNext->pvOwner )	
	
#endif
