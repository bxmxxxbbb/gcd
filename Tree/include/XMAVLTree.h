#ifndef __XMAVLTREE_H__
#define __XMAVLTREE_H__

#include "XMTreeNode.h"

typedef struct  XMTree_L XMAVLTree_l , *pXMAVLTree_l;

int XMAVLTreePush(pXMAVLTree_l pl, void *Data);

pXMTreeIterator_s XMAVLTreeFind(pXMAVLTree_l pl, void *Data);

pXMTreeIterator_s XMAVLTreeErase(pXMAVLTree_l pl , pXMTreeIterator_s ps);

void XMAVLTreeReoccupy(pXMAVLTree_l pl , pXMTreeIterator_s ps);

typedef struct XMAVLTree_F
{
    pXMAVLTree_l (*New)(int Compare(void *DataA ,void *DataB), void Free(void *Data));

    void (*Destroy)(pXMAVLTree_l pl);

    pXMTreeIterator_s (*Each)(pXMTree_l pl,pXMTreeIterator_s ps);

    pXMTreeIterator_s (*ReverseEach)(pXMTree_l pl, pXMTreeIterator_s ps);

    int (*Push)(pXMAVLTree_l pl,void *Data);

    pXMTreeIterator_s (*Find)(pXMAVLTree_l pl , void *Data);

    pXMTreeIterator_s (*Erase)(pXMAVLTree_l pl, pXMTreeIterator_s ps);

    void (*Reoccupy)(pXMAVLTree_l pl , pXMTreeIterator_s ps);
}XMAVLTree_f, *pXMAVLTree_f;

pXMAVLTree_f XMAVLTreeFunInit();

void XMAVLTreeFumDestroy(pXMAVLTree_f);

#endif