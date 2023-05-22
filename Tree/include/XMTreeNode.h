#ifndef __XMTREENODE_H__
#define __XMTREENODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define CP printf("CheckPoint[%s]:%d\n",__FILE__,__LINE__);

typedef struct XMTreeNode_S
{
    int Count;
    int Color;
    struct XMTreeNode_S *Last,*Left,*Right;
    void *Data;
}XMTreeNode_s, *pXMTreeNode_s;

typedef struct XMTreeNodePool_L
{
    int Count;
    pXMTreeNode_s psNode;
    pthread_mutex_t Locker;
}XMTreeNodePool_l, *pXMTreeNodePool_l;

static inline pXMTreeNodePool_l XMTreeNodePoolNew()
{
    pXMTreeNodePool_l pl = (pXMTreeNodePool_l)malloc(sizeof(XMTreeNodePool_l));
    if(!pl) return NULL;

    pl->Count = 1;
    pl->psNode = (pXMTreeNode_s)malloc(sizeof(XMTreeNode_s));
    pl->psNode->Count = 0;
    pl->psNode->Color = 0;
    pl->psNode->Last = NULL;
    pl->psNode->Data = NULL;
    pl->psNode->Left = NULL;
    pl->psNode->Right = NULL;
    pthread_mutex_init(&pl->Locker,NULL);

    return pl;
}

static inline void XMTreeNodePoolDestroy(pXMTreeNodePool_l pl)
{
    if (!pl) return ;

    pXMTreeNode_s ps = NULL;
    while(pl->psNode)
    {
        ps  = pl->psNode;
        pl->psNode = pl->psNode->Last;
        free(ps);
    }

    pthread_mutex_destroy(&pl->Locker);
    free(pl);
    return ;
}

static inline pXMTreeNode_s XMTreeNodePoolGet(pXMTreeNodePool_l pl)
{
    pXMTreeNode_s ps = NULL;

    pthread_mutex_lock(&pl->Locker);

    if (!pl->psNode)
    {
        int i = 0;
        if (pl->Count <1 << 16)pl->Count <<= 1;
        while(i++ < pl->Count)
        {
            ps = (pXMTreeNode_s)malloc(sizeof(XMTreeNode_s));
            ps->Count = 0;
            ps->Color = 0;
            ps->Data = NULL;
            ps->Last = pl->psNode;
            ps->Left = NULL;
            ps->Right = NULL;
            pl->psNode = ps;
        }
    }

    ps = pl->psNode;
    pl->psNode = pl->psNode->Last;
    ps->Last = NULL;

    pthread_mutex_unlock(&pl->Locker);

    return ps;
}

static inline void XMTreeNodePoolReturn(pXMTreeNodePool_l pl, pXMTreeNode_s ps)
{
    if (!pl || !ps) return ;

    pthread_mutex_lock(&pl->Locker);

    ps->Last = pl->psNode;
    ps->Left = NULL;
    ps->Right = NULL;
    pl->psNode = ps;

    pthread_mutex_unlock(&pl->Locker);

    return ;
}

#define XMTreeNodeIsNULL(ps) (!(ps)->Color)

static inline void LeftRotate(pXMTreeNode_s ps)
{
    if (!ps) return ;

    pXMTreeNode_s Now = ps , Last = Now->Last , Left = Now->Left , Right = Now->Right;

    if (!Right) return ;

    if (Now->Last )
    {
        if(Now == Last->Right)
            Last->Right = Right;
        else 
            Last->Left = Right;
    }
    Right->Last = Now->Last;

    if(Right->Left)
    {
        Right->Left->Last = Now;
        Now->Right = Right->Left;
    }

    Right->Left = Now;
    Now->Last = Right;

    return ;
}

static inline void RightRotate(pXMTreeNode_s ps)
{
    if(!ps) return;

    pXMTreeNode_s Now = ps , Last = Now->Last, Left = Now->Left , Right = Now->Right;

    if(!Left) return;

    if(Now->Last)
    {
        if(Now == Last->Right)
            Last->Right = Left;
        else 
            Last->Left = Left;
    }
    Left->Last = Now->Last;

    if(Left->Right)
    {
        Left->Right->Last = Now;
        Now->Left = Left->Right;
    }

    Left->Right = Now;
    Now->Last = Left;

    return ;
}

typedef struct  XMTree_L
{
    short Count;
    pXMTreeNodePool_l plNode;
    pXMTreeNode_s psNode;
    int (*Compare)(void * DataA, void *DataB);
    void (*Free)(void *Data);
    pthread_mutex_t Locker; 
}XMTree_l, *pXMTree_l;


typedef struct XMTreeNode_S XMTreeIterator_s, *pXMTreeIterator_s;

static inline pXMTree_l XMTreeNew(int Compare(void *, void *) , void Free(void *Data))
{
    if ( !Compare) return NULL;

    pXMTree_l pl = (pXMTree_l)malloc(sizeof(XMTree_l));
    if(!pl) return NULL;

    pl->Count = 0;
    pl->plNode = XMTreeNodeNew();
    pl->psNode = GetXMTreeNode(pl->plNode);

    pl->Compare = Compare;
    pl->Free = Free;

    pthread_mutex_init(&pl->Locker , NULL);

    return pl;
}

static inline void XMTreeDestroy(pXMTree_l pl)
{
    if ( !pl ) return ;

    pXMTreeNode_s psNode = pl->psNode;
    pXMTreeNode_s psNodeTmp = NULL;
    while(psNode)
    {
        if (psNode->Left)
            psNode = psNode->Left;
        else if (psNode->Right)
            psNode = psNode->Right;
        else 
        {
            if (psNode->Data && pl->Free) pl->Free(psNode->Data);
            if(psNode->Last)
            {
                if(psNode->Last->Right == psNode)
                    psNode->Last->Right = NULL;
                else 
                    psNode->Last->Left = NULL;
            }
            psNodeTmp = psNode;
            psNode = psNode->Last;
            ReturnXMTreeNode(pl->plNode, psNodeTmp);
        }
    }
    XMTreeNodeDestroy(pl->plNode);

    pthread_mutex_destroy(&pl->Locker);

    free(pl);

    return;
}

static inline pXMTreeIterator_s XMTreeEach(pXMTree_l pl, pXMTreeIterator_s ps)
{
    if(!pl) return NULL;

    pXMTreeNode_s psNode =NULL;

    if(ps)
    {
        if(!XMTreeNodeIsNULL(ps->Right))
        {
            psNode = ps->Right;
        }
        else 
        {
            while (ps->Last && ps->Last->Right == ps)
            {
                ps = ps->Last;
            }
            return ps->Last;   
        }
    }
    else 
        psNode = pl->psNode;
    while (!XMTreeNodeIsNULL(psNode))
    {
        psNode = psNode->Left;
    }
    return psNode;    
}

static inline pXMTreeIterator_s XMTreeReverseEach(pXMTree_l pl, pXMTreeIterator_s ps)
{
    if (!pl || !pl->psNode->Color) return NULL;
    pXMTreeNode_s psNode = NULL;
    if (ps)
    {
        if (!XMTreeNodeIsNULL(ps->Left))
        {
            psNode = ps->Left;
        }
        else 
        {
            while (ps->Last && ps->Last->Left == ps)
                ps = ps->Last;
            return ps->Last;
        }
    }
    else psNode = pl->psNode;

    while (!XMTreeNodeIsNULL(psNode->Right)) psNode = psNode->Right;

    return psNode;
}

#endif