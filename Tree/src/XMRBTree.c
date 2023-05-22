#include "XMRBTree.h"

#include "stdio.h"

#define Black 0
#define Red 1

typedef struct XMRBTree_Node
{
    char RB:1;
    void *Data;
    struct XMRBTree_Node *Left , *Right , *Father;    
}Node ,*pNode;

struct  XMRBTree_S
{
    int (*Compare)(void *, void *);
    void (*Free)(void *);
    struct XMRBTree_Node *RootNode;
};

int XMRBTreeNum = 0;
pNode FreeNodeS = NULL , FreeNodeE = NULL;

static pNode NodeGet()
{
    pNode hpNode = NULL;
    if ( FreeNodeS == FreeNodeE)
    {
        int i = 0;
        for (i = 0; i < 512 ; i++)
        {
            hpNode = (pNode)malloc(sizeof(Node));

            hpNode->RB = 0;
            hpNode->Data = NULL;
            hpNode->Left = NULL;
            hpNode->Right = NULL;
            hpNode->Father = NULL;

            FreeNodeE->Father = hpNode;
            FreeNodeE = FreeNodeE->Father;
        }
    }
    hpNode = FreeNodeS;
    FreeNodeS = hpNode->Father;
    hpNode->Father = NULL;
    return hpNode;
}

static void NodeReturn(const pNode hpNode)
{
    if(!hpNode) return ;
    hpNode->RB = 0;
    hpNode->Data = NULL;
    hpNode->Father = NULL;
    hpNode->Left = NULL;
    hpNode->Right = NULL;

    FreeNodeE->Father = hpNode;
    FreeNodeE = FreeNodeE->Father;
}

#define LeftRotate(hpNode) Rotate(hpNode , 0)
#define RightRotate(hpNode) Rotate(hpNode , 1)
static void Rotate(pNode hpNode,const int Type)
{
    pNode _hpNode = NULL;
    if (!Type)
    {
        _hpNode = hpNode->Right;
        _hpNode->Father = hpNode->Father;
        hpNode->Father = _hpNode;
        hpNode->Right = _hpNode->Left;
        _hpNode->Left = hpNode;
    }
    else 
    {
        _hpNode = hpNode->Left;
        _hpNode->Father = hpNode->Father;
        hpNode->Father = _hpNode;
        hpNode->Left = _hpNode->Right;
        _hpNode->Right = hpNode;
    }
}

static pXMRBTree_s Init(int (*Compare)(void *, void *), void (*Free)(void *))
{
    if(!Compare || !Free) return NULL;

    XMRBTreeNum++;
    if(!FreeNodeS && !FreeNodeE)
    {
        FreeNodeS = (pNode)malloc(sizeof(Node));
        FreeNodeE = FreeNodeS;
    }

    pXMRBTree_s hpXMRBTree = (pXMRBTree_s)malloc(sizeof(XMRBTree_s));
    hpXMRBTree->RootNode = NULL;
    hpXMRBTree->Compare = Compare;
    hpXMRBTree->Free = Free;

    return hpXMRBTree;
}

static void Destroy(pXMRBTree_s hpXMRBTree)
{
    if (XMRBTreeNum)
    {
        XMRBTreeNum--;
        pNode hpNode = NULL;
        while (FreeNodeS)
        {
            hpNode = FreeNodeS;
            FreeNodeS = FreeNodeS->Father;
            free(hpNode);
            hpNode = NULL;
        }
        FreeNodeE = FreeNodeS;
    }
}

static void InsterFixUP(pNode hpNode)
{
    pNode Now = hpNode, Father = hpNode->Father , GrandFather = NULL , Uncle = NULL;
    if (!Father)
    {
        Now->RB = Black;
    }
    else if (Father->RB) 
    {
        GrandFather = Father->Father;
        if ( Father = GrandFather->Left) Uncle = GrandFather->Right;
        else Uncle = GrandFather->Left;

        if ( Uncle->RB)
        {
            Father->RB = Black;
            Uncle->RB = Black;
            GrandFather->RB = Red;
            InsterFixUP(GrandFather);
        }
        else 
        {
            if ( Now == Father->Right)
            {
                LeftRotate(Father);
                InsterFixUP(Father);
            }
            else 
            {
                Father->RB = Black;
                GrandFather = Red;
                RightRotate(GrandFather);
            }
        }
    }
    return ;
}

