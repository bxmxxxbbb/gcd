#include "XMSocket.h"

#include <unistd.h>

static void *Listener_TCP_Select(void *Param)
{
    pXMSocket_s psSocket = (pXMSocket_s)Param;
    if(!psSocket) return NULL;

    int rv = 0;
    fd_set fd;
    struct timeval timeout;
    pXMListIterator_s psIt = NULL;
    int MaxSocket = 0;
    do
    {
        if(psSocket->Alive == 0) break;

        FD_ZERO(&fd);
        
        FD_SET(psSocket->psLocalSocket->Socket, &fd);
        MaxSocket = psSocket->psLocalSocket->Socket;

        if(psSocket->plSocket)
        {
            psIt = NULL;
            XMListEachEX(psSocket->plSocket, psIt)
            {
                pXMSocketEntity_s psSocketEntity = psIt->Data;
                FD_SET(psSocketEntity->Socket,&fd);
                if(psSocketEntity->Socket > MaxSocket) MaxSocket = psSocketEntity->Socket;
            }
        }
        timeout.tv_sec = 10;
        rv = select(MaxSocket + 1, &fd, NULL, NULL, &timeout);
        if(rv < 0) {rv = -1; break;}

        if(psSocket->plSocket)
        {
            psIt = NULL;
            XMListEachEX(psSocket->plSocket, psIt)
            {
                pXMSocketEntity_s psSocketEntity = psIt->Data;
                if(FD_ISSET(psSocketEntity->Socket, &fd))
                {
                    char *Data = NULL ;int DataLen = 0;
                    do
                    {
                        rv = XMSocketRead(psSocketEntity->Socket, &Data, &DataLen);
                        if(rv < 0)
                        {
                            psIt = XMListErase(psSocket->plSocket, psIt);
                            break;
                        }
                        psSocket->ReadCB(psIt, Data, DataLen, psSocket->ReadParam);
                    }while(0);
                    if(Data) XMSocketFree(Data);
                }
            }
        }
        if(FD_ISSET(psSocket->psLocalSocket->Socket, &fd))
        {
            if(psSocket->plSocket)
            {
                pXMSocketEntity_s ps = XMSocketServerAccept(psSocket->psLocalSocket);
                if(ps)
                {
                    XMListPushBack(psSocket->plSocket, ps);
                    psSocket->LogInCB(ps,psSocket->LogInParam);
                }
            }
            else 
            {
                char *Data = NULL; int DataLen = 0;
                do
                {
                    rv = XMSocketRead(psSocket->psLocalSocket , &Data , &DataLen);
                    if( rv < 0)
                    {
                        psSocket->psLocalSocket = XMSocketClientInit(psSocket->TargetIP, psSocket->TargetPort, psSocket->LocalIP, psSocket->LocalPort);
                        break;
                    }
                    psSocket->ReadCB(psSocket->psLocalSocket,Data,DataLen,psSocket->ReadParam);
                }while(0);
                if(Data) XMSocketFree(Data);
            }
        }
        sleep(0);
    }while(1);
    if(psSocket->plSocket) XMListDestroy(psSocket->plSocket);
    free(psSocket);

    return NULL;
}

static void *Listener_TCP_RPool(void *Param)
{

}

void XMSocketDestroy(pXMSocket_s psXMSocket)
{
    if(!psXMSocket) return ;

    psXMSocket->Alive = 0;

    return ;
}

pXMSocket_s XMSocketInit(struct XMSocket_P pXMSocket)
{
    pXMSocket_s ps =NULL;

    int rv = 0;

    pXMSocketEntity_s psSocketEntity = 0;
    do
    {
        ps = (pXMSocket_s)malloc(sizeof(XMSocket_s));
        if(!ps) {rv = -1; break;}

        ps->CryptoMod = pXMSocket.CryptoMod;

        strcpy(ps->LocalIP, pXMSocket.LocalIP);
        strcpy(ps->TargetIP, pXMSocket.TargetIP);
        ps->LocalPort = pXMSocket.LocalPort;
        ps->TargetPort = pXMSocket.TargetPort;

        ps->ReadCB = pXMSocket.ReadCB;
        ps->ReadParam = pXMSocket.ReadParam;

        ps->LogInCB = pXMSocket.LogInCB;
        ps->LogInParam = pXMSocket.LogInParam;

        ps->Alive = 1;

        if(pXMSocket.ServiceMode == XMSocketServer)
        {
            while(!(ps->psLocalSocket = XMSocketServerInit(pXMSocket.LocalIP,pXMSocket.LocalPort))) sleep(3);

            ps->plSocket = XMListInit(XMSocketClose);
            if(!ps->plSocket) {rv = -1; break;}
        }
        else 
        {
            while(!(ps->psLocalSocket = XMSocketClientInit(pXMSocket.TargetIP,pXMSocket.TargetPort,pXMSocket.LocalIP,pXMSocket.LocalPort))) sleep(3);

            ps->plSocket = NULL;
        }

        if(pXMSocket.ProtocolMode == XMSocketTCP)
        {
            pthread_t pid = 0;
            if(pXMSocket.MultiplexingMode == XMSocketSelect)
            {
                pthread_create(&pid, NULL , Listener_TCP_Select, ps);
                pthread_detach(pid);
            }
            else if(pXMSocket.MultiplexingMode == XMSocketEPoll)
            {

            }
            else 
            {
                rv = -1; break;
            }
        }
        else if(pXMSocket.ProtocolMode == XMSocketUDP)
        {

        }
        else 
        {
            rv = -1; break;
        }
    }while(0);
    if (rv < 0 && ps) free(ps);

    return ps;
}

int XMSocketSendData(pXMSocket_s psSocket, const char *Data,const int DataLen)
{
    if(!psSocket) return -1;

    return XMSocketSend(psSocket->psLocalSocket, Data, DataLen);
}