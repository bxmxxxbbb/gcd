#include "XMList.h"
#include "XMListNode.h"

#include <unistd.h>

#define CP printf("%d:%s\n",__LINE__,__FILE__);

struct XMList_L
{
    pXMListNodePool_l plPool;
    pXMListNode_s psNode;
    void (*Free)(void *);
    int Alive;
    int Count;
    pthread_mutex_t Locker;
};

static void Erase(pXMList_l plList , pXMListNode_s ps)
{
    if(ps->Alive || ps->Count) return ;

    ps->Next->Last = ps->Last;
    ps->Last->Next = ps->Next;

    if (plList->Free && ps->Data)plList->Free(ps->Data);
    ps->Data = NULL;

    XMNodePoolReturn(plList->plPool , ps);

    return ;
}

// static void * Clean(void *Param)
// {
//     pXMList_l plList = Param;
//     pXMListIterator_s pIt = plList->psNode;

//     int cnt = 0;

//     pXMListIterator_s pTmp = NULL;

//     while(1)
//     {
//         sleep(plList->CleanInterval);
//         cnt = 100000;
//         pthread_rwlock_wrlock(&plList->RWLocker);
//         if(!plList->Alive) break;
//         pIt = pIt->Next;
//         while(cnt--)
//         {
//             if (pIt == plList->psNode) break;
//             if (!pIt->Data && !pIt->Count)
//             {
//                 pTmp = pIt;
//                 pIt = pIt->Next;
//                 Erase(plList->plPool , pTmp);
//             }
//             else 
//             {
//                 pIt = pIt->Next;
//             }
//         }
//         pthread_rwlock_unlock(&plList->RWLocker);
//     }

//     pXMListNode_s psNode = NULL;

//     if(plList->Free)
//         while(plList->psNode->Next != plList->psNode)
//         {
//             psNode = plList->psNode->Next;
//             plList->psNode->Next = plList->psNode->Next->Next;
//             plList->Free(psNode->Data);
//             XMNodePoolReturn(plList->plPool , psNode);
//         }
//     else 
//         while(plList->psNode->Next !- plList->psNode)
//         {
//             psNode = plList->psNode->Next;
//             plList->psNode->Next = plList->psNode->Next->Next;
//             XMNodePoolReturn(plList->plPool , psNode);
//         }
    
//     XMNodePoolReturn(plList->plPool , plList->psNode);
//     XMNodePoolDestroy(plList->plPool);
//     pthread_rwlock_unlock(&plList->RWLocker);

//     pthread_rwlock_destroy(&plList->RWLocker);
//     free(plList);
//     return NULL;
// }

pXMList_l XMListInit(void Free(void *))
{
    pXMList_l plList = (pXMList_l)malloc(sizeof(XMList_l));
    if (!plList) return NULL;

    plList->Count = 0;
    plList->Alive = 1;

    plList->plPool = XMNodePoolInit();
    plList->psNode = XMNodePoolGet(plList->plPool);
    plList->psNode->Last = plList->psNode;
    plList->psNode->Next = plList->psNode;

    plList->Free = Free;

    pthread_mutex_init(&plList->Locker , NULL);

    // if(!CleanInterval) return plList;

    // pthread_t pid;
    // pthread_create(&pid , NULL , Clean , plList);
    // pthread_detach(pid);

    return plList;
}

void XMListDestroy(pXMList_l plList)
{
    if (!plList) return;

    // if(!plList->CleanInterval)
    // {
            pXMListNode_s psNode = NULL;

            if(plList->Free)
                while(plList->psNode->Next != plList->psNode)
                {
                    psNode = plList->psNode->Next;
                    plList->psNode->Next = plList->psNode->Next->Next;
                    plList->Free(psNode->Data);
                    XMNodePoolReturn(plList->plPool , psNode);
                }
            else 
                while(plList->psNode->Next != plList->psNode)
                {
                    psNode = plList->psNode->Next;
                    plList->psNode->Next = plList->psNode->Next->Count;
                    XMNodePoolReturn(plList->plPool, psNode);
                }
            XMNodePoolReturn(plList->plPool , plList->psNode);
            XMNodePoolDestroy(plList->plPool);

            pthread_mutex_destroy(&plList->Locker);
            free(plList);
    // }
    // else 
    // {
    //     plList->Alive = 0;
    // }
    return;
}

