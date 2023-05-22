#include "XMSQL.h"

#define CP \
    printf("%d\n",__LINE__);

pXMSQLData_f pfXMSQLData = NULL;

static int FunExist = 0;
static pthread_mutex_t FunExistLock = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t Locker = PTHREAD_MUTEX_INITIALIZER;
#define Lock pthread_mutex_lock(&Locker);
#define Unlock pthread_mutex_unlock(&Locker);

#define MAXLEN 1024 * 10

#ifdef XMSQL_LITE

static pXMSQL_s Init(const char *FilePath)
{
    if(!FilePath) return NULL;

    int rv = 0;

    pXMSQL_s ps = (pXMSQL_s)malloc(sizeof(XMSQL_s));

    if(!ps) return NULL;

    ps->pSQL = NULL;

    do
    {
        rv = sqlite3_open_v2(FilePath, &ps->pSQL, SQLITE_OPEN_READWRITE, NULL);
        if(rv < 0) {rv = -1; break;}
    }while(0);

    if(rv < 0)
    {
        if(ps->pSQL) sqlite3_close_v2(ps->pSQL);
        free(ps);
        return NULL;
    }
    return ps;
}

static void Destroy(pXMSQL_s ps)
{
    if(!ps) return ;

    sqlite3_close_v2(ps->pSQL);

    free(ps);

    return ;
}

static int XMSQLEXEC_EX_GetResult(sqlite3_stmt *pStmt, pXMSQLData_s Data, pXMSQLData_l *pDataList)
{
    if(!pStmt || !Data || !pDataList) return -1;

    int rv = -1;

    int RowNum = 0;
    pXMSQLData_s Tmp = NULL;

    int it = 0;
    int TmpInt = 0;
    do
    {
        while (1)
        {
            rv = sqlite3_step(pStmt);
            if(rv == SQLITE_BUSY) {usleep(100); continue;}
            else if(rv != SQLITE_ROW) break;

            it = 0;
            XMSQLDataEach(Tmp, Data)
            {
                if(Tmp->ValueType == XMSQL_CHARS)
                {
                        Data[it].Value = (char *)sqlite3_column_text(pStmt, it);
                }
                else if(Tmp->ValueType == XMSQL_INT)
                {
                    TmpInt = sqlite3_column_int(pStmt, it);
                    Data[it].Value = &TmpInt;
                }
                else 
                {
                    // printf("sqlite3_step unknown XMSQL_Type"); rv = -1; break;
                }
                it++;
            }
            XMSQLDataListAdd(pDataList, Data);
        }

    } while (0);
    if(rv == SQLITE_DONE) rv = 0;

    return rv ;
}

static int XMSQLEXEC(pXMSQL_s psXMSQL, const char *SQLBuf, pXMSQLData_s Cond,pXMSQLData_s Data, pXMSQLData_l *pDataList)
{
    if(!psXMSQL || !SQLBuf) return -1;

    int rv = 0;
    Lock
    {
    sqlite3_stmt *pStmt = NULL;
    const char *pzTail = NULL;
    do
    {
        rv = sqlite3_prepare_v2(psXMSQL->pSQL, SQLBuf, strlen(SQLBuf),&pStmt , &pzTail);
        if(rv != SQLITE_OK) {rv = -1; break;}

        int itBind = 0;
        pXMSQLData_s Tmp = NULL;
        if(Data)
        {
            XMSQLDataEach(Tmp, Data)
            {
                if(Tmp->Value == NULL ) continue;
                if(Tmp->ValueType == XMSQL_CHARS)
                {
                    sqlite3_bind_text(pStmt, ++itBind, (char *)Tmp->Value, -1, SQLITE_STATIC);
                }
                else if(Tmp->ValueType == XMSQL_INT)
                {
                    sqlite3_bind_int(pSTmt, ++itBind, *(int *)Tmp->Value);
                }
                else 
                {
                    // printf("Data unknow XMSQL_type");
                    // rv = -1;
                    // break;
                }
            }
        }
        if(Cond)
        {
            XMSQLDataEach(Tmp,Cond)
            {
                if(Tmp->Value == NULL) continue;
                if(Tmp->ValueType == XMSQL_CHARS)
                {
                    sqlite3_bind_text(pStmt, ++itBind, (char *)Tmp->Value, -1, SQLITE_STATIC);
                }
                else if(Tmp->ValueType == XMSQL_INT)
                {
                    sqlite3_bind_int(pStmt, ++itBind, *(int *)Tmp->Value);
                }
                else 
                {
                    // printf("Cond unknown XMSQLType");
                    // rv = -1;
                    // break;
                }
            }
        }

        if(pDataList)
        {
            rv = XMSQLEXEC_EX_GetResult(pStmt, Data, pDataList);
            if(rv) break;
        }
        else 
        {
            while ((rv = sqlite3_step(pStmt)) == SQLITE_BUSY) usleep(100);
            if(rv == SQLITE_DONE) rv = 0;
            
        }
        rv = 0;
    } while (0);
    if(pStmt) sqlite3_finalize(pStmt);
    }
    Unlock
    return rv ;
}

