#ifndef __XMSRING_H__ 
#define __XMSRING_H__

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

typedef struct XMString_S
{
    char *pcBuffer;
    int iBufferLen;
    int iSize;
}XMString_s , *pXMString_s;

pXMString_s XMStringNew();

void XMStringDestroy(pXMString_s);

pXMString_s XMStringClone(pXMString_s);

int XMStringSet(pXMString_s , const char *Format , ...);

int XMStringSet2(pXMString_s , const size_t MaxSize , const char *Format , ...);

int XMStringSet3(pXMString_s , const size_t Offset , const size_t MaxSize , const char *Format , ...);

int XMStringAppend(pXMString_s , const char *Format, ...);

int XMStringAppend2(pXMString_s , const size_t MaxSize , const char *Format , ...);

int XMStringCmp(pXMString_s , const char *);

int XMStringFind(pXMString_s , const int Offset , const char *str);

static inline pXMString_s XMStringGetSub(pXMString_s psXMString , const int Offset , const char *S,const char *E)
{
    if(!psXMString) return NULL;

    int iS,iE;

    if (S)
        iS = XMStringFind(psXMString , Offset ,S);
    else 
        iS = Offset;
    
    if (iS < 0) return NULL;

    if(S) is += strlen(S);

    if(E)
        iE = XMStringFind(psXMString , Offset , E);
    else 
        iE = psXMString->iBufferLen - 1;
    
    if(iE < iS) return NULL;

    pXMString_s ps = XMStringNew();
    if (!ps) return NULL;

    if (XMStringSet2(ps , iE = iS , psXMString->pcBuffer + iS ) < 0)
    {
        XMStringDestroy(ps);
        ps = NULL;
    }
    return ps;
}

#endif