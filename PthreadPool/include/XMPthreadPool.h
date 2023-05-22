#ifndef __XMPthreadPool_H__
#define __XMPthreadPool_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct XMPthreadPool_S
{
    void *Data;
    void (*Function)(void *);
} XMPthreadPool_s, *pXMPthreadPool_s;

void XMPthreadPoolInit(int Num);

void XMPthreadPoolRun(void Fun(void *Param), void *Data);

void XMPthreadPoolDestroy();

typedef struct  XMPthreadPool_F
{
    void (*Init)(int Num);
    void (*Run)(void Fun(void *Param), void *Data);
    void (*Destroy)();
}XMPthreadPool_f, *pXMPthreadPool_f;

pXMPthreadPool_f XMPthreadPoolFunInit();
void XMPthreadPoolFunDesroy(pXMPthreadPool_f pfXMPthreadPool);

#ifdef __cplusplus
}
#endif

#endif