#else

pXMSQL_s XMSQLInitByParam(const char *Host, const int Port, const char *Username, const char *Password, const char *DBName)
{
    if(!Host || !Username || !Password || !DBName) return NULL;

    int rv = 0;

    MYSQL *pSQL = NULL;

    {
    do
    {
        pSQL = mysql_init(pSQL);
        if(!pSQL) {rv = -1; break;}

        int value = 1;
        mysql_options(pSQL , MYSQL_OPT_RECONNECT, &value);

        pSQL = mysql_real_connect(pSQL ,Host ,Username , Password , DBName , 0 , NULL , 0);
        if(!pSQL) { rv = -1; break;}
    } while (0);
    }

    if(rv)
    {
        if(pSQL) mysql_close(pSQL);
        return NULL;
    }

    pXMSQL_s ps = (pXMSQL_s)malloc(sizeof(XMSQL_s));
    ps->pSQL = pSQL;
    ps->DBName = strdup(DBName);

    return ps;
}

struct XMSQLConf_S
{
    char Host[20];
    char Port[20];
    char Username[20];
    char Password[20];
    char DBName[20];
};

pXMSQL_s XMSQLInit(const char *FilePath)
{
    if(!FilePath) return NULL;
    struct XMSQLConf_S sXMSQLConf;
    XMParamReader_s XMSQLConf[] = 
    {
        {"Host" , sXMSQLConf.Host , "Host[20]",XMParamReaderNULL},
        {"Port" , sXMSQLConf.Port , "Port[20]",XMParamReaderNULL},
        {"Username" , sXMSQLConf.Username , "Username[20]", XMParamReaderNULL},
        {"Password" , sXMSQLConf.Password , "Password[20]", XMParamReaderNULL},
        {"DBName" , sXMSQLConf.DBName , "DBName[20]", XMParamReaderNULL},
        {0,0,0,0}
    };

    pXMSQL_s psXMSQL = NULL;

    do
    {
        if(XMParamReaderByFile(XMSQLConf , FilePath) < 0)
        {
            XMParamReaderHelp(XMSQLConf);
            break;
        }
        psXMSQL = XMSQLInitByParam(sXMSQLConf.Host, atoi(sXMSQLConf.Port), sXMSQLConf.Username, sXMSQLConf.Password, sXMSQLConf.DBName);
        if(psXMSQL) break;
    }while(0);

    return psXMSQL;
}

void XMSQLDestroy(pXMSQL_s ps)
{
    if(!ps) return ;

    mysql_close(ps->pSQL);
    free(ps->DBName);

    free(ps);

    return ;
}

