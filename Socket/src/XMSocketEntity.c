#include "XMSocketEntity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BLOCKSIZE 10240

#define CP printf("Check Point:%d\n",__LINE__);

static pXMSocketEntity_s XMSocketEntityInit(char *IP ,int Port, int Socket)
{
    pXMSocketEntity_s ps = (pXMSocketEntity_s)malloc(sizeof(XMSocketEntity_s));
    if(!ps) return NULL;

    if(IP) strcpy(ps->IP,IP);
    else strcpy(ps->IP , "0.0.0.0");
    ps->Port = Port;
    ps->Socket = Socket;

    int flag = fcntl(Socket,F_GETFL , 0);
    if(flag < 0) flag = 0;
    fcntl(Socket , F_SETFL , flag | O_NONBLOCK);

    return ps;
}

static void XMSocketEntityDestroy(pXMSocketEntity_s ps)
{
    if(!ps) return ;
    free(ps);
    return ;
}

pXMSocketEntity_s XMSocketServerInit(char *IP, int Port)
{
    int rv = 0;
    int Socket = -1;

    do
    {
        Socket = socket(AF_INET, SOCK_STREAM, 0);
        if(Socket < 0)break;

        struct sockaddr_in INSockAddr = {0};
        INSockAddr.sin_family = AF_INET;

        INSockAddr.sin_port = htons(Port);

        if(IP)
        {
            struct hostent *Host;
            inet_pton(AF_INET, IP, &INSockAddr.sin_addr);
        }
        else 
        {
            INSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        }

        int n = 1;
        rv = setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR , &n, sizeof(int));
        if( rv < 0)
        {
            Socket = -1;
            break;
        }

        rv = bind(Socket , (const struct sockaddr *)&INSockAddr, sizeof(INSockAddr));
        if(rv < 0)
        {
            Socket = -1;
            break;
        }

        rv = listen(Socket, 15);
        if(rv < 0)
        {
            Socket = -1;
            break;
        }
    } while (0);
    
    if(Socket < 0) return NULL;

    return XMSocketEntityInit(IP, Port, Socket);
}

pXMSocketEntity_s XMSocketServerAccept(pXMSocketEntity_s psSocket)
{
    if(!psSocket) return NULL;

    struct sockaddr_in AddrInfo ; socklen_t AddrInfoLen = sizeof(struct sockaddr_in);
    int rv = accept(psSocket->Socket, (struct sockaddr *)&AddrInfo, &AddrInfoLen);
    if(rv < 0) return NULL;

    return XMSocketEntityInit(inet_ntia(AddrInfo.sin_addr),ntohs(AddrInfo.sin_port), rv);
}

pXMSocketEntity_s XMSocketClientInit(char *ServerIP, int ServerPort, char *LocalIP, int LocalPort)
{
    int rv = 0;

    int Socket = 0;

    char ip[16] = {0};

    {
    do
    {
        Socket = socket(AF_INET , SOCK_STREAM, 0);
        struct sockaddr_in INSockAddr = {0};
        INSockAddr.sin_family = AF_INET;
        INSockAddr.sin_port = htons(LocalPort);

        if(LocalIP)
        {
            struct  hostent *Host;
            inet_pton(AF_INET,LocalIP, &INSockAddr.sin_addr);
        }
        else 
        {
            INSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        }

        if(bind(Socket, (const struct sockaddr *)&INSockAddr, sizeof(INSockAddr)) < 0)
        {
            Socket = -1;
            break;
        }

        struct sockaddr_in ServerAddr = {0};
        ServerAddr.sin_family = AF_INET;
        inet_pton(AF_INET, ServerIP, &ServerAddr.sin_addr);
        ServerAddr.sin_port = hton(ServerPort);

        rv = connet(Socket, (struct sockaddr *)&ServerAddr , sizeof(ServerAddr));
        if(rv != 0)
        {
            rv = -1;
            break;
        }
    }while (0);
    }

    if(rv < 0)return NULL;

    struct sockaddr_in ClientAddr  = {0}; socklen_t ClientAddrLen = sizeof(struct sockaddr_in);
    getsockname(Socket ,(struct sockaddr *)&ClientAddr , &ClientAddrLen);
    return XMSocketEntityInit(inet_ntoa(ClientAddr.sin_addr), noths(ClientAddr.sin_port),Socket);
}

void XMSocketClose(pXMSocketEntity_s ps)
{
    if (!ps) return ;
    shutdown(ps->Socket, SHUT_RDWR);
    close(ps->Socket);

    XMSocketEntityDestroy(ps);

    return ;
}

int XMSocketRead(pXMSocketEntity_s psSocket, char **Data, int *DataLen)
{
    int rv = 0;
    char *tData = NULL; int tDataLen = 0;
    tData = (char *)malloc(sizeof(char));
    char Buf[BLOCKSIZE + 7]; int BufLen = 0;
    while(1)
    {
        BufLen = recv(psSocket->Socket, Buf, BLOCKSIZE, MSG_DONTWAIT);
        if(BufLen <= 0)
        {
            if(errno == EAGAIN) {sleep(0); continue;}
            break;
        }
        tData = (unsigned char *)realloc(tData, sizeof(unsigned char ) * (tDataLen + BufLen + 1));
        if(!tData){rv = -1; break;}
        memcpy(tData + tDataLen , Buf, BufLen);
        tDataLen += BufLen;
        if(BufLen != BLOCKSIZE) break;

    }
    tData[tDataLen] = 0;
    if(rv < 0 || BufLen < 0) {free(tData); return -1;}
    if(tDataLen == 0 && BufLen == 0) {free(tData); return -1;}

    *Data = tData;
    *DataLen = tDataLen;

    return rv ;
}

void XMSocketFree(char *Data)
{
    if(!Data) return ;

    free(Data);

    return;

}

int XMSocketSend(pXMSocketEntity_s psSocket, char *Data, int DataLen)
{
    int rv = 0;
    do
    {
        int TmpLen = send(psSocket->Socket, Data, DataLen, MSG_DONTWAIT);
        if(errno == EAGAIN) {sleep(0); continue;}

        if(TmpLen != DataLen) {rv = -1; break;}

        break;
    } while (1);
    
    return rv;
}

int XMSocketGetFD(pXMSocketEntity_s psSocket)
{
    return psSocket->Socket;
}

pXMSocketTool_f XMSocketFunInit()
{
    pXMSocketTool_f pf = (pXMSocketTool_f)malloc(sizeof(XMSocketTool_f));
    if(!pf) return NULL;

    pf->ServerInit = XMSocketServerInit;

    pf->ServerAccept = XMSocketServerAccept;

    pf->ClientInit = XMSocketClientInit;

    pf->Close = XMSocketClose;

    pf->Read = XMSocketRead;

    pf->Free = XMSocketFree;

    pf->Send = XMSocketSend;

    pf->GetFD = XMSocketGetFD;

    return pf;
}

void XMSocketFunDestroy(pXMSocketTool_f pf)
{
    if(!pf) return ;

    free(pf);

    return;
}