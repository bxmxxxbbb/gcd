#ifndef __XMQUEUE_H__
#define __XMQUEUE_H__

#include "./XMList.h"

typedef struct XMList_L XMQueue_l , * pXMQueue_l;

pXMQueue_l XMQueueInit(void Free(void *));

void XMQueueDestroy(pXMQueue_l); 

int XMQueuePush(pXMQueue_l plXMQueue , void *Data);

pXMListIterator_s XMQueueFront(pXMQueue_l);

void XMQueuePop(pXMQueue_l);

typedef struct XMQueue_F
{
    pXMQueue_l (*Init)(void Free(void *));
    
    void (*Destroy)(pXMQueue_l);

    int (*Push)(pXMQueue_l pl , void *Data);

    pXMListIterator_s (*Front)(pXMQueue_l);

    void (*Pop)(pXMQueue_l);
}XMQueue_f , *pXMQueue_f;

pXMQueue_f XMQueueFunInit();

void XMQueueFunDestroy(pXMQueue_f pf);

#endif