static int XMSQLEXEC_EX_GetResult(MYSQL_STMT *pStmt, pXMSQLData_s Data, pXMSQLData_l *pDataList)
{
    if(!pStmt || !Data || !pDataList) return -1;

    int rv = 0;

    {
    int RowCnt = 0;
    int itBindResult = 0;
    MYSQL_BIND *pBindResult = NULL;
    unsigned long *pBindResultLength = NULL;
    bool *pBindResultIsNull = NULL;
    pXMSQLData_s TmpData = NULL;
    do
    {
        pXMSQLData_s Tmp = NULL;
        XMSQLDataEach(Tmp, Data) RowCnt++;

        TmpData = (pXMSQLData_s)malloc(sizeof(XMSQLData_s) * (RowCnt + 1));
        if(!TmpData) {rv = -1; break;}
        memset(TmpData , 0 , sizeof(XMSQLData_s) * (RowCnt + 1));
        
        pBindResult = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * (RowCnt));
        if(!pBindResult) {rv = -1; break;}
        memset(pBindResult , 0 , sizeof(MYSQL_BIND) * (RowCnt));

        pBindResultLength = (unsigned long *)malloc(sizeof(unsigned long) * (RowCnt));
        if(!pBindResultLength) {rv = -1; break;}
        memset(pBindResultLength , 0 , sizeof(unsigned long) * (RowCnt));

        pBindResultIsNull = (bool *)malloc(sizeof(bool) * (RowCnt));
        if(!pBindResultIsNull) {rv = -1; break;}
        memset(pBindResultIsNull , 0 , sizeof(bool) * (RowCnt));

        XMSQLDataEach(Tmp, Data)
        {
            TmpData[itBindResult].Value = NULL;
            TmpData[itBindResult].Name = Tmp->Name;
            TmpData[itBindResult].ValueType = Tmp->ValueType;

            pBindResult[itBindResult].buffer_type = Tmp->ValueType;
            pBindResult[itBindResult].length = &pBindResultLength[itBindResult];
            pBindResult[itBindResult].is_null = &pBindResultIsNull[itBindResult];

            if(Tmp->ValueType == XMSQL_CHARS)
            {
                TmpData[itBindResult].Value = (char *)malloc(sizeof(char) * (MAXLEN + 1));
                pBindResult[itBindResult].buffer = TmpData[itBindResult].Value;
                pBindResult[itBindResult].buffer_length = MAXLEN;
            }
            else if(Tmp->ValueType == XMSQL_INT) 
            {
                TmpData[itBindResult].Value = (long *)malloc(sizeof(long));
                pBindResult[itBindResult].buffer = TmpData[itBindResult].Value;
            }
            itBindResult++;
        }

        rv = mysql_stmt_bind_result(pStmt, pBindResult);
        if(rv)
        {
            // printf("SQLERROR %d:%s\n",pStmt->last_errno , pStmt->last_error);
            rv = -1; break;
        }

        rv = mysql_stmt_execute(pStmt);
        if(rv)
        {
            // printf("SQLERROR %d:%s\n",pStmt->last_errno,pStmt->last_error);
            rv = -1; break;
        }

        rv = mysql_stmt_store_result(pStmt);
        if(rv)
        {
            // printf("SQLERROR %d:%s\n",pStmt->last_errno, pStmt->last_error);
            rv = -1; break;
        }

        int i = 0;

        while (!mysql_stmt_fetch(pStmt))
        {
            for(i = 0; i < RowCnt; i++)
            {
                if(*pBindResult[i].is_null) continue;
                TmpData[i].Value = pBindResult[i].buffer;

                // if(pBindResult[i].buffer_type == XMSQL_CHARS)
                //     printf("SQLValue = %s\n",TmpData[i].Value);
                // else if(pBindResult[i].buffer_type == XMSQL_INT)
                //      printf("SQLValue = %d\n", *(int *)TmpData[i].Value);
            }
            XMSQLDataListAdd(pDataList, TmpData);
        }
        
    } while (0);
    
    if(pBindResult) free(pBindResult);
    if(RowCnt)
    {
        for(int i = 0; i < RowCnt; i++)
        {
            if(TmpData[i].Value) free(TmpData[i].Value);
        }
        free(TmpData);
    }
    if(pBindResultLength) free(pBindResultLength);
    if(pBindResultIsNull) free(pBindResultIsNull);
    }
    return rv ;
}

