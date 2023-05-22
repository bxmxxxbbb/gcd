#include "XMSkipList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define CP printf("%s:%d\n",__FILE__, __LINE__);

#define IsOverTime(Time) (Time && (Time < 0 || Time < time(NULL)))

typedef struct Value_S
{
    void *Value;
    int OverTime;
}Value_s, *pValue_s;

typedef struct  Node_S
{
    char *Key;
    struct Node_S *Up, *Dn , *Next, *Last;
    pValue_s Value;
}Node_s, *pNode_s;

static pthread_mutex_t NodePoolLocker = PTHREAD_MUTEX_INITIALIZER;

pNode_s psNodePool = NULL;

/**
 * @brief  节点池初始化
 */

static void NodePoolInit()
{
    static int Count = 1, MaxCount = 2048;
    if(Count < 2048) Count <<= 1;
    int Cnt = Count;
    while (Cnt--)
    {
        pNode_s psNode = (pNode_s)malloc(sizeof(Node_s));
        if(!psNode) continue;

        psNode->Key = NULL;
        psNode->Value = NULL;
        psNode->Up = NULL;
        psNode->Dn = NULL;
        psNode->Next = psNodePool;
        psNode->Last = NULL;

        psNodePool = psNode;
    }
    return;
}

/**
 * @brief 节点池销毁
 * 
 */
void NodePoolDestroy()
{
    pNode_s psNode = NULL;
    while (psNodePool)
    {
        psNode = psNodePool;
        psNodePool = psNode->Next;
        free(psNode);
    }
    return;
}

/**
 * @brief 节点池获取节点
 * 
 * @return pNode_s 获取的节点
 */
static pNode_s NodePoolGet()
{
    pNode_s psNode = NULL;
    pthread_mutex_lock(&NodePoolLocker);
    if(!psNodePool) NodePoolInit();
    if(!psNodePool) return NULL;
    psNode = psNodePool;
    psNodePool = psNode->Next;
    psNode->Next = NULL;
    pthread_mutex_unlock(&NodePoolLocker);
    return psNode;
}

/**
 * @brief 节点池返还
 * 
 * @param psNode 返还的节点
 */
static void NodePoolReturn(pNode_s psNode)
{
    if(!psNode) return;

    psNode->Key = NULL;
    psNode->Value = NULL;
    psNode->Up = NULL;
    psNode->Dn = NULL;
    psNode->Last = NULL;
    psNode->Next = NULL;
    pthread_mutex_lock(&NodePoolLocker);
    psNode->Next = psNodePool;
    psNodePool = psNode;
    pthread_mutex_unlock(&NodePoolLocker);

    return ;
}

/**
 * @brief 插入到当前节点的右方
 * 
 * @param psNode 当前节点
 * @param psInsertNode 插入节点
 */
static void NodeInsertNext(pNode_s psNode,pNode_s psInsertNode)
{
    psInsertNode->Next = psNode->Next;

    if(psNode->Next) psNode->Next->Last = psInsertNode;

    psNode->Next = psInsertNode;

    psInsertNode->Last = psNode;

    return ;
}

/**
 * @brief 删除节点
 * 
 * @param psNode 当前节点
 */
static void NodeDelete(pNode_s psNode)
{
    if (!psNode) return;

    if(psNode->Last) psNode->Last->Next = psNode->Next;
    if(psNode->Next) psNode->Next->Last = psNode->Last;

    NodePoolReturn(psNode);

    return ;
}

typedef struct  List_S
{
    long long Count ;
    struct List_S *Up;
    struct List_S *Dn;
    pNode_s Head;
}List_s, *pList_s;

/**
 * @brief 新建链表
 * 
 * @return pList_s 生成的新链表
 */
static pList_s ListNew()
{
    pList_s psList = (pList_s)malloc(sizeof(List_s));
    if(!psList) return NULL;

    psList->Count = 0;
    psList->Up = NULL;
    psList->Dn = NULL;
    psList->Head = NodePoolGet();

    return psList;
}

/**
 * @brief 销毁链表
 * 
 * @param  psList 销毁链表
 */
static void ListDestroy(pList_s psList)
{
    if(!psList) return ;

    if(psList->Up) psList->Up->Dn = psList->Dn;
    if(psList->Dn) psList->Dn->Up = psList->Up; 

    pNode_s psNode = NULL;
    while (psList->Head)
    {
        psNode = psList->Head;
        psList->Head = psList->Head->Next;
        NodeDelete(psNode);
    }
    free(psList);
    return ;
}

