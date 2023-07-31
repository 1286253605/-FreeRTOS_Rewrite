#include "FreeRTOS.h"
#include <stdlib.h>
#include "list.h"


// 普通链表节点的初始化
void vListInitialiseItem( ListItem_t * const pxNewListItem ) {
    // 常量指针 不能改变指向
    // 节点所在的链表为空，没有插入任何节点。
    pxNewListItem->pvContainer = NULL;
}


// 链表根节点初始化
void vListInitialise( List_t * const pxList ) {
    // 让索引指向最后一个节点而不是NULL
    pxList->pxIndex = ( ListItem_t * ) &( pxList->xListEnd );

    // 保证升序排列时其在最后的位置
    pxList->xListEnd.xItemValue = portMAX_DELAY;

    // 没有节点，指向自己
    pxList->xListEnd.pxNext = (ListItem_t *) &( pxList->xListEnd );
    pxList->xListEnd.pxPrevious = (ListItem_t *) &( pxList->xListEnd );

    // 初始化链表节点数量为0
    pxList->uxNumberOfItems = (UBaseType_t) 0;

}

// 插入节点到空链表中，因为是空链表所以pxIndex才等于End最后一个节点
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem ) {
    ListItem_t * const pxIndex = pxList->pxIndex;


    pxNewListItem->pxNext = pxIndex;                    // 插入尾部当然下一个是最后一个节点
    pxNewListItem->pxPrevious = pxIndex->pxPrevious;
    pxIndex->pxPrevious = pxNewListItem;                // 链表中只有最后一个节点，插入尾部则这两个互为前后

    pxNewListItem->pvContainer = (void *) pxList;       // 新节点加入当前链表

    ( pxList->uxNumberOfItems )++;

}


// 项升序添加到链表中
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem ) {
    ListItem_t *pxIterator;

    // 获得节点排序辅助值
    const TickType_t xValueOfinsertion = pxNewListItem->xItemValue;

    // 寻找节点要加入的位置，使用for循环不断寻找下一个节点进行比较
    if ( xValueOfinsertion == portMAX_DELAY ) {
        // 如果插入的辅助值等于最后一个节点的值，则插入遍历节点等于根节点的前一个节点
        pxIterator = pxList->xListEnd.pxPrevious;
    }

    else {
        for ( pxIterator = ( ListItem_t * ) &pxList->xListEnd;              // 获得头部节点的位置(是双向链表)
                pxIterator->pxNext->xItemValue <= xValueOfinsertion;        // 如果指示器的下一个节点的值 大于 插入的值，则循环结束
                pxIterator = pxIterator->pxNext ) {                         // 如果指示器的下一个节点的值 小于 插入的值，则++（升序排列 小的在前）
                    // 什么也不做，只是为了获得插入节点的位置
                }
    }


    // 获得插入位置后进行插入，想象一下插入的步骤，先解决pxIterator的Next节点，再解决pxIterator
    pxNewListItem->pxNext = pxIterator->pxNext;
    pxIterator->pxNext->pxPrevious = pxNewListItem;

    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem;

    // 记录插入节点所在的链表
    pxNewListItem->pvContainer = ( void* ) pxList;

    // 节点计数值++
    ( pxList->uxNumberOfItems )++;
}


// 删除指定节点，并返回链表当前节点计数值
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove ) {
    // 获取节点所在的链表
    List_t * const pxList = ( List_t * ) pxItemToRemove->pvContainer;
    // 将节点移除，首先需要把被删除节点两头的节点连接起来，再将节点自身的连接删掉，但是官方代码并没有清除被删除节点的指向
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    // 一般要删除节点需要遍历，所以链表的 Iterator 可能指向这边
    if ( pxList->pxIndex == pxItemToRemove ) {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }

    // 计数器++
    ( pxList->uxNumberOfItems )++;   

    return pxList->uxNumberOfItems;

}

