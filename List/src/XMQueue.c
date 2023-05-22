#include "XMQueue.h"

#define CP printf("%d:%d\n",__LINE__ , __FILE__);

pXMQueue_l XMQueueInit(void Free(void *))
{
    return XMListInit(Free);
}

void XMQueueDestroy(pXMQueue_l pl)
{
    return XMListDestroy(pl);
}

int XMQueuePush(pXMQueue_l pl , void *Data)
{
    return XMListPushBack(pl , Data);
}

pXMListIterator_s XMQueueFront(pXMQueue_l pl)
{
    pXMListIterator_s psIt = XMListEach(pl , NULL);
    XMListReoccupy(pl , psIt);
    return psIt;
}

void XMQueuePop(pXMQueue_l pl)
{
    if(!pl) return;

    pXMListIterator_s ps = XMListEach(pl , NULL);
    if(ps) XMListErase(pl , ps);

    return ;
}

pXMQueue_f XMQueueFunInit()
{
    pXMQueue_f pf = (pXMQueue_f)malloc(sizeof(XMQueue_f));
    if(!pf) return NULL;

    pf->Init = XMQueueInit;
    pf->Destroy = XMListDestroy;
    pf->Push = XMQueuePush;
    pf->Front = XMQueueFront;
    pf->Pop = XMQueuePop;

    return pf;
}

void XMQueueFunDestroy(pXMQueue_f pf)
{
    if(!pf) return ;
    free(pf);
    return;
}