static void Inster(pXMRBTree_s hpXMRBTree , void *Data)
{
    pNode hpNode = hpXMRBTree->RootNode , Father = NULL;
    while (hpNode)
    {
        Father = hpNode;
        if (hpXMRBTree->Compare(hpNode->Data , Data) < 0) hpNode = hpNode->Left;
        else hpNode = hpNode->Right;
    }
    hpNode = NodeGet();
    hpNode->Data = Data;
    hpNode->Father = Father;
    hpNode->Left = NULL;
    hpNode->Right = NULL;
    hpNode->RB = Red;

    InsterFixUP(hpNode);
}

static void Select(pXMRBTree_s hpXMRBTree_s , void* Data)
{

}

static void DeleteFixUP(const pNode hpNode)
{
    if ( !hpNode ) return ;
    if (hpNode->RB)
    {
        hpNode->RB = Black;
        return;
    }
    pNode Father = hpNode->Father , Brother = NULL;
    if (hpNode == Father->Left )
    {
        Brother = Father->Right;
        if ( Brother->RB)
        {
            Brother->RB = Black;
            Father->RB = Red;
            LeftRotate(Father);
            DeleteFixUP(hpNode);
        }
        else
        {
            if(!Brother->Left->RB && !Brother->Right->RB)
            {
                Brother->RB = Red;
                DeleteFixUP(Father);
            }
            else if (Brother->Left->RB &&!Brother->Right->RB)
            {
                Brother->Left->RB = Black;
                Brother->RB = Red;
                RightRotate(Brother);
                DeleteFixUP(hpNode);
            }
            else if(Brother->Right->RB)
            {
                Brother->RB = hpNode->Father;
                hpNode->Father = Black;
                Brother->Right->RB = Black;
            }
        }
    }
    else 
    {
        Brother = Father->Left;
        if (Brother->RB)
        {
            Brother->RB = Black;
            Father->RB = Red;
            RightRotate(Father);
            DeleteFixUP(hpNode);
        }
        else 
        {
            if (!Brother->Right->RB && !Brother->Left->RB)
            {
                Brother->RB= Red;
                DeleteFixUP(Father);
            }
            else if(Brother->Right->RB && !Brother->Left->RB)
            {
                Brother->Right->RB = Black;
                Brother->RB = Red;
                LeftRotate(Brother);
                DeleteFixUP(hpNode);
            }
            else if (Brother->Left->RB)
            {
                Brother->RB = hpNode->Father;
            }
        }
    }
}

static void Delete(pXMRBTree_s hpXMRBTree ,void* Data)
{
    pNode hpNode = hpXMRBTree;

    while (hpNode)
    {
        if(hpXMRBTree->Compare(hpNode->Data , Data) < 0) hpNode = hpNode->Left;
        else if(hpXMRBTree->Compare(hpNode->Data, Data) > 0) hpNode = hpNode->Right;
        else break;
    }
    if(!hpNode) return;

    hpXMRBTree->Free(hpNode->Data);

    pNode hpNode_Del = NULL , hpNode_Child = NULL;

    if (!hpNode->Left || !hpNode->Right ) hpNode_Del = hpNode;
    else 
    {
        hpNode_Del = hpNode->Right;
        while (hpNode_Del->Left)
        {
            hpNode_Del = hpNode_Del->Left;
        }
    }
    if ( hpNode_Del->Left) hpNode_Child = hpNode_Del->Left;
    else hpNode_Child = hpNode_Del->Right;

    if(hpNode_Child) hpNode_Child->Father = hpNode_Del->Father;
    if (!hpNode_Del->Father) hpXMRBTree->RootNode = hpNode_Child;
    else if(hpNode_Del->Father->Left == hpNode_Del) hpNode_Del->Father->Left = hpNode_Child;
    else hpNode_Del->Father->Right = hpNode_Child;

    if(hpNode_Del != hpNode) hpNode->Data = hpNode_Del->Data;
    if(hpNode_Del != hpXMRBTree->RootNode && !hpNode_Del->RB) DeleteFixUP(hpNode_Child);
    NodeReturn(hpNode_Del);

    hpXMRBTree->RootNode->RB = Black;
    
}

static void Foreach(pXMRBTree_s hpXMRBTree , void (*Operater)(void *));