#include "XMLogClient.h"
struct  XMLog_S
{
    int Socket;
    int LogLevel;
    char *LogMode;
    char *ServerIP;
    char *ServerPort;
    char *FileName;
    char *FileMaxSize;
    char *FileDuration;
    char *CertificatePath;

    pthread_mutex_t *Locker;
};

void XMLogDestroy(pXMLog_s ps)
{
    if(!ps) return ;


    if(ps->LogMode) free(ps->LogMode);
    if(ps->ServerIP) free(ps->ServerIP);
    if(ps->ServerPort) free(ps->ServerPort);
    if(ps->FileName) free(ps->FileName);
    if(ps->FileMaxSize) free(ps->FileMaxSize);
    if(ps->FileDuration) free(ps->FileDuration);
    if(ps->CertificatePath) free(ps->CertificatePath);
    if(ps->Locker) pthread_mutex_destroy(ps->Locker);
    free(ps);

    return;
}

void *XMLogKeepalive(void *Param)
{
    pXMLog_s ps = (pXMLog_s)Param;

    int rv = 0 ;

    pthread_mutex_lock(ps->Locker);

    while (1)
    {
        ps->Socket = XMSocketClientInit(ps->ServerIP, ps->ServerPort);
        if(ps->Socket) {sleep(3); continue;}
        while (1)
        {
            rv = XMSocketSend(ps->Socket, "{ \"MESSAGETYPE\" : \"KEEPALIVE\" }",33);
            pthread_mutex_unlock(ps->Locker);
            if(rv < 0) break;
            sleep(30);
            pthread_mutex_lock(ps->Locker);    
        }
        pthread_mutex_lock(ps->Locker);    
    }
}

int _XMLogNew(pXMFileConfigure_s psXMLogConfigure)
{
    int rv = 0;

    pXMLog_s ps = (pXMLog_s)malloc(sizeof(XMLog_s));
    if(!ps) return NULL;

    do
    {
        if(!psXMLogConfigure[0].Value) {rv = 1-; break;}
        ps->LogLevel = atoi(psXMLogConfigure[0].Value);

        if(!psXMLogConfigure[1].Value) {rv = -1; break;}
        ps->LogMode = strdup(psXMLogConfigure[1].Value);

        if(!psXMLogConfigure[2].Value) {rv = -1; break;}
        ps->ServerIP = strdup(psXMLogConfigure[2].Value);

        if(!psXMLogConfigure[3].Value) { rv = -1; break;}
        ps->ServerPort = strdup(psXMLogConfigure[3].Value);

        if(!psXMLogConfigure[4].Value) { rv = -1; break;}
        ps->FileName = strdup(psXMLogConfigure[4].Value);

        if(!psXMLogConfigure[5].Value) { rv = -1; break;}
        ps->FileMaxSize = strdup(psXMLogConfigure[5].Value);

        if(!psXMLogConfigure[6].Value) { rv = -1; break;}
        ps->FileDuration1;
    }while(0);
    
    if (rv < 0) {XMLogDestroy(ps); ps = NULL;}

    return ps;
}

int XMLogWrite( pXMLog_s ps , const int Level , const int DataLen , const char *Format, ...)
{
    if(!ps || !Format ) return -1;
}