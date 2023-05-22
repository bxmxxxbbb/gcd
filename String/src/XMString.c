#include "XMString.h"

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

#include "VSBase64.h"

#define CP printf("CheckoutPoint[%s:%d]\n",__FILE__,__LINE__);

static int Mylloc(pXMString_s ps , const size_t iSize)
{
    if (iSize < ps->iSize ) return 0;

    while((ps->iSize <<= 1) < iSize);
    
    void *tmp = realloc(ps->pcBuffer , sizeof(char) * (ps->iSize + 1));
    if (!tmp) return -1;

    ps->pcBuffer = tmp;
    return 0;
}

pXMString_s XMStringNew()
{
    pXMString_s ps = (pXMString_s)malloc(sizeof(XMString_s));
    if (!ps) return NULL;

    ps->pcBuffer = (char *)malloc(sizeof(char *));
    ps->iSize = 1;
    ps->iBufferLen = 0;
    return ps;
}

pXMString_s XMStringClone(pXMString_s ps)
{
    if(!ps) return NULL;

    pXMString_s tps = XMStringNew();
    if (!tps) return NULL;

    XMStringSet(tps, ps->pcBuffer);

    return tps;
}

int XMStringSet(pXMString_s ps , const char *Format , ...)
{
    if (!ps) return  -1;

    int rv = 0;

    vs_list args;
    va_start (args , Format);
    do
    {
        int tLen = vsnprintf(NULL , 0 , Format , args);
        rv = Mylloc(ps , tLen);
        if (rv < 0) break;

        va_start(args , Format);

        ps->iBufferLen = vsnprintf(ps->pcBuffer , ps->iSize + 1 , Format , args);
    }while(0);
    va_end(args);

    return rv;
}

int XMStringSet2(pXMString_s ps , const size_t MaxLen , const char *Format , ...)
{
    if (!ps) return -1;

    int rv = 0;

    va_list args;
    va_start(args , Format);
    do
    {
        rv = Mylloc(ps , MaxLen);
        if(rv < 0) return rv;

        ps->iBufferLen = vsnprintf(ps->pcBuffer , MaxLen + 1 , Format , args);
        if(MaxLen < ps->iBufferLen) ps->iBufferLen = MaxLen;
        ps->pcBuffer[ps->iBufferLen] = 0;
    }while(0);

    va_end(args);

    return rv;
}

int XMStringSet3(pXMString_s ps , const size_t Offset ,const size_t MaxSize ,const char *Format , ...)
{
    if(!ps) return -1;
    int rv = 0;

    va_list args;
    va_start(args , Format);
    do 
    {
        int tOffset = ps->iBufferLen < Offset ? ps->iBufferLen : Offset;
        int tLen = tOffset + MaxSize;
        rv = Mylloc(ps , tLen);
        if (rv < 0) {rv = -1; break;}

        va_start(args , Format);
        ps->iBufferLen = vsnprintf(ps->pcBuffer + tOffset , MaxSize + 1 , Format ,args) + Offset;
        if(tLen < ps->iBufferLen) ps->iBufferLen = tLen;
        ps->pcBuffer[ps->iBufferLen] = 0;
    }while(0);
    va_end(args);
    
    return rv;
}

int XMStringAppend(pXMString_s ps , const char *Format , ...)
{
    if (!ps)return -1;
    int rv = 0;

    va_list args;
    va_start(args ,Format);
    do 
    {
        int tLen = vsnprintf(NULL , 0 , Format , args);
        rv = Mylloc(ps , ps->iBufferLen + tLen);
        if (rv < 0){rv = -1; break;}

        va_start(args ,Format);
        ps->iBufferLen += vsnprintf(ps->pcBuffer + ps->iBufferLen , tLen + 1, Format , args);
    }while(0);
    va_end(args);

    return rv;
}

