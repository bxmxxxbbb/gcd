#ifndef __XMSOCKETENTITY_H__
#define __XMSOCKETENTITY_H__

#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

typedef struct  XMSocketEntity_S
{
    int Port;
    char IP[20];
    int Socket;
}XMSocketEntity_s, *pXMSocketEntity_s;

/**
 * @brief Socket服务监听初始化
 * 
 * @param ServerIP 可选
 * @param ServerPort
 * @return pXMSocketEntity_s Socket结构实体
 */
pXMSocketEntity_s XMSocketServerInit(char *ServerIP, int ServerPort);

/**
 * @brief  Socket服务监听Accept
 * 
 * @param psSocket 服务端Socket结构实体
 * @retrun pXMSocketEntity_s 客户端Socket结构实体
 */
pXMSocketEntity_s pXMSocketServerAccept(pXMSocketEntity_s psSocket);

/**
 * @brief Socket服务端初始化
 * 
 * @param ServerIP
 * @param ServerPort
 * @param LocalIP
 * @param LocalPort
 * @return pXMSocketEntity_s Socket结构实体
 */

/**
 * @brief 关闭Socket
 * 
 * @param psSocket Socket结构实体
 */
void XMSocketClose(pXMSocketEntity_s psSocket);

/**
 * @brief 
 * 
 * @param psSocket
 * @param Data
 * @param DataLen
 * @return int
 */
int XMSocketRead(pXMSocketEntity_s psSocket, char **Data, int *DataLen);

void XMSocketFree(char *Data);

int XMSocketSend(pXMSocketEntity_s psSocket , char *Data, int DataLen);

typedef struct XMSocketTool_F
{
    pXMSocketEntity_s (*ServerInit)(char *ServerIP, int ServerPort);
    
    pXMSocketEntity_s (*ServerAccept)(pXMSocketEntity_s psSocket);

    pXMSocketEntity_s (*ClientInit)(char *ServerIP, int ServerPort, char *LocalIP, int LocalPort);

    void (*Close)(pXMSocketEntity_s);

    int (*Read)(pXMSocketEntity_s , char **Data, int *DataLen);

    void (*Free)(char *Data);

    int (*GetFD)(pXMSocketEntity_s);

    int (*Send)(pXMSocketEntity_s , char *Data, int DataLen);
}XMSocketTool_f, *pXMSocketTool_f;


pXMSocketTool_f XMSocketFunInit();

void XMSocketFunDestroy(pXMSocketTool_f);
#endif