int XMListPushBack(pXMList_l plList , void *Data)
{
    if (!plList) return -1;
    pXMListNode_s psNode = XMNodePoolGet(plList->plPool);

    if(!psNode) return -1;

    pthread_mutex_lock(&plList->Locker);

    psNode->Data = Data;
    psNode->Last = plList->psNode->Last;
    psNode->Next = plList->psNode;

    plList->psNode->Last->Next = psNode;
    plList->psNode->Last = psNode;
    plList->Count++;

    pthread_mutex_unlock(&plList->Locker);

    return 0;
}

int XMListPushFront(pXMList_l plList , void * Data)
{
    if(!plList) return -1;

    pXMListNode_s psNode = XMNodePoolGet(plList->plPool);
    if(!psNode) return -1;

    pthread_mutex_lock(&plList->Locker);
    psNode->Data = Data;
    psNode->Next = plList->psNode->Next;
    psNode->Last = plList->psNode;

    plList->psNode->Next->Last = psNode;
    plList->psNode->Next = psNode;
    plList->Count++;
    pthread_mutex_unlock(&plList->Locker);

    return 0;
}

pXMListIterator_s XMListEach(pXMList_l plList , pXMListIterator_s psIterator)
{
    if (!plList) return NULL;

    pXMListIterator_s psNode = NULL;

    pthread_mutex_lock(&plList->Locker);

    if(!psIterator)
        psNode = plList->psNode->Next;
    else
    {
        psNode = psIterator->Next;
        psIterator->Count--;
        Erase(plList,psIterator);
    }

    while(!psNode->Alive && psNode != plList->psNode) psNode = psNode->Next;

    if(psNode == plList->psNode) psNode = NULL;
    else psNode->Count++;

    pthread_mutex_unlock(&plList->Locker);

    return psNode;
}

pXMListIterator_s XMListReverseEach(pXMList_l plList , pXMListIterator_s psIterator)
{
    if (!plList) return NULL;

    pXMListIterator_s psNode = NULL;

    pthread_mutex_lock(&plList->Locker);

    if(!psIterator)
        psNode=plList->psNode->Last;
    else
    {
        psNode = psIterator->Last;
        psIterator->Count--;
        Erase(plList , psIterator);
    }

    while(!psNode->Alive && psNode != plList->psNode) psNode = psNode->Last;

    if(psNode == plList->psNode) psNode = NULL;
    else psNode->Count++;

    pthread_mutex_unlock(&plList->Locker);

    return psNode;
}

pXMListIterator_s XMListErase(pXMList_l plList , pXMListIterator_s psIterator)
{
    if(!plList || !psIterator) return NULL;

    pthread_mutex_lock(&plList->Locker);
    pXMListIterator_s ps = NULL;

    if (psIterator->Alive)
    {
        psIterator->Alive = 0;

        ps = psIterator->Last;
        while(!ps->Alive) ps = ps->Last;

        if(ps == plList->psNode) ps = NULL;
        else ps->Count++;

        psIterator->Count--;
        Erase(plList , psIterator);

        plList->Count--;
    }
    pthread_mutex_unlock(&plList->Locker);

    return ps;
}

void XMListReoccupy(pXMList_l plList , pXMListIterator_s psIterator)
{
    if(!psIterator) return ;
    psIterator->Count--;
    Erase(plList , psIterator);
    return ; 
}

void XMListClean(pXMList_l plList)
{
    if(!plList) return ;

    pXMListNode_s psNode = plList->psNode->Next;
    pXMListNode_s psNodeTmp = NULL;

    pthread_mutex_lock(&plList->Locker);
    while(psNode != plList->psNode)
    {
        if(!psNode->Alive && !psNode->Count)
        {
            psNode->Last->Next = psNode->Next;
            psNode->Next->Last = psNode->Last;
            psNodeTmp = psNode;
            psNode = psNodeTmp->Next;
            XMNodePoolReturn(plList->plPool , psNodeTmp);
        }
        else 
        {
            psNode = psNode->Next;
        }
    }
    pthread_mutex_unlock(&plList->Locker);

    return;
}

pXMList_f XMListFunInit()
{
    pXMList_f pf = (pXMList_f)malloc(sizeof(XMList_f));
    if(!pf) return NULL;
    
    pf->Init = XMListInit;
    pf->Destroy = XMListDestroy;
    pf->PushBack = XMListPushBack;
    pf->PushFront = XMListPushFront;
    pf->Each = XMListEach;
    pf->ReverseEach = XMListReverseEach;
    pf->Erase = XMListErase;
    pf->Clean = XMListClean;

    return pf;
}

void XMListFunDestroy(pXMList_f pf)
{
    if(!pf) return;
    free(pf);

    return ;
}