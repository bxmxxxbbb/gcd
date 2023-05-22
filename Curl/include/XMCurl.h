/**
*@file XMCurl.h
*@author XBB(1653651960@qq.com)
*@brief curl工具封装
*XMCurl
* |
* -curl
*      |
*      -zlib
*      -openssl
*@version 0.1
*@date 2022-09-30
*
*@copyright Copyright (c) 2022
*
*/

#ifndef __XMCURL_H__
#define __XMCURL_H__

#include <curl/curl.h>

int XMCurlGet(const char *URL, char **Result);
int XMCurlPost(const char *URL, const char *PostData, const char **Headers, char **Result);
int XMCurlPut(const char *URL, const char *PostData, const char **Headers, char **Result);
int XMCurlDelete(const char *URL, const char *PostData, const char **Headers, char **Result);
void XMCurlFree(char *Result);

/**
*@brief 发送http的方法封装结构
*
*/
typedef struct XMCurl_F
{
    /**
     * @brief 发送一个get请求
     * 
     * @param URL 请求的url
     * @param Result 带回请求的结果，使用本结构free释放
     * @return int 0:成功 !0:失败
    */
   int (*Get)(const char *URL, char **Result);

   /**
    * @brief 发送post请求
    * 
    * @param URL 请求的url
    * @param PostData 请求的post参数
    * @param Headers http头部,遍历插入一维以NULL结尾
    * @param Result 带回请求的结果,使用本结构free释放
    * @return int 0:成功 !0:失败
   */
  int  (*Post)(const char *URL, const char *PostData, const char **Headers, char **Result);

  /**
   * @brief 释放返回结果的空间
   * 
   * @param 被释放的参数
  */
    void (*Free)(char *Param);
}XMCurl_f, *pXMCurl_f;

/**
 * @brief 方法结构初始化
 * 
 * @return pXMCurl_f 方法结构句柄
*/
pXMCurl_f XMCurlFunInit();

/**
 * @brief http的方法结构句柄销毁
 * 
 * @param fp 方法结构句柄
*/
void XMCurlFunDestroy(pXMCurl_f fp);

#endif