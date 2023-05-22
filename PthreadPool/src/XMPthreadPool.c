#include "XMPthreadPool.h"
#include "XMQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define CP printf("CheckPoint:%d\n", __LINE__);

pXMQueue_l plXMQueue = NULL;
int QueueMaxCount = 0;
int QueueCount =0;

// pthread_mutex_t QueueLocker;
// #define QueueLock pthread_mutex_lock(&QueueLocker);
// #define QueueUnlock pthread_mutex_unlock(&QueueLocker);

pthread_spinlock_t QueueLocker;
#define QueueLock pthread_spin_lock(&QueueLocker);
#define QueueUnlock pthread_spin_unlock(&QueueLocker);

typedef struct  XMPthreadPool_Q
{
    pthread_mutex_t *Locker;
    pXMPthreadPool_s *Param;
}XMPthreadPool_q , *pXMPthreadPool_q;

void *Pthread(void *Param);

static void Free(pXMPthreadPool_q pq)
{
    if(!pq) return ;

    *pq->Param = NULL;
    pthread_mutex_unlock(pq->Locker);

    return;
}

static pXMPthreadPool_q New()
{
    pXMPthreadPool_q pq = (pXMPthreadPool_q)malloc(sizeof(XMPthreadPool_q));
    if(!pq) return NULL;

    pq->Param = (pXMPthreadPool_s *)malloc(sizeof(pXMPthreadPool_s));
    if(!pq->Param)
    {
        free(pq);
        return NULL;
    }

    pq->Locker = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(pq->Locker,NULL);

    pthread_t pid = 0;
    pthread_create(&pid, NULL, Pthread, pq);
    pthread_detach(pid);

    return pq;
}

static void Init(int Num)
{
    plXMQueue = XMQueueNew(NULL);
    if(!plXMQueue) return;
     
     pthread_spin_init(&QueueLocker,NULL);
    //  pthread_mutex_init(&QueueLocker, NULL);

    QueueMaxCount = Num;

    while (Num--) New();

    return ;    
}

static void Destroy()
{
    QueueLock
    pXMListIterator_s ps = NULL;

    while (ps = XMQueueFront(plXMQueue))
    {
        Free(ps->Data);
        XMQueuePop(plXMQueue);
    }
    QueueUnlock

    // pthread_mutex_destroy(&QueueLocker);
    pthread_spin_destroy(&QueueLocker);
    return ;
}

static pXMPthreadPool_q Get()
{
    pXMPthreadPool_q pq = NULL;
    QueueLock
    pXMListIterator_s ps = XMQueueFront(plXMQueue);
    if(!ps)
    {
        QueueUnlock
        New();
    }
    else 
    {
        pq = ps->Data;
        XMQueuePop(plXMQueue);
        QueueCount--;
        QueueUnlock
    }
    return pq;
}

static int Return(pXMPthreadPool_q pq)
{
    QueueLock
    if(QueueCount + 1 > QueueMaxCount)
    {
        QueueUnlock
        Free(pq);
        return -1;
    }
    XMQueuePush(plXMQueue, pq);
    QueueCount++;
    QueueUnlock
    
    return 0;
}

void *Pthread(void *Param)
{
    pXMPthreadPool_q pq = Param;
    if(!pq || !pq->Param) return NULL;
    pXMPthreadPool_s *ps = pq->Param;
    do
    {
        pthread_mutex_lock(pq->Locker);

        if(Return(pq)) break;

        pthread_mutex_lock(pq->Locker);

        if(!*ps) break;
        (*ps)->Function((*ps)->Data);
        free(*ps);
        pthread_mutex_unlock(pq->Locker);
    } while (1);

    free(*(pq->Param));
    free(pq->Param);
    pthread_mutex_destroy(pq->Locker);
    free(pq->Locker);

    free(pq);

    return NULL;
}

void XMPthreadPoolInit(int Num)
{
    return Init(Num);
}

void XMPthreadPoolRun(void Fun(void *Param), void *Data)
{
    pXMPthreadPool_q pq = NULL;

    while (!(pq = Get()));
    pXMPthreadPool_s Param = (pXMPthreadPool_s)malloc(sizeof(XMPthreadPool_s));
    Param->Function = Fun;
    Param->Data = Data;

    *pq->Param = Param;
    pthread_mutex_unlock(pq->Locker);

    return;
}

void XMPthreadPoolDestroy()
{
    Destroy();

    XMQueueDestroy(plXMQueue);

    return;
}

pXMPthreadPool_f XMPthreadPoolFunInit()
{
    pXMPthreadPool_f pf = (pXMPthreadPool_f)malloc(sizeof(XMPthreadPool_f));
    if(!pf) return NULL;
    pf->Init = XMPthreadPoolInit;
    pf->Run = XMPthreadPoolRun;
    pf->Destroy = XMPthreadPoolDestroy;
    return pf;
}

void XMPthreadPoolFunDestroy(pXMPthreadPool_f pfXMPthreadPool)
{
    if(!pfXMPthreadPool) return;
    free(pfXMPthreadPool);
    return;
}