#ifndef __XMMQTT_H__
#define __XMMQTT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"

enum QosType_E
{
    Repeated,
    OnlyOnce,
    LeastOnce
};

typedef struct  XMQTT_S
{
    MQTTClient Client;
    char *Address;
    char *ClientID;
    char *UserName;
    char *Password;
    long TimeOut;

    int IsPublishClient;
}XMQTT_s, *pXMMQTT_s;

typedef void (CallBack)(char *Topic, char *Message);

typedef struct  XMMQTT_F
{
    /**
     * @brief 
     * 
     * @param Address
     * @param ClientID
     * @param UserName
     * @param TimeOut
     * @return pXMMQTT_s
     */
    pXMMQTT_s (*Init)(char *Address, char *ClientID, char *UserName. char *Password, long TimeOut);

    /**
     * @brief 
     * 
     * @param ppsXMMQTT
     * @return pXMMQTT_s
     */
    pXMMQTT_s (*PubClientInit)(pXMMQTT_s *ppsXMMQTT);

    /**
     * @brief 
     * 
     * @param ppsXMMQTT
     * @param CallBack
     * @return pXMMQTT_s
     */
    pXMMQTT_s (*SubClientInit)(pXMMQTT_s *ppsXMMQTT, void CallBack(char *Topic, char *Message));

    /**
     * @brief 
     * 
     * @param psXMMQTT
     * @param Topic
     * @param Message
     * @param Qos
     * @return int
     */
    int (*ClientRun)(pXMMQTT_s psXMMQTT, char *Topic , char *Message, int Qos);

    /**
     * @brief 
     * 
     * @param psXMMQTT
     * @param Topic
     * @return int
     */
    int (*CancleSubscribe)(pXMMQTT_s psXMMQTT, char *Topic);

    /**
     * @brief 
     * 
     * @param psXMMQTT
     */
    void (*Destroy)(pXMMQTT_s psXMMQTT);
}XMMQTT_f *pXMMQTT_f;

pXMMQTT_f XMMQTTFunInit();

void XMMQTTFunDestroy(pXMMQTT_f);
#endif