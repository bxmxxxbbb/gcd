#ifndef __XMDelayExec_H__
#define __XMDelayExec_H__

#include "XMList.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct XMDelayExec_L XMDelayExec_l, *pXMDelayExec_l;

pXMDelayExec_l XMDelayExecInit();

void XMDelayExecDestroy(pXMDelayExec_l plXMDelayExec);

int XMDelayExecLoad(pXMDelayExec_l plXMDelayExec, void *Exec(void*), void *Prarm,void ParamFree(void *),const unsigned long Delay);

void XMDelayExecCancel(pXMDelayExec_l plXMDelayExec, unsigned long XMDelayExecID);

typedef struct XMDelayExec_F
{
    pXMDelayExec_l (*Init)();
    void (*Destroy)(pXMDelayExec_l plXMDelayExec);
    int (*Load)(pXMDelayExec_l plXMDelayExec, void * Exec(void*), void *Param, void ParamFree(void *), const unsigned long Delay);
    void (*Cancel)(pXMDelayExec_l plXMDelayExec, unsigned long XMDelayExecID);
}XMDelayExec_f, *pXMDelayExec_f;

pXMDelayExec_f XMDelayExecFunInit();
void XMDelayExecFunDestroy(pXMDelayExec_f pfXMDelayExec);

#ifdef __cplusplus
}
#endif

#endif