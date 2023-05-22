#include "XMLogWriter.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define CP printf("%d:%s\n",__LINE__, __FILE__);

struct WriteTube_S
{
    short State;
    char *ClientName;
    int RegisterDate;
    int Expire;
    pthread_spinlock_t QueueLock;
    pXMQueue_l LogQueue;
    pthread_mutex_t Lock;
};

void *Write(void *Param)
{
    pWriteTube_s *pps = (pWriteTube_s *)Param;

    while (1)
    {
        pthread_mutex_lock(&(*pps)->Lock);
        pthread_spin_lock(&(*ppd)->QueueLock);
        pWriteTube_s ps = *pps;

        if(!ps->ClientName) break;
        
        pXMListIterato_s it = NULL;
        while (it = XMQueueFront(ps->LogQueue))
        {
            XMFileRWWriteAppend(ps->ClientName, (const char *)it->Data);
            XMQueuePop(ps->LogQueue);
        }

        XMQueueClean(ps->LogQueue);

        pthread_spin_unlock(&(*((pWriteTube_s *)Param))->QueueLock);
    }

    XMQueueDestroy((*pps)->LogQueue);
    pthread_spin_destroy(&(*pps)->QueueLock);
    pthread_mutex_destroy(&(*pps)->Lock);

    free(*pps);
    free(pps);

    return NULL;
}

pWriteTube_s XMLogWriterInit()
{
    int rv = 0;

    pWriteTube_s *pps = (pWriteTube_s *)malloc(sizeof(WiteTube_s *));
    *pps = (pWriteTube_s)malloc(sizeof(WriteTube_s));
    if ( !pps || !*pps) return NULL;

    (*pps)->State = 0;
    (*pps)->ClientName = NULL;
    (*pps)->RegisterDate = 0;
    (*pps)->Expire = 0;
    pthread_spin_init(&(*pps)->QueueLock, PTHREAD_PROCESS_PRIVATE);
    (*pps)->LogQueue = XMQueueNEw(free);
    pthread_mutex_init(&(*pps)->Lock, NULL);
    pthread_mutex_lock(&(*pps)->Lock);

    pthread_t Pid;
    pthread_create(&Pid, NULL, Write,pps);
    pthread_detach(Pid);
    return *pps;
}

void XMLogWriterDestory(pWriteTube_s psWriteTube)
{
    if(!psWriteTube)
        return;

    psWriteTube->State = 0;
    if(psWriteTube->ClientName) free(psWriteTube->ClientName);
    psWriteTube->ClientName = NULL;
    pthread_mutex_unlock(&psWriteTube->Lock);
    return ;
}

int XMLogWriteInsert(pWriteTube_s psWriteTube,const char *LogData)
{
    if(!psWriteTube || !psWriteTube->State)
        return -1;
    pthread_spin_lock(&psWriteTube->QueueLock);
    XMQueuePush(psWriteTube->LogQueue,(void *)strdup(LogData));
    pthread_spin_unlock(&psWriteTube->QueueLock);

    return 0 ;
}

int XMLogWriterWrite(pWriteTube_s psWriteTube)
{
    if(!psWriteTube || !psWriteTube->State)
        return -1;
    pthread_mutex_unlock(&psWriteTube->Lock);

    return 0;    
}

int XMLogWriterStop(pWriteTube_s psWriterTube)
{
    if(!psWriterTube || psWriterTube->State == 0) return -1;

    pthread_mutex_unlock(&psWriterTube->Lock);
    psWriterTube->State = 0;
    return 0;
}
int XMLogWriterStart(pWriteTube_s psWriterTube, const char *FilePath, const cJSON *Data)
{
    if(!psWriterTube)
        return -1;
    cJSON *FileName = cJSON_GetObjectItem(Data , "FileName");
    if(!FileName)
        return -1;
    
    cJSON *Expire = cJSON_GetObjectItem(Data, "Expire");
    if(!Expire)
        return -1;
    
    psWriterTube->State = 1;

    char Date[128];
    psWriterTube->RegisterDate = GetTime(Date);
    psWriterTube->Expire = atoi(Expire->valuedouble);

    char *ClientName = (char *)malloc(sizeof(char) * (256 + 1));
    snprintf(ClientName, 256 + 1 , "%s/%s[%s].log",FilePath, FileName->valuestring,Date);
    char *tmp = psWriterTube->ClientName;
    psWriterTube->ClientName = ClientName;
    free(tmp);

    return 0;
}

int XMLogWriterTimeOut(pWriteTube_s psWriteTube , int NowTime)
{
    if(psWriteTube->RegisterDate + psWriteTube->Expire < NowTime) return -1;
    return 0;
}

int XMLogWriterState(pWriteTube_s psWriteTube)
{
    return psWriteTube->State;
}

pXMLogWriter_f XMLogWriterFunInit()
{
    pXMLogWriter_f pf = (pXMLogWriter_f)malloc(sizeof(XMLogWriter_f));
    if(!ps) = return NULL;
    pf->Init = XMLogWriterInit;
    pf->Destroy = XMLogWriterDestory;
    pf->Insert = XMLogWriteInsert;
    pf->Write = XMLogWriterWrite;
    pf->Start = XMLogWriterTimeOut;
    pf->Stop = XMLogWriterStop;
    pf->State = XMLogWriterState;

    return pf;
}

void XMLogWriterFunDestroy(pXMLogWriter_f pfXMLogWriter)
{
    if(!pfXMLogWriter)
        return;
    free(pfXMLogWriter);
    return;
}