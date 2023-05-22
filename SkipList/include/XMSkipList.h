#ifndef __XMSkipList_H__
#define __XMSkipList_H__

#ifdef __cplusplus
extern"C"
{
#endif


typedef struct XMSkipListParam_S
{
    void (*Free)(void *Param);
    void *(*Copy)(const void *Param);
    int CleanInterval;
}XMSkipListParam_s, *pXMSkipListParam_s;

pXMSkipList_s XMSkipListInit(const XMSkipListParam_s sParam);

void XMSkipListDestroy(pXMSkipList_s psXMSkipList);

int XMSkipListInsert(pXMSkipList_s psXMSkipList, const char *Key, void *Value, int OverTime);

void *XMSkipListSelect(pXMSkipList_s psXMSkipList, const char *Key);

void XMSkipListDelete(pXMSkipList_s psXMSkipList, const char *Key);

void XMSkipListShow(pXMSkipList_s psXMSkipList);

void NodePoolDestroy();

typedef  struct  XMSkipList_F
{
    pXMSkipList_s (*Init)(const XMSkipListParam_s sParam);
    void (*Destroy)(pXMSkipList_s psXMSkipList);
    int (*Insert)(pXMSkipList_s psXMSkipList,const char *Key,void *Value, int OverTime);
    void *(*Select)(pXMSkipList_s psXMSkipList , const char *Key);
    void (*Delete)(pXMSkipList_s psXMSkipList, const char *Key);
}XMSkipList_f, *pXMSkipList_f;

pXMSkipList_f XMSkipListFunInit();
void XMSkipListFunDestroy(pXMSkipList_f pfXMSkipList);

#ifdef __cplusplus
}
#endif

#endif