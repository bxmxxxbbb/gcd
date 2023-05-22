#ifndef __XMMYSQL_H__
#define __XMMYSQL_H__

#include "XMSQLPublic.h"
#include "XMSQLData.h"

#include "XMParamReader.h"

#define XMSQL_OK 0

/**
 * @brief XMSQL数据库链接句柄
 * @param pMySQL MySQL句柄
 * @param DBName 数据库名
 * @example pXMSQL_s ps = NULL;
*/
typedef struct  XMSQL_S
{
    #ifdef XMSQL_SQLITE
    sqlite3 *pSQL;
    #else
    MYSQL *pSQL;
    char *DBName;
    #endif
}XMSQL_s, *pXMSQL_s;

#ifndef XMSQL_SQLITE
/**
 * @brief 句柄初始化
 * 
 * @param DBHost 数据库地址
 * @param Username 用户名
 * @param Password 密码
 * @param DBName 数据库名
 * @return pXMSQL_s XMSQL数据库连接句柄
*/
pXMSQL_s XMSQLInitByParam(const char *Host, const int Port, const char *Username, const char *Password, const char *DBName);

#endif

/**
 * @brief 
 * 
 * @param FilePath
 * @return pXMSQL_s
*/
pXMSQL_s XMSQLInit(const char *FilePath);

/**
 * @brief 句柄销毁
 * 
 * @param psXMSQL XMSQL数据库链接句柄
*/
void XMSQLDestroy(pXMSQL_s ps);

/**
 * @brief 
 * 
 * @param psXMSQL
 * @param SQLBuf
 * @param Cond
 * @param Data
 * @param pDataList
 * @return int 
*/
int XMSQLEXEC(pXMSQL_s psXMSQL,const char *SQLBuf, pXMSQLData_s Cond, pXMSQLData_s Data,pXMSQLData_l *pDataList);

/**
 * @brief 数据库插入操作
 * 
 * @param psXMSQL XMSQL数据库连接句柄
 * @param TableName 表名
 * @param Data XMSQLData句柄(数据)
 * @return int 错误码
*/
int XMSQLInster(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Data);

/**
 * @brief 数据库更新操作
 * 
 * @param psXMSQL XMSQL数据库连接句柄
 * @param TableName 表名
 * @param Cond XMSQLData句柄(条件)
 * @param Data XMSQLData句柄(数据)
 * @return int 错误码
*/
int XMSQLUpdata(pXMSQL_s psXMSQL, const char *TableName,pXMSQLData_s Cond,pXMSQLData_s Data);

/**
 * @brief 数据库查询操作
 * 
 * @param psXMSQL XMSQL数据库连接句柄
 * @param TableName 表名
 * @param Cond XMSQLData句柄(条件)
 * @param Data XMSQLData句柄(数据)
 * @param pDataList XMSQLData数据链结构句柄地址指针，返回查询结果
 * @return int 错误码
*/
int XMSQLSelect(pXMSQL_s psXMQL, const char *TableName, pXMSQLData_s Cond,pXMSQLData_s Data, pXMSQLData_l *pDataList);

/**
 * @brief 数据库删除操作
 * 
 * @param psXMSQL XMSQL数据库连接句柄
 * @param TableName 表名
 * @param Cond XMSQLData句柄(条件)
 * @return int 错误码
*/
int  XMSQLDelete(pXMSQL_s psXMSQL,const char *TableName, pXMSQLData_s Cond);

/**
 * @brief 数据库更新或删除操作
 * 
 * @param psXMSQL XMSQL数据库连接句柄
 * @param Cond XMSQLData句柄(条件)
 * @param Data XMSQLData句柄(数据)
 * @return int 错误码
*/
int XMSQLUpdataForce(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond, pXMSQLData_s Data);

/**
 * @brief XMSQL函数库
 * @example pXMSQL_f pf = NULL
*/
typedef struct  XMSQL_F
{
    #ifdef XMSQL_SQLITE
    /**
     * @brief 句柄初始化
     * 
     * @param FileName 数据库路径
     * @param DBName 数据库名
     * @return pXMSQL_s XMSQL数据库连接句柄 
    */
   pXMSQL_s (*Init)(const char *FilePath);
   #else
   pXMSQL_s (*Init)(const char *FilePath);
   
   pXMSQL_s (*InitByParam)(const char *DBHost , const int Port,const char *Username, const char *Passord,const char *DBName);
   #endif

   void (*Destroy)(pXMSQL_s psXMSQL);

   int (*EXEC)(pXMSQL_s psXMSQL,const char *SQLBuf,pXMSQLData_s Cond, pXMSQLData_s Data,pXMSQLData_l *pDataList);

   int (*Inster)(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Data);

   int (*Update)(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond, pXMSQLData_s Data);

   int (*Select)(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond, pXMSQLData_s Data, pXMSQLData_l *pDataList);

   int (*Delete)(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond);

   int (*UpdateForce)(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond, pXMSQLData_s Data);

    //XMSQLData函数库
    pXMSQLData_f pfXMSQLData;
}XMSQL_f, *pXMSQL_f;

/**
 * @return pXMSQL_f XMSQL函数库句柄
*/
pXMSQL_f XMSQLFunInit();

/**
 * @brief XMSQL数据库销毁
 * 
 * @param pf XMSQL函数库句柄
*/
void XMSQLFunDestroy(pXMSQL_f pf);

#endif