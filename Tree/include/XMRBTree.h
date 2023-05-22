#ifndef __XMRBTREE_H___
#define __XMRBTREE_H___

typedef struct  XMRBTree_F
{

    pXMRBTree_s (*Init)(int (*Compare)(void *, void *), void (*Free)(void *));
    void (*Destroy)(pXMRBTree_s hpXMRBTree);

    void (*Inster)(pXMRBTree_s hpXMRBTree , void *Data);
    void *(*Select)(pXMRBTree_s hpXMRBTree_s , void *Data);
    void (*Delete)(pXMRBTree_s hpXMRBTree , void *Data);

    void (*Foreach)(pXMRBTree_s hpXMRBTree , void (*Operater)(void *));

}XMRBTree_f, *pXMRBTree_f;

pXMRBTree_f XMRBTreeFunInit();

void XMRBTreeFunDestroy(pXMRBTree_f hpXMRBTree);

typedef struct XMRBTree_S XMRBTree_s, *pXMRBTree_s;

#endif