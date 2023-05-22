#ifndef __XMSQL_H__
#define __XMSQL_H__

#include "XMSQLPublic.h"

#ifdef XMSQL_SQLITE
#define XMSQL_CHARS SQLITE_TEXT
#define XMSQL_INT SQLITE_INTEGER
#else
#define XMSQL_CHARS MYSQL_TYPE_STRING
#define XMSQL_INT MYSQL_TYPE_LONG
#endif 

#define XMSQLDataEach(Item,Block) \
    for(Item = Block; Item->Name || Item->Value || Item->ValueType; Item++)

/**
 * @brief XMSQLData基本数据结构，二维数组以{0,0,0}结尾
 * @param Name 列名
 * @param Value 列值
 * @param ValueType 列值类型
 * @example XMSQLData_s ps[3] {{"ID", &[1], XMSQL_INT}, {"Name", "abc", XMSQL_String} {0,0,0}}
 * @example pXMSQLData_s ps = (pXMSQLData_s)malloc(sizeof(XMSQLData_s) * (2 + 1));
*/

typedef struct XMSQLData_S
{
    char *Name;
    void *Value;
    int ValueType;
}XMSQLData_s, *pXMSQLData_s;

/**
 * @brief XMSQLData数据链结构
 * @param Data XMSQL数据块
 * @param pNext 链后继
 * @example pXMSQLData_ pl = NULL
*/
typedef struct XMSQLData_L
{
    pXMSQLData_s Data;
    struct XMSQLData_L *pNext;
}XMSQLData_l, *pXMSQLData_l;

/**
 * @brief 销毁数据链，释放空间
 * 
 * @param pl 数据链指针
*/
void XMSQLDataListDestroy(pXMSQLData_l pl);

/**
 * @brief 链表添加数据
 * 
 * @param pl 数据链地址指针
 * @param ps 待插入数据指针
 * @return int 错误码
*/
int XMSQLDataListAdd(pXMSQLData_l *ppl, pXMSQLData_s ps);

/**
 * @brief XMSQLData函数库
 * 
 * @example pXMSQLData_f fp = NULL;
*/
typedef struct XMSQLData_F
{
    int (*ListAdd)(pXMSQLData_l *pl ,pXMSQLData_s ps);

    void (*ListDestroy)(pXMSQLData_l pl);
}XMSQLData_f, *pXMSQLData_f;

/**
 * @brief XMSQLData数据库初始化
 * 
 * @return pXMQSLData_f XMSQLData函数库句柄
*/
pXMSQLData_f XMSQLDataFunInit();

/**
 * @brief XMSQLData函数库初始化销毁
 * 
 * @param pf XMSQLData函数库句柄
*/
void XMSQLDataFunDestroy(pXMSQLData_f pf);

#endif