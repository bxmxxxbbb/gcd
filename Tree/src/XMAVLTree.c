#include "XMAVLTree.h"


static void XMAVLTreeUpdata(pXMTreeNode_s psNode)
{
    if(!psNode) return ;

    while (psNode)
    {
        if(abs(psNode->Left->Color - psNode->Right->Color) > 1)
        {
            if(psNode->Left->Color > psNode->Right->Color)
            {
                if(psNode->Left->Color && psNode->Left->Right->Color > psNode->Left->Left->Color)
                {
                    psNode = psNode->Left;
                    LeftRotate(psNode);
                }
                else 
                    RightRotate(psNode);
            }
            else 
            {
                if(psNode->Right->Color && psNode->Right->Left->Color > psNode->Right->Right->Color)
                {
                    psNode = psNode->Right;
                    RightRotate(psNode);
                }
                else 
                    LeftRotate(psNode);
            }
        }
        psNode->Color = 1;
        psNode->Color += psNode->Left->Color > psNode->Right->Color ? psNode->Left->Color : psNode->Right->Color;

        psNode = psNode->Last;
    }
    return;   
}

int XMAVLTreePush(pXMAVLTree_l  pl, void *Data)
{
    if (!pl) return -1;

    pXMTreeNode_s psNode = pl->psNode;

    while (psNode)
    {
        if (!psNode->Color)
        {
            psNode->Data = Data;
            psNode->Left = GetXMTreeNode(pl->plNode);
            psNode->Left->Last = psNode;
            psNode->Right = GetXMTreeNode(pl->plNode);
            psNode->Right->Last = psNode;

            // if(psNode->Last && psNode->Last->Last)
            // {
            //     if(psNode->Last->Right == psNode && psNode->Last->Last->Left == psNode->Last)
            //     {
            //         LeftRotate(psNode->Last);
            //         psNode = psNode->Left;
            //     }
            //     else if(psNode->Last->Left == psNode && psNode->Last->Last->Right == psNode->Last)
            //     {
            //         RightRotate(psNode->Last);
            //         psNode = psNode->Right;
            //     }
            // }
            XMAVLTreeUpdata(psNode);

            while (psNode->Last) psNode = psNode->Last;
            pl->psNode = psNode;
            
            break;
        } 
        if(pl->Compare(Data , psNode->Data) >= 0)
            psNode = psNode->Right;
        else 
            psNode = psNode->Left;
    }
    return 0;
}

pXMTreeIterator_s XMAVLTreeFind(pXMAVLTree_l pl , void *Data)
{
    if ( !pl || !Data) return NULL;
    if (pl->Compare(Data , XMTreeReverseEach(pl , NULL)) > 0) return NULL;
    if (pl->Compare(Data , XMTreeEach(pl , NULL)) < 0 ) return NULL;

    pXMTreeNode_s psNode = pl->psNode;

    int M = 0;
    while (psNode->Color)
    {
        M = pl->Compare(Data , psNode->Data);
        if(M > 0)
        {
            if (!psNode->Right->Color) break;
            psNode = psNode->Right;
        }
        else if(M < 0)
        {
            if(!psNode->Left->Color) break;
            psNode = psNode->Left;
        }
        else 
        {
            break;
        }
    }
    psNode->Count++;
    return psNode;
}

pXMTreeIterator_s XMAVLTreeErase(pXMAVLTree_l pl , pXMTreeIterator_s ps)
{
    if(!pl || !ps) return NULL;

    pXMTreeNode_s psNext = NULL;

    if(ps->Right->Color)
    {
        psNext = ps->Right;
        while (psNext->Left->Color) psNext = psNext->Left;
        
        psNext->Right->Last = psNext->Last;
        if(psNext->Last)
        {
            if(psNext->Last->Left == psNext)
                psNext->Last->Left = psNext->Right;
            else 
                psNext->Last->Right = psNext->Right;
        }

        if(ps->Data && pl->Free) pl->Free(ps->Data);
        ps->Data = psNext->Data;

        XMAVLTreeUpdata(psNext->Last);


        ReturnXMTreeNode(pl->plNode , psNext->Left);
        ReturnXMTreeNode(pl->plNode, psNext);

        pl->psNode = ps;
        while(pl->psNode->Last) pl->psNode = pl->psNode->Last;
        
        ps = XMTreeReverseEach(pl ,ps);
    }
    else 
    {
        ps->Left->Last = ps->Last;
        if(ps->Last)
        {
            if(ps->Last->Left == ps)
                ps->Last->Left = ps->Left;
            else 
                ps->Last->Right = ps->Left;
        }

        XMAVLTreeUpdata(ps->Last);

        if( ps == pl->psNode) pl->psNode = ps->Left;

        pXMTreeNode_s psTmp = ps;
        ps = ps->Last;
        if(ps) ps = XMTreeReverseEach(pl ,ps);

        if(psTmp->Data && pl->Free) pl->Free(psTmp->Data);
        ReturnXMTreeNode(pl->plNode , psTmp->Right);
        ReturnXMTreeNode(pl->plNode , psTmp);
    }

    return ps;
}

void XMAVLTreeReoccupy(pXMAVLTree_l pl , pXMTreeIterator_s ps)
{
    if(!ps) return;
    ps->Count--;
    if(ps->Data) XMAVLTreeErase(pl,ps);

    return;
}

pXMAVLTree_f XMAVLTreeFunInit()
{
    pXMAVLTree_f pf = (pXMAVLTree_f)malloc(sizeof(XMAVLTree_f));
    if(!pf) return NULL;
    pf->New = XMTreeNew;
    pf->Destroy = XMTreeDestroy;
    pf->Each = XMTreeEach;
    pf->ReverseEach = XMTreeReverseEach;
    pf->Push = XMAVLTreePush;
    pf->Find = XMAVLTreeFind;
    pf->Erase = XMAVLTreeErase;
    pf->Reoccupy = XMAVLTreeReoccupy;

    return pf;
}

void XMAVLTreeFunDestroy(pXMAVLTree_f pf)
{
    if(!pf) return ;

    free(pf);
    return ;
}