/**
 * @brief 上层添加链
 * 
 */
static pList_s ListInsertUp(pList_s psList)
{
    if(!psList) return NULL;

    pList_s ps = ListNew();
    
    if(psList->Head->Up) psList->Up->Dn = ps;
    ps->Up = psList->Up;
    ps->Dn = psList;
    psList->Up = ps;

    if(psList->Up) psList->Head->Up->Dn = ps->Head;
    ps->Head->Up = psList->Head->Up;
    ps->Head->Dn = psList->Head;
    ps->Head->Up = ps->Head;

    return ps;
}

typedef struct  XMSkipList_S
{
    pthread_rwlock_t RWLock;
    short Close;

    pList_s psList;
    int Count;

    int CleanInterval;
    void (*Free)(void *Param);
    void *(*Copy)(const void *Param);
}XMSkipList_s, *pXMSkipList_s;

typedef struct XMSkipIterator_S
{
    pList_s psList;
    pNode_s psNode;
}XMSkipIterator_s, *pXMSkipIterator_s;
#define XMSkipIteratorInit(psXMSkipList) {psXMSkipList->psList,psXMSkipList->psList->Head}

static void *Clean(void *Param)
{
    pXMSkipList_s ps = Param;

    pthread_rwlock_wrlock(&ps->RWLock);
    if(ps->Close) return NULL;
    pList_s psList = ps->psList;
    while(psList->Dn) psList = psList->Dn;
    pthread_rwlock_unlock(&ps->RWLock);

    pNode_s psNode = psList->Head;

    int cnt, floor;
    while (1)
    {
        if(ps->Close) break;

        pthread_rwlock_wrlock(&ps->RWLock);
        cnt = 10000;
        while (cnt--)
        {
            if(!psNode->Next)
            {
                psNode = psList->Head;
                break;
            }
            if(IsOverTime(psNode->Next->Value->OverTime))
            {
                pNode_s t1psNode = psNode->Next , t2psNode;
                floor = 1;

                while (t1psNode)
                {
                    t2psNode = t1psNode;
                    t1psNode = t1psNode->Up;

                    if(floor)
                    {
                        if(ps->Free && t2psNode->Value->Value) ps->Free(t2psNode->Value->Value);
                        t2psNode->Value->Value = NULL;
                        free(t2psNode->Key);
                        free(t2psNode->Value);
                        floor = 0;
                    }
                    NodeDelete(t2psNode);
                }
            }
            psNode = psNode->Next;
        }
        pthread_rwlock_unlock(&ps->RWLock);
        sleep(ps->CleanInterval);
    }
    pthread_rwlock_destroy(&ps->RWLock);
    free(ps);
    
    return NULL;
}

/**
 * @brief 跳链初始化
 * 
 * @param sParam 跳链入参
 * @return pXMSkipList_s 跳链s
 */
pXMSkipList_s XMSkipListInit(const XMSkipListParam_s sParam)
{
    pXMSkipList_s ps = (pXMSkipList_s)malloc(sizeof(XMSkipList_s));
    if(!ps) return NULL;

    pthread_rwlock_init(&ps->RWLock, NULL);

    ps->CleanInterval = sParam.CleanInterval;
    ps->Free = sParam.Free;
    ps->Copy = sParam.Copy;

    ps->Count = 0;

    ps->psList = ListNew();

    
    ps->Close = 0;
    pthread_t pid;
    pthread_create(&pid, NULL, Clean, ps);
    pthread_detach(pid);

    return ps;
}

/**
 * @brief 跳链销毁
 * 
 * @param psXMSkipList 跳链
 */
void XMSkipListDestroy(pXMSkipList_s psXMSkipList)
{
    if(!psXMSkipList) return;

    pthread_rwlock_wrlock(&psXMSkipList->RWLock);

    pList_s psList = psXMSkipList->psList;

    pNode_s psNode = NULL;

    psXMSkipList->Close = 1;

    if(psXMSkipList->Free)
    {
        while (psList->Dn) psList = psList->Dn;
        psNode = psList->Head->Next;
        
        while (psNode)
        {
            if(psNode->Value->Value)
            {
                psXMSkipList->Free(psNode->Value->Value);
                psNode->Value->Value = NULL;
            }
            psNode = psNode->Next;
        }
    }
    psNode = psList->Head->Next;
    while (psNode)
    {
        if(psNode->Key) free(psNode->Key);
        psNode->Key = NULL;
        psNode = psNode->Next;
    }

    while (psXMSkipList->psList)
    {
        psList = psXMSkipList->psList;
        psXMSkipList->psList = psList->Dn;
        ListDestroy(psList);
    }
    pthread_rwlock_unlock(&psXMSkipList->RWLock);

    return ;
}

