#ifndef __XMSOCKET_H__
#define __XMSOCKET_H__

#include "XMSocketEntity.h"

#include "XMList.h"

#include <pthread.h>

enum
{
    XMSocketTCP,
    XMSocketUDP,

    XMSocketServer,
    XMSocketClient,

    XMSocketSelect,
    XMSocketEPoll,

    XMSocketCrypto

};
struct  XMSocket_P
{
    int ProtocolMode;
    int ServiceMode;
    int MultiplexingMode;
    int CryptoMod;

    char *LocalIP ;
    int LocalPort;
    char *TargetIP;
    int TargetPort;

    void *(*LogInCB)(pXMSocketEntity_s, void *);
    void *LogInParam;
    void *(*ReadCB)(pXMSocketEntity_s , char *, int , void *);
    void *ReadParam;
};

typedef struct  XMSocket_S
{
    int CryptoMod;

    char LocalIP[20]; int LocalPort;
    char TargetIP[20]; int TargetPort;

    pXMSocketEntity_s psLocalSocket;
    pXMList_l plSocket;

    void *(*LogInCB)(pXMSocketEntity_s ,void *);
    void *LogInParam;

    void *(*ReadCB)(pXMSocketEntity_s , char *, int , void *);
    void *ReadParam;

    int Alive;

    pthread_mutex_t Locker;
}XMSocket_s, *pXMSocket_s;

#endif