int XMSQLEXEC(pXMSQL_s psXMSQL, const char *SQLBuf, pXMSQLData_s Cond, pXMSQLData_s Data,pXMSQLData_l *pDataList)
{
    if(!psXMSQL || !SQLBuf) return -1;

    int rv = 0;
    Lock
    {
        MYSQL_STMT *pStmt = NULL;
        MYSQL_BIND *pBind = NULL;
        unsigned long *pBindLength = NULL;
        bool *pBindIsNull = NULL;
        do
        {
            pStmt = mysql_stmt_init(psXMSQL->pSQL);
            if(!pStmt)
            {
                // printf("SQLERROR %d:%s\n",pStmt->last_errno, pStmt->last_error);
                rv = -1; break;
            }

            // printf("SQLBuf = %s, %d\n", SQLBuf,strlen(SQLBuf));
            rv = mysql_stmt_prepare(pStmt, SQLBuf,strlen(SQLBuf));
            if(rv)
            {
                // printf("SQLERROR %d:%s\n",pStmt->last_errno, pStmt->last_error);
                rv = -1; break;
            }

            int cnt = mysql_stmt_param_count(pStmt);
            pBind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * (cnt));
            if(!pBind) {rv = -1; break;}
            memset(pBind, 0 , sizeof(MYSQL_BIND) * (cnt));

            pBindLength = (unsigned long *)malloc(sizeof(unsigned long) * (cnt));
            if(!pBindLength) {rv = -1; break;}
            memset(pBindLength, 0 ,sizeof(unsigned long) * (cnt));

            pBindIsNull = (bool *)malloc(sizeof(bool) * (cnt));
            if(!pBindIsNull) {rv = -1; break;}
            memset(pBindIsNull, 0, sizeof(bool) * (cnt));

            int itBind = 0;
            pXMSQLData_s Tmp = NULL;

            // printf("{\n");
            if(Data)
            {
                XMSQLDataEach(Tmp, Data)
                {
                    if(pDataList) continue;
                    pBind[itBind].buffer = Tmp->Value;
                    pBind[itBind].buffer_type = Tmp->ValueType;
                    pBind[itBind].is_null = &pBindIsNull[itBind];
                    if(!Tmp->Value)
                    {
                        pBindIsNull[itBind] = 1;
                        itBind++;
                        continue;
                    }
                    // printf("%s:",Tmp->Name);
                    if(Tmp->ValueType == XMSQL_CHARS)
                    {
                        pBind[itBind].buffer_length = strlen(Tmp->Value);
                        // printf("%s\n",Tmp->Value);
                    }
                    else if(Tmp->ValueType == XMSQL_INT)
                    {
                        // printf("%d\n", *(int *)Tmp->Value);
                    }
                    else 
                    {
                        // printf("unkown XMSQL_Type"); rv = -1; break;
                    }
                    itBind++;
                }
            }
            if(Cond)
            {
                XMSQLDataEach(Tmp, Cond)
                {
                    if(!Tmp->Value) pBindIsNull[itBind] = 1;
                    pBind[itBind].buffer = Tmp->Value;
                    pBind[itBind].buffer_type = Tmp->ValueType;
                    pBind[itBind].is_null = &pBindIsNull[itBind];
                    if(!Tmp->Value)
                    {
                        pBindIsNull[itBind] = 1;
                        itBind++;
                        continue;
                    }
                    
                    // printf("%s:",Tmp->Name);
                    if(Tmp->ValueType == XMSQL_CHARS)
                    {
                        pBind[itBind].buffer_length = strlen(Tmp->Value);
                        // printf("%s\n",Tmp->Value);
                    }
                    else if(Tmp->ValueType == XMSQL_INT)
                    {
                        // printf("%d\n",*(int *)Tmp->Value);
                    }
                    else 
                    {
                        // printf("unknown XMSQL_Type"); rv = -1; break;
                    }
                    itBind++;
                }
            }
            // printf("}\n");

            rv = mysql_stmt_bind_param(pStmt, pBind);
            if(rv)
            {
                printf("SQLERROR %d:%s\n",pStmt->last_errno,pStmt->last_error);
                rv = -1; break;
            }

            if(!pDataList)
            {
                rv = mysql_stmt_execute(pStmt);
                if(rv)
                {
                    printf("SQLERROR %d:%s\n", pStmt->last_errno,pStmt->last_error);
                    rv = -1; break;
                }
            }
            else 
            {
                rv = XMSQLEXEC_EX_GetResult(pStmt, Data, pDataList);
                if(rv) break;
            }

            rv = mysql_stmt_affected_rows(pStmt);
            if(rv <= 0)
            {
                // printf("SQL affected %d row\n",rv);
                rv = -1; break;                
            }
            rv = 0;
        } while (0);
        if(pBind) free(pBind);
        if(pStmt) mysql_stmt_close(pStmt);
        if(pBindLength) free(pBindLength);
        if(pBindIsNull) free(pBindIsNull);
    }
    Unlock

    return rv;
}

