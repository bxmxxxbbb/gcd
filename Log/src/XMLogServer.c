#include "XMFileConfigure.h"
#include "XMLogWriter.h"
#include "XMSocketTool.h"
#include "XMList.h"

#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

XMFileConfigure_s psConf[] = 
{
    {"LogPath" , NULL},
    {"LocalPort" , NULL},
    {"FileDuration", NULL},
    {"PollIntervalTime" ,NULL},
    {0 , 0},
};

pWriteTube_s psWriteTube[1024];

int Close = 0;

void *PollWriteTube(void *Param)
{
    while (1)
    {
        for(int i = 0; i < 1024; i++) XMLogWriterWrite(psWriteTube[i]);

        if(Close) break;
        sleep(atoi(psConf[3].Value));
    }
    return ;
}

int SendVerify(int ClientSocket)
{
    int rv = 0;
    do
    {
        fd_set fd;
        FD_ZERO(&fd);
        FD_SET(ClientSocket, &fd);
        timeout.tv_sec  = 10;

        rv = select(ClientSocket + 1, NULL,&fd, NULL, &timeout);
        if(rv <= 0) break;

        if(FD_ISSET(ClientSocket, &fd))
        {
            rv = XMSocketSend(ClientSocket, "Update", 6);
            if(rv < 0) break;
        }
    } while (0);
    return rv;
}

void ListenSocket()
{
    int rv = 0;
    pXMList_l plSocketList = XMListNew(free);
    if(!plSocketList) return ;
    pXMListIterator_s it = NULL;

    int ServerSocket = XMSocketServerInit(atoi(psConf[1].Value));
    if(ServerSocket <= 0) return ;

    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(ServerSocket, &fd);
    struct timeval timeout;
    int ClientSocket;
    int MaxSocket = ServerSocket;
    int CleanFlag = 0;
    while (1)
    {
        timeout.tv_sec = 10;
        rv = select(MaxSocket + 1 , &fd , NULL , NULL, &timeout);
        if(rv < 0) break;
        if(rv == 0) continue;

        MaxSocket = ServerSocket;
        while (it = XMListEach(plSocketList, it))  
        if(FD_ISSET (*(int *)it->Data, &fd))
        do
        {
            int ClientSocket = *(int *)it->Data;
            cJSON *Root = NULL;
            char *Data = NULL; int DataLen = 0;
            do
            {
                if(!XMLogWriterState(psWriteTube[ClientSocket]))
                {
                    rv = SendVerify(ClientSocket);
                    break;
                }

                rv = XMSocketRead(ClientSocket, &Data, &DataLen);
                if(rv < 0) break;

                Root = cJSON_Parse(Data);
                if(!Root) break;

                if(cJSON_GetObjectItem(Root, "FileName"))
                {
                    rv = XMLogWriterStart(psWriteTube[ClientSocket] , psConf[0].Value, Root);
                    if ( rv < 0) break;
                }
                else 
                {
                    cJSON *LogData = cJSON_GetObjectItem(Root, "LogData");
                    if (!LogData)  break;

                    rv = XMLogWriterInsert(psWriteTube[ClientSocket], LogData->valuestring);
                    if(rv < 0) break;
                }
            } while (0);
            cJSON_Delete(Root);
            XMSocketFree(Data);

            if(rv < 0 || XMLogWriterTimeOut(psWriteTube[ClientSocket], XMGetTime(NULL)))
            {
                XMLogWriterStop(ClientSocket);
                XMSocketClose(ClientSocket);
                XMListEach(plSocketList,it);
                CleanFlag = 1;
                break;
            }
            if(ClientSocket > MaxSocket) MaxSocket = ClientSocket;
        } while (0);
        
        if(CleanFlag)
        {
            XMListClean(plSocketList);
            CleanFlag = 0;
        }

        if(FD_ISSET(ServerSocket, &fd))
        do
        {
            ClientSocket = XMSocketServerAccept(ServerSocket);
            if(ClientSocket <= 0) break;

            rv = SendVerify(ClientSocket);
            if(rv < 0) break;

            int *pClientSocket = (int *)malloc(sizeof(int));
            *pClientSocket = ClientSocket;
            XMListPushBack(plSocketList, (void *)pClientSocket);
        } while (0);   
    }
}

int main()
{
    for (int i = 0; i < 1024; i++)
    {
        psWriteTube[i] = XMLogWriterInit();
    }

    XMFileConfigureRead(psConf, "/etc/XMLogServer.conf");

    pthread_t Pid;

    pthread_create(&Pid, NULL , PollWriteTube, NULL);
    pthread_detach(Pid);

    ListenSocket();

    pthread_cancel(Pid);
    sleep(Atoi(psConf[3].Value) + 3);
    Close = 1;

    XMFileConfigureClear(psConf);
    for(int i = 0; i < 1024; i++) XMLogWriterDestroy(psWriteTube[i]);

    return 0;
}