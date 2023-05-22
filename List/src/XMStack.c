#include "XMStack.h"

pXMStack_l XMStackInit(void Free(void *))
{
    return XMListInit(Free);
}

void XMStackDestory(pXMStack_l pl)
{
    return XMListDestory(pl);
}

int XMStackPush(pXMStack_l pl , void *Data)
{
    return XMListPushFront(pl , Data);
}

pXMListIterator_s XMStackTop(pXMStack_l pl)
{
    pXMListIterator_s psIt = XMListEach(pl, NULL);
    XMListReoccupy(pl , psIt);
    return psIt;
}

void XMStackPop(pXMStack_l pl)
{
    if(!pl) return ;
    pXMListIterator_s ps = XMListEach(pl , NULL);
    if(ps) XMListErase(pl , ps);

    return ;
}

pXMStack_f XMStackFunInit()
{
    pXMStack_f pf = (pXMStack_f)malloc(sizeof(XMStack_f));
    if (!pf) return NULL;

    pf->Init = XMStackInit;
    pf->Destroy = XMStackDestroy;
    pf->Push = XMStackPush;
    pf->Top = XMStackTop;
    pf->Pop = XMStackPop;

    return pf;
}

void XMStackFunDestroy(pXMStack_f pf)
{
    if(!pf) return ;
    free(pf);
    return ;
}