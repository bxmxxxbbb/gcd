#ifndef __XMLogWriter_H__
#define __XMLogWriter_H__

#include <pthread.h>
#include "XMQueue.h"
#include "XMFileRW.h"
#include <cJSON.h>


#ifdef __cplusplus
extern "C"
{
#endif
static inline long XMGetTime(char *Date)
{
    struct tm *tm_ptr = NULL;
    time_t tDate = 0;
    time(&tDate);
    if(Date)
    {
        tm_ptr = localtime(&tDate);
        sprintf(Date, "%04d-%02d-%02dT%02d:%02d:%02d",1900 + tm_ptr->tm_year, 1 + tm_ptr->tm_mon, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
    }
    return (long)tDate;
}

typedef struct WriteTube_S WriteTube_s, *pWriteTube_s;

pWriteTube_s XMLogWriterInit();

void XMLogWriterDestroy(pWriteTube_s psWriteTube);

int XMLogWriterInsert(pWriteTube_s psWriteTube, const char *LogData);

int XMLogWriterWrite(pWriteTube_s psWriteTube);

int XMLogWriterStop(pWriteTube_s psWriteTube);

int XMLogWriterStart(pWriteTube_s psWritetube, const char *FilePath, const cJSON *Data);

int XMLogWriterTimeout(pWriteTube_s psWritetube, int NowTime);

int XMLogWriterState(pWriteTube_s psWritetube);

typedef struct XMLogWriter_F
{
    pWriteTube_s (*Init)();
    void (*Destroy)(pWriteTube_s psWriteTube);
    int (*Insert)(pWriteTube_s psWriteTube, const char *LogData);
    int (*Write)(pWriteTube_s psWriteTube);
    int (*Stop)(pWriteTube_s psWriteTube);
    int (*Start)(pWriteTube_s psWriteTube, const char *FilePath, const cJSON *Data);
    int (*TimeOut)(pWriteTube_s psWriteTube, int NowTime);
    int (*State)(pWriteTube_s psWriteTube);
}XMLogWriter_f, *pXMLogWriter_f;

pXMLogWriter_f XMLogWriterFunInit();

void XMLogWriterFunDestroy(pXMFileRW_f pfXMLogWriter);
#ifdef __cplusplus
}
#endif
#endif