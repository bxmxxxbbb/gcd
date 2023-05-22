/**
*@file FileRW.h
*@author bxm
*@brief 文件管理项目总头文件
*@version 0.2
*@date 2022-08-22
*
*@copyright Copyright (c) 2022
*/

#ifndef __XMFILERW_H__
#define __XMFILERW_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
*@brief 文件读取
*@param FilePath 文件路径
*@param Data [OUT] 读出数据
*@param DataLen [OUT] 读出数据长度
*@return int 0：SUCCESS ，other :FAIL
*/

int XMFileRWRead(const char *FilePath, char **Data, int *DataLen);

/**
*@brief 文件覆写
*
*@param FilePath 文件路径
*@param Data [IN] 写入数据
*@param DateLen [IN] 写入数据长度
*@return int 0:SUCCESS other:FAIL
*/

int XMFileRWWrite(const char *FilePath , const char *Data,const int DataLen);

/**
*
*@brief 文件追加
*
*@param FilePath 文件路径
*@param Data [IN] 写入数据
*@param DataLen [IN] 写入数据长度
*return int 0:SUCCESS other:FAIL
*/
int XMFileRWWriteAppend(const char *FilePath, const char *Data , const int DataLen);

/**
*@brief 相关参数空间释放
*
*
*@param Data 待释放数据
*/
void XMFileFree(char *Data);

/**
*@brief 文件操作函数句柄
*
*/
typedef struct XMFileRW_F
{
    int (*Read)(const char *FilePath, char **Data, int *DataLen);

    int (*Write)(const char *FilePath, const char *Data, const int DataLen);

    int (*WriteAppend)(const char *FilePath , const char *Data , const int DataLen);

    void (*Free)(char *Data);
}XMFileRW_f , *pXMFileRW_f;

/**
*@brief 文件读写函数句柄初始化
*
*return pXMFileRW_f 文件读写函数句柄
*/
pXMFileRW_f XMFileRWFunInit();

/**
*@brief 文件读写函数句柄销毁
*
*@param pfXMFileRW 文件读写函数句柄
*/
void XMFileRWFunDestroy(pXMFileRW_f pfXMFileRW);

#endif