#endif

int XMSQLInster(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Data)
{
    if(!psXMSQL || !TableName || !Data) return -1;

    int rv = 0;
    {
    char *SQLBuf = NULL;
    int SQLBufLen = 0;
    do
    {
        SQLBuf = (char *)malloc(sizeof(char) * (2048 + 1));
        if(!SQLBuf) {rv = -1; break;}

        SQLBufLen = snprintf(SQLBuf, 2048+1, "INSERT INTO %s (",TableName);

        pXMSQLData_s Tmp = NULL;
        XMSQLDataEach(Tmp ,Data)
            SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, " %s ", Tmp->Name);

        SQLBuf[--SQLBufLen] = 0;

        SQLBufLen += snprintf(SQLBuf + SQLBufLen , 2048 - SQLBufLen + 1, " ) VALUES (");

        XMSQLDataEach(Tmp, Data)
            SQLBufLen += snprintf(SQLBuf + SQLBufLen , 2048 - SQLBufLen + 1 ," ? ");
        
        SQLBuf[--SQLBufLen] = 0;

        SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, ");");

        printf("SQL = %s\n", SQLBuf);

        rv = XMSQLEXEC(psXMSQL, SQLBuf, NULL, Data, NULL);
        if(rv) break;
    } while (0);
    if(SQLBuf) free(SQLBuf);
    }

    return rv ;
}

int XMSQLUpdate(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond,pXMSQLData_s Data)
{
    if(!psXMSQL || !TableName || !Data) return -1;

    int rv = 0;
    {
    char *SQLBuf = NULL;
    int SQLBufLen = 0;
    do
    {
        SQLBuf = (char *)malloc(sizeof(char) * (2048 + 1));
        if(!SQLBuf) { rv = -1; break;}

        SQLBufLen = snprintf(SQLBuf , 2048 + 1, "UPDATE %s SET", TableName);

        pXMSQLData_s Tmp = NULL;

        XMSQLDataEach(Tmp, Data)
            SQLBufLen += snprintf(SQLBuf + SQLBufLen , 2048 - SQLBufLen + 1, " %s=?", Tmp->Name);
        
        SQLBuf[--SQLBufLen] = 0;

        if(Cond)
        {
            SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, " WHERE");

            XMSQLDataEach(Tmp,Cond)
                SQLBufLen += snprintf(SQLBuf + SQLBufLen , 2048 -SQLBufLen + 1 , " %s=? AND", Tmp->Name);
            SQLBufLen -= 4;
            SQLBuf[SQLBufLen] = 0;
        }

        SQLBufLen += snprintf(SQLBuf + SQLBufLen , 208 - SQLBufLen + 1, ";");

        printf("SQL = %s\n", SQLBuf);
        rv = XMSQLEXEC(psXMSQL, SQLBuf, Cond, Data, NULL);
        if(rv) break;
    } while (0);
    if(SQLBuf) free(SQLBuf);
    }

    return rv ;
}

