#ifndef LIST_H
#define LIST_H
/* -----------头文件包含----------- */
#include "FreeRTOS.h"

/* -----------结构体定义----------- */

/* 普通节点结构体定义 */
struct  xLIST_ITEM 
{
    TickType_t xItemValue;              // 辅助值，用于节点排列
    struct xLIST_ITEM * pxNext;         // 指向下一个节点
    struct xLIST_ITEM * pxPrevious;     // 指向前一个节点
    void * pvOwner;                     // 指向节点内核对象，通常是TCB
    void * pvContainer;                 // 指向节点所在的列表
};

typedef struct xLIST_ITEM ListItem_t;   //类型重定义





// 精简链表节点结构体
struct xMINI_LIST_ITEM 
{
    TickType_t xItemValue;              //辅助值，用于节点升序排列
    struct xLIST_ITEM * pxNext;
    struct xLIST_ITEM * pxPrevious;
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;



// 链表根节点定义
typedef struct xList
{
    UBaseType_t uxNumberOfItems;        // 除去根节点 链表节点个数 
    ListItem_t *pxIndex;                // 链表节点索引指针 用于遍历整个链表
    // 双向链表头尾相连，最后一个节点也就是第一个节点。
    // 最后一个节点是一个实体，不是一个指针！
    MiniListItem_t xListEnd;
} List_t;


/* -----------宏函数----------- */
// #define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner )\
//         ( ( pxListem )->pvOwner = ( void * ) (pxOwner) )         // 韦东山把pxListItem打错了
#define listSET_LIST_ITEM_OWNER( pxListItem, pxOwner )		( ( pxListItem )->pvOwner = ( void * ) ( pxOwner ) )


/* 获取节点拥有者 */
#define listGET_LIST_ITEM_OWNER( pxListItem )\
        ( ( pxListItem )->pvOwner )

/* 初始化节点排序辅助值 */
#define listSET_LIST_ITEM_VALUE( pxListItem, xValue )\
        ( ( pxListItem )->xItemValue = ( xValue ) )

/* 获取节点排序辅助值 */
#define listGET_LIST_ITEM_VALUE( pxListItem )\
        ( ( pxListItem )->xItemValue )

/* 获取链表根节点的节点计数器的值 */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY( pxList )\
        ( ( ( pxList )->xListEnd ).pxNext->xItemValue )

/* 获取链表的入口节点 */
#define listGET_HEAD_ENTRY( pxList )\
        ( ( ( pxList )->xListEnd ).pxNext )

/* 获取节点的下一个节点 */
#define listGET_NEXT( pxListItem )\
        ( ( pxListItem )->pxNext )

/* 获取链表的最后一个节点 */
#define listGET_END_MARKER( pxList )\
        ( ( ListItem_t const * ) ( &( ( pxList )->xListEnd ) ) )

/* 判断链表是否为空 */
#define listLIST_IS_EMPTY( pxList )\
        ( ( BaseType_t ) ( ( pxList )->uxNumberOfItems == ( UBaseType_t ) 0 ) )

/* 获取链表的节点数 */
#define listCURRENT_LIST_LENGTH( pxList )\
        ( ( pxList )->uxNumberOfItems )

/* 获取链表第一个节点的OWNER，即TCB */
#define listGET_OWNER_OF_NEXT_ENTRY( pxTCB, pxList )                                        \
{                                                                                           \
    List_t * const pxConstList = ( pxList );                                                \
    /* 节点索引指向链表第一个节点 */                                                            \
    ( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;                            \
    /* 判断链表迭代器是否指向了End节点，如果是，则指向下一个节点 */                                  \
    if( ( void * ) ( pxConstList )->pxIndex == ( void * ) &( ( pxConstList )->xListEnd ) )  \
    {                                                                                       \
        ( pxConstList )->pxIndex = ( pxConstList )->pxIndex->pxNext;                        \
    }                                                                                       \
    /* 获取节点的OWNER，即TCB */                                                              \
    ( pxTCB ) = ( pxConstList )->pxIndex->pvOwner;                                          \
}


/* -----------函数声明----------- */

void vListInitialiseItem( ListItem_t * const pxNewListItem );
void vListInitialise( List_t * const pxList );
void xListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem );
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem );
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove );


#endif

