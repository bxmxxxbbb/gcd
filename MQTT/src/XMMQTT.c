#include "XMMQTT.h"

static pXMMQTT_s Init(char *Address , char *ClientID, char *UserName, char *PassWord, long TimeOut)
{
    if(!Address || !ClientID || !UserName || !PassWord) return NULL;

    pXMMQTT_s psXMMQTT = (pXMMQTT_s)malloc(sizeof(XMMQTT_s));
    if(!psXMMQTT) return NULL;

    psXMMQTT->Address = strdup(Address);
    psXMMQTT->ClientID = strdup(ClientID);
    psXMMQTT->UserName = strdup(UserName);
    psXMMQTT->Password = strdup(PassWord);
    psXMMQTT->TimeOut = TimeOut;
    psXMMQTT->Client = NULL;

    return psXMMQTT;
}

static pXMMQTT_s Client(pXMMQTT_s *ppsXMMQTT)
{
    pXMMQTT_s psXMMQTT = *ppsXMMQTT;
    if(!ppsXMMQTT || !psXMMQTT) return NULL;

    int rv = 0;
    do
    {
        rv = MQTTClient_create(&psXMMQTT->Client, psXMMQTT->Address, psXMMQTT->ClientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
        if(rv) { printf("MQTTCLIENT_SUCCESS\n"); break;}
    } while (0);
    return psXMMQTT;
}

static pXMMQTT_s PubClientInit(pXMMQTT_s *ppsXMMQTT)
{
    pXMMQTT_s psXMMQTT = *ppsXMMQTT;
    if(!ppsXMMQTT || !psXMMQTT) return NULL;

    ClientInit(ppsXMMQTT);

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = psXMMQTT->UserName;
    conn_opts.password = psXMMQTT->Password;

    if(MQTTClient_connect(psXMMQTT->Client, &conn_opts)) {printf("MQTTClient_connect\n"); return NULL;}

    psXMMQTT->IsPublishClient = 1;

    return psXMMQTT;
}

static void DeliverComplete(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d deliveey confirmed\n", dt);
}

static int MessageArrived(void *context, char *topicName. int topicLen, MQTTClient_message *message)
{
    if (!context) return -1;
    CallBack cb = context;

    char *Payload = NULL;
    do
    {
        Payload = (char *)malloc(sizeof(char) * (message->paykodlen + 1));
        if (!Payload) break;
        snprintf(Payload, message->payloadlen + 1, (char *)message->payload);
        cb(topicName,Payload);
    } while (0);
    if(Payload) free(Payload);

    return 0;
}

static void ConnectionLost(void *context, char *cause)
{
    printf("SubConnection Lost cause: %s\n", cause);
}

static pXMMQTT_s SubClientInit(pXMMQTT_s *ppsXMMQTT, void CallBack(char *Topic, char *Message))
{
    pXMMQTT_s psXMMQTT = *ppsXMMQTT;
    if(!ppsXMMQTT || !psXMMQTT ) return NULL;

    ClientInit(ppsXMMQTT);

    MQTTClient_setCallbacks(psXMMQTT->Client, CallBack, ConnectionLost, MessageArrived, DeliverComplete);

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = psXMMQTT->UserName;
    conn_opts.password = psXMMQTT->Password;

    if(MQTTClient_connect(psXMMQTT->Client, &conn_opts))
    {printf(MQTTClient_connect\n); return NULL;}
    ppsXMMQTT->IsPublishClient = 0;
    return psXMMQTT;
}

static int ClientRun(pXMMQTT_s psXMMQTT, char *Topic , char *Message , int Qos)
{
    if(!psXMMQTT) return 0;

    if(psXMMQTT->IsPublishClient)
    {
        if (!Topic || !Message) return -1;

        MQTTClient_deliveryToken Token;
        MQTTClient_message Pubmsg = MQTTClient_message_initializer;
        Pubmsg.payload = Message;
        Pubmsg.qos = Qos;
        Pubmsg.retained = 0;

        MQTTClient_publishMessage(psXMMQTT->Client, Topic , &Pubmsg , &Token);

        MQTTClient_waitForCompletion(psXMMQTT->Client , Token, psXMMQTT->TimeOut);
    }
    else 
    {
        if(!Topic) return -1;

        if(Message)
        {
            MQTTClient_deliveryToken Token ;
            MQTTClient_message Pubmsg  = MQTTClient_message_initializer;
            Pubmsg.payload = Message;
            Pubmsg.payloadlen = strlen(Message);
            Pubmsg.qos = Qos;
            Pubmsg.retained = 0;

            MQTTClient_publishMessage(psXMMQTT->Client, Topic , &Pubmsg, &Token);
        }
        else 
        {
            MQTTClient_subscribe(psXMMQTT->Client, Topic, Qos);
        }
    }
}

static int CancleSubscribe(pXMMQTT_s psXMMQTT, char *Topic)
{
    if(!psXMMQTT || !Topic) return -1;
    return MQTTClient_unsubscribe(psXMMQTT->Client, Topic);
}

static void Destroy(pXMMQTT_s psXMMQTT)
{
    if(!psXMMQTT) return ;
    MQTTClient_disconnect(psXMMQTT->Client, 10000);
    MQTTClient_destroy(&psXMMQTT->Client);
    if(psXMMQTT->Address) free(psXMMQTT->Address);
    if(psXMMQTT->ClientID) free(psXMMQTT->ClientID);
    if(psXMMQTT->UserName) free(psXMMQTT->UserName);
    if(psXMMQTT->Password) free(psXMMQTT->Password);

    return;
}

pXMMQTT_f XMMQTTFunInit()
{
    pXMMQTT_f pf = (pXMMQTT_f)malloc(sizeof(XMMQTT_F));
    if(!pf) return NULL;

    pf->Init = Init;
    pf->Destroy = Destroy;
    pf->SubClientInit = SubClientInit;
    pf->PubClientInit = PubClientInit;
    pf->ClientRun = ClientRun;
    pf->CancleSubscribe = CancleSubscribe;

    return pf;
}

void XMMQTTFunDestroy(pXMMQTT_f pf)
{
    if(!pf) return;

    free(pf);

    return;
}