/**
 * @brief 键值比对
 * 
 * @param psNode 节点
 * @param Key 键值
 * @return int 节点键值>键值 || 节点为空：1 节点键值==键值：0 节点键值<键值：-1
 */
static int KeyCmp(pNode_s psNode,const char *Key)
{
    if(!psNode) return 1;

    int rv = 0;

    static int i = 0,Len = 0;

    do
    {
        Len = strlen(psNode->Key);
        if(strlen(Key) < Len) { Len = strlen(Key); rv = 1;}
        else if(strlen(Key) == Len) rv = 0;
        else rv = -1;
        for(i = 0 ; i < Len ; i++)
        {
            if(psNode->Key[i] < Key[i]){rv = -1; break;}
            else if(psNode->Key[i] == Key[i]) continue;
            else {rv = 1; break;}
        }
    }while (0);
    return rv ;    
}

/**
 * @brief 查找键应该出现的位置的前一个节点
 * 
 * @param pXMSkipIterator_s 跳链二元组句柄
 * @param Key 键值
 * @return int 0:已找到 其他: 未找到
 */
static int FindLast(pXMSkipIterator_s psIt, const char *Key)
{
    if(!psIt || !Key) return -1;
    int  rv = 0;

    while (psIt->psNode)
    {
        rv = KeyCmp(psIt->psNode->Next, Key);
        if(rv == 1)
        {
            if(!psIt->psList->Dn) break;
            psIt->psNode = psIt->psNode->Dn;
            psIt->psList = psIt->psList->Dn;
        }
        else if (rv == 0)
        {
            psIt->psNode = psIt->psNode->Next;
            while (psIt->psNode->Dn)
            {
                psIt->psList = psIt->psList->Dn;
                psIt->psNode = psIt->psNode->Dn;
                // printf("%x\n",psIt->psNode);
            }
            psIt->psNode = psIt->psNode->Last;
            break;
        }
        else 
        psIt->psNode = psIt->psNode->Next; 
    }
    return rv ;
}

static int LevelUp()
{
    static int i = 5;

    if(i++ == 5)
    {
        srand((int)time(NULL));
        i = 0;
    }

    int random = rand();

    if(random % 7 > 3) return 1;
    else return 0;
}

int XMSkipListInsert(pXMSkipList_s psXMSkipList, const char *Key, void *Value, int OverTime)
{
    if(!psXMSkipList || !Key || !Value) return -1;

    pthread_rwlock_wrlock(&psXMSkipList->RWLock);

    int rv = 0;

    {
    do{
        XMSkipIterator_s sXMSkipIterator  = XMSkipIteratorInit(psXMSkipList);

        rv = FindLast(&sXMSkipIterator, Key);

        if(rv)
        {
            pNode_s ps = NodePoolGet();
            if(!ps) {rv = -1; break;}

            ps->Key = malloc(sizeof(char *) * (strlen(Key) + 1));
            if(!ps->Key) { rv = -1; break;}
            snprintf(ps->Key, strlen(Key) + 1, "%s", Key);

            ps->Value = (pValue_s)malloc(sizeof(Value_s));

            if(psXMSkipList->Copy)
                ps->Value->Value = psXMSkipList->Copy(Value);
            else 
                ps->Value->Value = Value;
            
            if(OverTime)
                ps->Value->OverTime = OverTime + time(NULL);
            
            NodeInsertNext(sXMSkipIterator.psNode, ps);
            sXMSkipIterator.psList->Count++;
            sXMSkipIterator.psNode = ps;

            while (1)
            {
                if (sXMSkipIterator.psList->Count < 2) break;
                if (sXMSkipIterator.psList->Up && sXMSkipIterator.psList->Up->Count > sXMSkipIterator.psList->Count / 2) break;
                if (!LevelUp()) break;

                while (sXMSkipIterator.psNode->Last)
                {
                    sXMSkipIterator.psNode = sXMSkipIterator.psNode->Last;
                    if(sXMSkipIterator.psNode == sXMSkipIterator.psList->Head && !sXMSkipIterator.psList->Up)
                    {
                        pList_s psList = ListInsertUp(sXMSkipIterator.psList);
                        if(!psList) { rv = -1; break;}

                        psXMSkipList->Count++;
                        psXMSkipList->psList = psList;
                    }
                    if(sXMSkipIterator.psNode->Up)
                    {
                        sXMSkipIterator.psNode = sXMSkipIterator.psNode->Up;
                        sXMSkipIterator.psList = sXMSkipIterator.psList->Up;
                        break;
                    }
                }

                if(rv < 0) break;
                pNode_s psUp = NodePoolGet();
                ps->Up = psUp;
                psUp->Dn = ps;
                psUp->Key = ps->Key;
                psUp->Value = ps->Value;
                NodeInsertNext(sXMSkipIterator.psNode, psUp);
                sXMSkipIterator.psList->Count++;
                ps = psUp;
                sXMSkipIterator.psNode = ps;
            }
            rv = 0;
        }
        else 
        {
            if(psXMSkipList->Free) psXMSkipList->Free(sXMSkipIterator.psNode->Next->Value->Value);
            if(psXMSkipList->Copy)
                sXMSkipIterator.psNode->Next->Value->Value = psXMSkipList->Copy(Value);
            else
                sXMSkipIterator.psNode->Next->Value->Value = Value;
        }
    }while(0);
    }
    pthread_rwlock_unlock(&psXMSkipList->RWLock);

    return rv ;
}

