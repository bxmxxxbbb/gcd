#ifndef __XMStack_H__
#define __XMStack_H__

#include "./XMList.h"

typedef struct XMList_L XMStack_l , *pXMStack_l;

pXMStack_l XMStackInit(void Free(void *));

void XMStackDestroy(pXMStack_l);

int XMStackPush(pXMStack_l plXMStack , void *Data);

pXMListIterator_s XMStackTop(pXMStack_l);

void XMStackPop(pXMStack_l);

typedef struct XMStack_F
{
    pXMStack_l (*Init)(void Free(void *));

    void (*Destroy)(pXMStack_l);
    
    int (*Push)(pXMStack_l pl , void *Data);

    pXMListIterator_s (*Top)(pXMStack_l);

    void (*Pop)(pXMStack_l);
}XMStack_f , *pXMStack_f;

pXMStack_f XMStackFunInit();

void XMStackFunDestory(pXMStack_f pf);

#endif