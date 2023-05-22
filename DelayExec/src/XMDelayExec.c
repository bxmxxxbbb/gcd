#include "XMDelayExec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define CP printf("%d:%s\n",__LINE__,__FILE__);
typedef struct XMDelayExec_S
{
    unsigned long ID;
    pthread_mutex_t Locker;
    void * (*Exec)(void *);
    void *Param;
    void (*ParamFree)(void *);
    short sign;
    unsigned long Delay;
}XMDelayExec_s, *pXMDelayExec_s;

struct XMDelayExec_L
{
    pthread_mutex_t Locker;
    pXMList_l plList;
    short alive;
};

static unsigned long ID = 0;

pXMDelayExec_s Init(void * Exec(void *), void *Param, void ParamFree(void *), const unsigned long Delay)
{
    pXMDelayExec_s ps = (pXMDelayExec_s)malloc(sizeof(XMDelayExec_s));
    if(!ps) return NULL;

    ps->ID = ++ID;
    if(ID == __LONG_MAX__ - 1) ID = 0;

    ps->Exec = Exec;
    ps->Param = Param;
    ps->ParamFree = ParamFree;

    pthread_mutex_init(&ps->Locker, NULL);
    ps->sign = 1;
    ps->Delay = time(NULL) + Delay;

    return ps;
}

static void Destroy(pXMDelayExec_s ps)
{
    if(!ps) return ;

    if(ps->ParamFree) ps->ParamFree(ps->Param);

    pthread_mutex_destroy(&ps->Locker);

    free(ps);

    return;
}

void *DelayExec(void *Param)
{
    pXMDelayExec_s psDelayExec = Param;

    pthread_mutex_lock(&psDelayExec->Locker);

    if(psDelayExec->sign)
        psDelayExec->Exec(psDelayExec->Param);

    Destroy(psDelayExec);

    return NULL;
}

void *Each(void *Param)
{
    pXMDelayExec_l pl = Param;

    unsigned long Time = time(NULL);
    pXMListIterator_s psIt = NULL;
    pXMDelayExec_s ps = NULL;

    while(1)
    {
        sleep(1);
        psIt = NULL;
        Time = time(NULL);
        pthread_mutex_lock(&pl->Locker);

        while((psIt = XMListEach(pl->plList, psIt)))
        {
            ps = psIt->Data;
            if(Time >= ps->Delay)
            {
                pthread_mutex_unlock(&ps->Locker);
                psIt = XMListErase(pl->plList, psIt);
            }
        }
        pthread_mutex_unlock(&pl->Locker);

        if(!pl->alive) break;
    }

    pthread_mutex_destroy(&pl->Locker);
    XMListDestroy(pl->plList);

    free(pl);

    return NULL;
}

pXMDelayExec_l XMDelayExecInit()
{
    pXMDelayExec_l pl = (pXMDelayExec_l)malloc(sizeof(XMDelayExec_l));
    if(!pl) return NULL;

    pl->plList = XMListInit(NULL);
    pl->alive = 1;
    pthread_mutex_init(&pl->Locker, NULL);
    
    pthread_t pid;
    pthread_create(&pid, NULL,Each, pl);
    pthread_detach(pid);

    return pl;
}

void XMDelayExecDestroy(pXMDelayExec_l plXMDelayExec)
{
    if(!plXMDelayExec) return;

    plXMDelayExec->alive = 0;

    return;
}

int XMDelayExecLoad(pXMDelayExec_l plXMDelayExec, void * Exec(void *), void *Param, void ParamFree(void *Param), const unsigned long Delay)
{
    if(!plXMDelayExec || !Exec) return -1;

    int rv = 0;

    pXMDelayExec_s ps = NULL;
    do
    {
        ps = Init(Exec, Param, ParamFree, Delay);
        if(!ps) {rv = -1; break;}

        pthread_mutex_lock(&ps->Locker);

        pthread_mutex_lock(&plXMDelayExec->Locker);
        rv = XMListPushBack(plXMDelayExec->plList, ps);
        pthread_mutex_unlock(&plXMDelayExec->Locker);

        if(rv) break;

        pthread_t pid;
        pthread_create(&pid, NULL, DelayExec, ps);
        pthread_detach(pid);

        rv = ps->ID;
    }while(0);
    if(rv < 0)
    {
        if(ps) Destroy(ps);
    }

    return rv;
}

void XMDelayExecCancel(pXMDelayExec_l plXMDelayExec, unsigned long XMDelayExecID)
{
    if(!plXMDelayExec) return;

    pXMListIterator_s psIt = NULL;
    pXMDelayExec_s ps = NULL;

    pthread_mutex_lock(&plXMDelayExec->Locker);
    while((psIt = XMListEach(plXMDelayExec->plList, psIt)))
    {
        ps = psIt->Data;
        if(ps->ID == XMDelayExecID)
        {
            ps->sign = 0;
            pthread_mutex_unlock(&ps->Locker);
            psIt = XMListErase(plXMDelayExec->plList,psIt);
            XMListBreakEX(plXMDelayExec->plList, psIt);
        }
    }
    pthread_mutex_unlock(&plXMDelayExec->Locker);

    return;
}

pXMDelayExec_f XMDelayExecFunInit()
{
    pXMDelayExec_f pf = (pXMDelayExec_f)malloc(sizeof(XMDelayExec_f));
    if(!pf) return NULL;
    pf->Init = XMDelayExecInit;
    pf->Destroy = XMDelayExecDestroy;
    pf->Load = XMDelayExecLoad;
    pf->Cancel = XMDelayExecCancel;
    return pf;
}

void XMDelayExecFunDestroy(pXMDelayExec_f pfXMDelayExec)
{
    if(!pfXMDelayExec) return ;
    free(pfXMDelayExec);
    return;
}