void *XMSkipListSelect(pXMSkipList_s psXMkipList, const char *Key)
{
    if(!psXMkipList || !Key) return NULL;
    int rv = 0;

    pthread_rwlock_rdlock(&psXMkipList->RWLock);

    XMSkipIterator_s sXMSkipIterator = XMSkipIteratorInit(psXMkipList);

    void *Result = NULL;
    do
    {
        rv = FindLast(&sXMSkipIterator,Key);
        if(rv == 0)
        {
            if(IsOverTime(sXMSkipIterator.psNode->Next->Value->OverTime)) break;
            if(psXMkipList->Copy && sXMSkipIterator.psNode->Next->Value->Value) Result = psXMkipList->Copy(sXMSkipIterator.psNode->Next->Value->Value);
            else Result = sXMSkipIterator.psNode->Next->Value->Value;
        }
    } while (0);

    pthread_rwlock_unlock(&psXMkipList->RWLock);

    return Result;
}

void XMSkipListDelete(pXMSkipList_s psXMSkipList, const char *Key)
{
    if(!psXMSkipList || !Key) return ;

    pthread_rwlock_wrlock(&psXMSkipList->RWLock);
    
    int rv = 0;
    {
    do
    {
        XMSkipIterator_s sXMSkipIterator = XMSkipIteratorInit(psXMSkipList);

        rv = FindLast(&sXMSkipIterator, Key);
        if(rv) break;

        sXMSkipIterator.psNode->Next->Value->OverTime = -1;
    } while (0);
    }

    pthread_rwlock_unlock(&psXMSkipList->RWLock);

    return ;
}

void XMSkipListShow(pXMSkipList_s psXMSkipList)
{
    if(!psXMSkipList) return;
    pList_s psList = psXMSkipList->psList;
    pNode_s psNode = NULL;

    int Listcnt = 0;

    while (psList)
    {
        printf("List%d:\n", Listcnt++);
        psNode = psList->Head;
        while (psNode->Next)
        {
            printf("%s = %s\n",psNode->Next->Key, psNode->Next->Value->Value);
            psNode = psNode->Next;
        }
        psList = psList->Dn;
    }
    return;
}

pXMSkipList_f XMSkipListFunInit()
{
    pXMSkipList_f pf = (pXMSkipList_f)malloc(sizeof(XMSkipList_f));
    if(!pf) return NULL;
    pf->Init = XMSkipListInit;
    pf->Destroy = XMSkipListDestroy;
    pf->Insert = XMSkipListInsert;
    pf->Select = XMSkipListSelect;
    pf->Delete = XMSkipListDelete;
    return pf;
}

void XMSkipListFunDestroy(pXMSkipList_f pfXMSkipList)
{
    if(!pfXMSkipList) return;
    free(pfXMSkipList);
    return;
}