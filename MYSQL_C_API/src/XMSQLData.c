#include "XMSQLData.h"

static void Destroy(pXMSQLData_s ps)
{
    if(!ps) return;
    pXMSQLData_s Tmp = NULL;

    XMSQLDataEach(Tmp, ps)
    {
        if(Tmp->Name) free(Tmp->Name);
        if(Tmp->Value) free(Tmp->Value);
    }
    free(ps);
    return ;
}

static pXMSQLData_s Clone(pXMSQLData_s ps)
{
    int XMSQLDataSize = 0;
    pXMSQLData_s Tmp = NULL;
    XMSQLDataEach(Tmp, ps) XMSQLDataSize++;

    pXMSQLData_s _ps = (pXMSQLData_s)malloc(sizeof(XMSQLDataSize) * (XMSQLDataSize + 1));
    if(!_ps) NULL;
    memset(_ps, 0 ,sizeof(XMSQLData_s) * (XMSQLDataSize + 1));

    int i = 0;
    XMSQLDataEach(Tmp, ps)
    {
        if(Tmp->Name) (_ps + i)->Name = strdup(Tmp->Name);
        if(Tmp->Value)
        {
            if(Tmp->ValueType == XMSQL_INT)
            {
                (_ps + i)->Value = (int *)malloc(sizeof(int));
                memcpy((_ps + i)->Value, Tmp->Value, sizeof(int));
            }
            else if(Tmp->ValueType == XMSQL_CHARS)
            {
                (_ps + i)->Value = strdup((char *)Tmp->Value);
            }
        }
        (_ps + i)->ValueType = Tmp->ValueType;
        i++;
    }
    return _ps;
}

static pXMSQLData_l ListInit()
{
    pXMSQLData_l pl = (pXMSQLData_l)malloc(sizeof(XMSQLData_l));
    if(!pl) return NULL;

    pl->Data = NULL;
    pl->pNext = NULL;

    return pl;
}

int XMSQLDataListAdd(pXMSQLData_l *ppl, pXMSQLData_s ps)
{
    if(!ppl || !ps) return -1;

    pXMSQLData_l _pl = ListInit();
    if(!_pl) return -1;

    _pl->Data = Clone(ps);

    _pl->pNext = *ppl;

    *ppl = _pl;

    return 0;
}

void XMSQLDataListDestroy(pXMSQLData_l pl)
{
    if(!pl)  return;

    pXMSQLData_l _pl = pl;

    while (pl)
    {
        _pl = pl->pNext;
        if(pl->Data) Destroy(pl->Data);
        free(pl);
        pl = _pl;
    }
    return;
}

pXMSQLData_f XMSQLDataFunInit()
{
    pXMSQLData_f pf = (pXMSQLData_f)malloc(sizeof(XMSQLData_f));
    if(!pf) return NULL;

    pf->ListAdd = XMSQLDataListAdd;
    pf->ListDestroy = XMSQLDataListDestroy;

    return pf;
}


void XMSQLDataFunDestroy(pXMSQLData_f pf)
{
    if(!pf) return;

    free(pf);

    return;
}