int XMSQLSelect(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond, pXMSQLData_s Data, pXMSQLData_l *pDataList)
{
    if(!psXMSQL || !TableName || !Data || !pDataList) return -1;

    int rv = 0;

    {
    char *SQLBuf = NULL; int SQLBufLen = 0;
    do
    {
        SQLBuf = (char *)malloc(sizeof(char) * (2048 + 1));
        if(!SQLBuf) { rv = -1; break;}

        pXMSQLData_s Tmp = NULL;

        SQLBufLen = snprintf (SQLBuf , 2048 + 1, "SELECT");

        if (Data)
        {
            XMSQLDataEach(Tmp,Data)
                SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, " %s ",Tmp->Name);
            SQLBuf[--SQLBufLen] = 0;
        }
        else 
            SQLBufLen += snprintf(SQLBuf + SQLBufLen , 2048 - SQLBufLen + 1, " *");

            SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, " FROM %s", TableName);

        if(Cond)
        {
            SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, " WHERE");

            XMSQLDataEach(Tmp , Cond)
                SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, " %s=? AND", Tmp->Name);
            SQLBufLen -= 4;
            SQLBuf[SQLBufLen] = 0;
        }

        SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, ";");

        printf("SQL = %s\n", SQLBuf);

        rv = XMSQLEXEC(psXMSQL,SQLBuf,Cond, Data, pDataList);
        if (rv) break;

        if(Data)
        {
            int i = 0;
            XMSQLDataEach(Tmp, Data) Tmp->Value = (*pDataList)->Data[i++].Value;
        }

    } while (0);
    if(SQLBuf) free(SQLBuf);
    }

    return rv ;
}

int XMSQLDelete(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond)
{
    if(!psXMSQL || !TableName) return -1;

    int rv = 0;
    {
    char *SQLBuf = NULL;
    int SQLBufLen = 0;
    do
    {
        SQLBuf = (char *)malloc(sizeof(char) * (2048 + 1));
        if(!SQLBuf) {rv = -1; break;}

        pXMSQLData_s Tmp = NULL;

        SQLBufLen = snprintf(SQLBuf, 2048 + 1, "DELETE FROM %s",TableName);

        if(Cond)
        {
            SQLBufLen += snprintf(SQLBuf + SQLBufLen, 2048 - SQLBufLen + 1, "WHERE");

            XMSQLDataEach(Tmp,Cond)
                SQLBufLen += snprintf(SQLBuf + SQLBufLen , 2048 - SQLBufLen + 1, " %s=? AND",Tmp->Name);
            SQLBufLen -= 4;
            SQLBuf[SQLBufLen] = 0;
        }

        SQLBufLen += snprintf(SQLBuf + SQLBufLen , 2048 - SQLBufLen + 1 ,";");

        printf("SQL = %s\n",SQLBuf);

        rv = XMSQLEXEC(psXMSQL,SQLBuf,Cond, NULL, NULL);
        if(rv) break;
    }while(0);
    if(SQLBuf) free(SQLBuf);
    }
    return rv;
}

int XMSQLUpdateForce(pXMSQL_s psXMSQL, const char *TableName, pXMSQLData_s Cond, pXMSQLData_s Data)
{
    if(!psXMSQL || !TableName) return -1;
    int rv = 0;

    rv = XMSQLUpdate(psXMSQL, TableName , Cond , Data);
    if(rv) rv = XMSQLInster(psXMSQL, TableName, Data);

    return rv;
}

/*SQLite*/
pXMSQL_f XMSQLFunInit()
{
    pthread_mutex_lock(&FunExistLock);
    if(!FunExist++)
    {
        pfXMSQLData = XMSQLDataFunInit();
    }

    pthread_mutex_unlock(&FunExistLock);
    pXMSQL_f pf = (pXMSQL_f)malloc(sizeof(XMSQL_f));
    pf->Init = XMSQLInit;
    #ifdef XMSQL_SQLITE
    #else
    pf->InitByParam = XMSQLInitByParam;
    #endif
    pf->Destroy = XMSQLDestroy;
    pf->Inster = XMSQLInster;
    pf->Update = XMSQLUpdate;
    pf->Select = XMSQLSelect;
    pf->Delete = XMSQLDelete;
    pf->UpdateForce = XMSQLUpdateForce;

    pf->pfXMSQLData = pfXMSQLData;

    return pf;
}

void XMSQLFunDestroy(pXMSQL_f pf)
{
    if(!pf) return;

    pthread_mutex_lock(&FunExistLock);
    if(!--FunExist)
    {
        XMSQLDataFunDestroy(pfXMSQLData);
    }
    pthread_mutex_unlock(&FunExistLock);

    free(pf);

    return ;
}
