#ifndef __XMLISTNODE_H__
#define __XMLISTNODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct XMListNode_S
{
    void *Data;
    struct XMListNode_S *Next ,*Last;
    int Count;
    int Alive;
}XMListNode_s,*pXMListNode_s;


typedef struct XMListNode_S XMListIterator_s, *pXMListIterator_s;

typedef struct XMListNodePool_L
{
    int Count;
    pXMListNode_s psNode;
}XMListNodePool_l,*pXMListNodePool_l;

static pthread_mutex_t XMNodePoolLocker = PTHREAD_MUTEX_INITIALIZER;

static inline pXMListNodePool_l XMNodePoolInit()
{
    pXMListNodePool_l plNodePool = (pXMListNodePool_l)malloc(sizeof(XMListNodePool_l));
    if (!plNodePool) return NULL;
    plNodePool->Count = 1;
    plNodePool->psNode = (pXMListNode_s)malloc(sizeof(XMListNode_s));
    plNodePool->psNode->Data = NULL;
    plNodePool->psNode->Next = NULL;
    plNodePool->psNode->Last = NULL;

    return plNodePool;
}

static inline pXMListNode_s XMNodePoolGet(pXMListNodePool_l plNodePool)
{
    if(!plNodePool) return NULL;
    pXMListNode_s ListNode = NULL;

    if(!plNodePool->psNode)
    {
        int i = 0;
        if (plNodePool->Count < 1 << 16)
        plNodePool->Count <<= 1;
        pthread_mutex_lock(&XMNodePoolLocker);
        while(i++ < plNodePool->Count)
        {
            ListNode = (pXMListNode_s)malloc(sizeof(XMListNode_s));
            ListNode->Alive = 1;
            ListNode->Count = 0;
            ListNode->Data = NULL;
            ListNode->Last = NULL;
            ListNode->Next = plNodePool->psNode;
            plNodePool->psNode = ListNode;
        }
        pthread_mutex_unlock(&XMNodePoolLocker);
    }
    pthread_mutex_lock(&XMNodePoolLocker);
    ListNode = plNodePool->psNode;
    ListNode->Alive = 1;
    ListNode->Count = 0;
    ListNode->Data = NULL;
    plNodePool->psNode = plNodePool->psNode->Next;
    pthread_mutex_unlock(&XMNodePoolLocker);

    return ListNode;
}

static inline void XMNodePoolReturn(pXMListNodePool_l plNodePool,pXMListNode_s psListNode)
{
    if(!plNodePool || !psListNode) return;
    pthread_mutex_lock(&XMNodePoolLocker);
    psListNode->Last = NULL;
    psListNode->Next = plNodePool->psNode;
    plNodePool->psNode = psListNode;
    pthread_mutex_unlock(&XMNodePoolLocker);
    return;
}

static inline void XMNodePoolDestroy(pXMListNodePool_l plNodePool)
{
    if(!plNodePool) return ;

    pXMListNode_s psNode = NULL;
    pthread_mutex_lock(&XMNodePoolLocker);
    while(plNodePool->psNode)
    {
        psNode = plNodePool->psNode;
        plNodePool->psNode = plNodePool->psNode->Next;
        free(psNode);
    }
    free(plNodePool);
    plNodePool = NULL;
    pthread_mutex_unlock(&XMNodePoolLocker);
    return;
}

#endif