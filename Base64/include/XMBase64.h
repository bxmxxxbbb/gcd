/**
*@file XMBase64.h
*@author bxm
*@brief Base64编码接口封装库
*@version 1.0
*@date 2022*08-22
*
*@copyright  Copyright (c)2022
*/

#ifndef __XMBASE64_H__
#define __XMBASE64_H__

int XMBase64Encode(const char *In,const int InLen,char **Out);
int XMBase64Decode(const char *In,char **Out,int *OutLen);
void XMBase64Free(char *Param);

/**
*@brief Base64编码接口封装结构
*
*/
typedef struct XMBase64_F
{
    /**
    *@brief Base64编码
    *
    *@param In 待编码数据
    *@param InLen 待编码数据长度
    *@param Out 编码后数据
    *@return int 0成功,其他失败
    */

    int (*Encode)(const char *In,const int InLen,char **Out);

    /**
    *@brief Base64解码
    *
    *@param In 待解码数据
    *@param InLen 待解码数据长度
    *@param Out 解码后数据
    *@return int 0成功，其他失败
    */

    int (*Decode)(const char *In,char **Out, int *OutLen);

    /**
    *@brief 释放内存
    *
    *@param Param 本结构生成内存指针
    */

    void (*Free)(char *Param);
}XMBase64_f, *pXMBase64_f;

/**
*@brief Base64编码接口初始化
*
*@return pXMBase64_f Base64编码接口句柄
*/
pXMBase64_f XMBase64FunInit();

/**
*@brief Base64编码接口销毁
*
*@param fp Base64编码接口句柄
*/

void XMBase64FunDestroy(pXMBase64_f pf);

#endif