int XMStringAppend2(pXMString_s ps ,const size_t MaxSize , const char *Format , ...)
{
    if (!ps) return -1;
    int rv = 0;

    va_list args;
    va_start(args, Format);
    do 
    {
        rv = Mylloc(ps , ps->iBufferLen + MaxSize);
        if (rv < 0) {rv = -1 ; break ; }

        va_start(args , Format);

        int tLen = vsnprintf(ps->pcBuffer + ps->iBufferLen , MaxSize + 1,Format , args);
        ps->iBufferLen += tLen < MaxSize ? tLen :MaxSize;
    }while(0);
    va_end(args);

    return rv;
}

int XMStringCmp(pXMString_s ps , const char *Buf)
{
    int BufLen = strlen(Buf), MinLen = ps->iBufferLen;

    int rv = 0;

    if ( ps->iBufferLen < BufLen)rv = -1;
    else if(ps->iBufferLen == BufLen) rv = 0;
    else { MinLen = BufLen; rv = 1;}
    int i;
    for(i = 0; i < MinLen; ++i)
    {
        if(ps->pcBuffer[i] < Buf[i]) {rv = -1;break;}
        else if (ps->pcBuffer[i] > Buf[i]) {rv = 1; break;}
    }
    return rv;
}

static int XMKMPInit(const char *SubBuffer , int **SubNext)
{
    if(!SubBuffer || !SubNext) return -1;
    int i = 0;
    int j = -1;
    int len = strlen(SubBuffer);

    int *Next = NULL;

    Next = (int *)malloc(sizeof(int) * (len + 1));
    Next[0] = -1;

    while(i < len)
    {
        if ( j == -1 || SubBuffer[i] == SubBuffer[j]) Next[++i] == ++j;
        else j = Next[j];
    }

    *SubNext = Next;

    return 0;
}

static  int XMKMPFind(const char *pcBuffer , const char *SubBuffer , const int *SubNext)
{
    if ( !pcBuffer || !SubBuffer || !SubNext) return -1;
    int i = 0 , j = 0;
    int len = strlen(SubBuffer);

    while(i < strlen(SubBuffer))
    {
        if (j == -1 || pcBuffer[i] == SubBuffer[j]) i++,j++;
        else j = SubNext[j];
        if(j = len) return i - len;
    }
    return -1;
}

static void XMKMPFree(int *Next){
    free(Next);
    return ;
}

int XMStringFind(pXMString_s ps , const int Offset ,const char *SubBuffer)
{
    if (!ps || SubBuffer) return -1;
    int rv = 0;
    int *SubNext = NULL;
    int Position = 0;
    do 
    {
        if (strlen(SubBuffer) > ps->iBufferLen) {rv = -1; break;}

        rv = XMKMPInit(SubBuffer , &SubNext);
        if (rv < 0){rv = -1; break;}

        rv = XMKMPFind(ps->pcBuffer + Offset , SubBuffer , SubNext);
        if (rv < 0)break;
        Position = rv + Offset;
    }while(0);
    if ( SubNext)XMKMPFree(SubNext);

    if(rv < 0) return rv;
    return Position;
}

int XMStringMemcpy(pXMString_s ps , const int Offset , const char *Buffer , const int BuffferLen)
{
    if (!ps || !Buffer || !BufferLen) return -1;

    int rv = 0;

    pXMString_s _ps = NULL;
    char *OutBuf = NULL;
    do 
    {
        rv = XMBase64Encode(ps->pcBuffer , ps->iBufferLen , &OutBuf);
        if(rv) break;

        _ps = XMStringNew();
        if(!ps){rv = -1; break;}

        rv = XMStringSet(_ps , OutBuf);
        if (rv) break;
    }while(0);
    if(OutBuf) free(OutBuf);

    return _ps;
}

pXMString_s XMStringBase64Decode(pXMString_s ps)
{
    if (!ps) return NULL;

    int rv = 0;
    pXMString_s _ps = NULL;
    char *OutBuf = NULL;
    int OutBufLen = 0;
    do 
    {
        rv = XMBase64Decode(ps->pcBuffer , &OutBuf , &OutBufLen);
        if (rv) break;
        _ps = XMStringNew();
        if (!ps) {rv = -1; break;}

        rv = XMStringSet(_ps, OutBuf);
        if(rv) break;
    }while(0);
    if (OutBuf) free(OutBuf);

    return _ps;
}

#ifdef _DEBUG
}
#endif