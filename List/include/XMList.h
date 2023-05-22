#ifndef __XMLIST_H__
#define __XMLIST_H__

#include "XMListNode.h"

typedef struct XMList_L XMList_l,*pXMList_l;

pXMList_l XMListInit(void Free(void*));

void XMListDestroy(pXMList_l plList);

int XMListPushBack(pXMList_l plList , void *Data);

int XMListPushFront(pXMList_l plList , void *Data);

#define XMListEachEX(plList , psIt) while((psIt) = XMListEach((plList),(psIt)))
pXMListIterator_s XMListEach(pXMList_l plList , pXMListIterator_s psIterator);

pXMListIterator_s XMListReverseEach(pXMList_l plList , pXMListIterator_s psIterator);

pXMListIterator_s XMListErase(pXMList_l plList , pXMListIterator_s psIterator);

#define XMListBreakEX(plList , psIterator) { XMListReoccupy(plList , psIterator); break;}
void XMListReoccupy(pXMList_l plList , pXMListIterator_s psIterator);

void XMListClean(pXMList_l plList);

typedef struct XMList_F
{
    pXMList_l (*Init)(void Free(void *));
    void (*Destroy)(pXMList_l plList);
    int (*PushBack)(pXMList_l plList , void *Data);
    int (*PushFront)(pXMList_l plList , void *Data);
    pXMListIterator_s (*Each)(pXMList_l plList , pXMListIterator_s psIterator);
    pXMListIterator_s (*ReverseEach)(pXMList_l plList , pXMListIterator_s psIterator);
    pXMListIterator_s (*Erase)(pXMList_l plList , pXMListIterator_s psIterator);
    void (*Clean)(pXMList_l plList);
}XMList_f , *pXMList_f;


pXMList_f XMListFunInit();

void XMListDestroy(pXMList_f pf);

#endif