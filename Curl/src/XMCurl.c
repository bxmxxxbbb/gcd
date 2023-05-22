#include "XMCurl.h"
#include <curl/curl.h>

#include <string.h>

#include <stdlib.h>

struct Result_P
{
    char *Buffer;
    int BufferSize;
};

static inline size_t GetResult(void *buffer, size_t size, size_t nmemb, void *Param)
{
    struct Result_P *Result = (struct Result_P *)Param;

    char *ptr = NULL;
    if(Result->Buffer)
        ptr = (char *)realloc(Result->Buffer,Result->BufferSize + size * nmemb + 1);
    else 
        ptr = (char *)malloc(Result->BufferSize + size * nmemb + 1);
    if(!ptr) return 0;

    Result->Buffer = ptr;

    memcpy(Result->Buffer + Result->BufferSize, (char *)buffer, size * nmemb);
    Result->BufferSize += size * nmemb;

    Result->Buffer[Result->BufferSize] = 0;

    return size * nmemb;
}

int XMCurlGet(const char *URL, char **Result)
{
    CURL *t = curl_easy_init();

    curl_easy_setopt(t, CURLOPT_URL , URL);

    curl_easy_setopt(t, CURLOPT_TIMEOUT, 3L);

    curl_easy_setopt(t, CURLOPT_SSL_VERIFYPEER, 0);

    curl_easy_setopt(t, CURLOPT_WRITEFUNCTION, GetResult);

    struct Result_P Param = {NULL, 0};

    curl_easy_setopt(t , CURLOPT_WRITEDATA, &Param);

    curl_easy_perform(t);

    curl_easy_cleanup(t);

    if(Result) *Result = Param.Buffer;
    else free(Param.Buffer);

    return 0;
}

int XMCurlPost(const char *URL, const char *PostData, const char **Headers, char **Result)
{
    int i = 0;
    struct curl_slist * list = NULL;
    CURL *t = curl_easy_init();

    curl_easy_setopt(t, CURLOPT_POST, 1);

    curl_easy_setopt(t, CURLOPT_TIMEOUT, 3L);

    curl_easy_setopt(t, CURLOPT_URL, URL);

    curl_easy_setopt(t,CURLOPT_POSTFIELDS, PostData);

    while(Headers[i]) list = curl_slist_append(list, (const char *)(Headers[i++])); //将很多的头域加入list

    curl_easy_setopt(t, CURLOPT_HTTPHEADER, list);

    // curl_easy_setopt(t, CURLOPT_HEADER, 1);

    curl_easy_setopt(t, CURLOPT_SSL_VERIFYPEER, 0);

    curl_easy_setopt(t, CURLOPT_WRITEFUNCTION, GetResult);

    struct Result_P Param = {NULL, 0};

    curl_easy_setopt(t, CURLOPT_WRITEDATA, &Param); // 发送后得到的结果

    curl_easy_perform(t);

    curl_slist_free_all(list);

    curl_easy_cleanup(t);

    if(Result) *Result = Param.Buffer;
    else free(Param.Buffer);

    return 0;
}

int XMCurlPut(const char *URL, const char *PostData, const char **Headers, char **Result)
{
    //X-HTTP-Method-Override: PUT;
    int i = 0;
    struct curl_slist *list = NULL;

    CURL * t = curl_easy_init();

    curl_easy_setopt(t, CURLOPT_UPLOAD, 1);

    curl_easy_setopt(t, CURLOPT_TIMEOUT, 3L);

    curl_easy_setopt(t, CURLOPT_URL, URL);

    curl_easy_setopt(t, CURLOPT_POSTFIELDS, PostData);

    // curl_easy_setopt(t, CURLOPT_CUSTOMREQUEST, "PUT");

    while(Headers[i]) list = curl_slist_append(list, (const char *)(Headers[i++]));

    curl_easy_setopt(t, CURLOPT_HTTPHEADER, list);

    curl_easy_setopt(t, CURLOPT_SSL_VERIFYPEER, 0);

    curl_easy_setopt(t, CURLOPT_WRITEFUNCTION, GetResult);

    struct Result_P Param =  {NULL, 0};

    curl_easy_setopt(t, CURLOPT_WRITEDATA, &Param);

    curl_easy_perform(t);

    curl_slist_free_all(list);

    curl_easy_cleanup(t);

    if(Result) *Result = Param.Buffer;
    else free(Param.Buffer);

    return 0;
}

int XMCurlDelete(const char *URL, const char *PostData, const char **Headers, char **Result)
{
    // X-HTTP-Method-Override: DELETE;
    int i = 0;
    struct curl_slist *list = NULL;

    CURL *t = curl_easy_init();

    curl_easy_setopt(t, CURLOPT_URL, URL);

    curl_easy_setopt(t, CURLOPT_POSTFIELDS, PostData);

    curl_easy_setopt(t, CURLOPT_CUSTOMREQUEST, "DELETE");

    while(Headers[i]) list = curl_slist_append(list, (const char  *)(Headers[i++]));

    curl_easy_setopt(t, CURLOPT_HTTPHEADER, list);

    curl_easy_setopt(t, CURLOPT_SSL_VERIFYPEER, 0);

    curl_easy_setopt(t, CURLOPT_WRITEFUNCTION, GetResult);

    struct Result_P Param = {NULL , 0};

    curl_easy_setopt(t, CURLOPT_WRITEDATA, &Param);

    curl_easy_perform(t);

    curl_slist_free_all(list);

    curl_easy_cleanup(t);

    if(Result) *Result =  Param.Buffer;
    else free(Param.Buffer);

    return 0;
}

void XMCurlFree(char *Result)
{
    if(Result) free(Result);
    return;
}

pXMCurl_f XMCurlFunInit()
{
    pXMCurl_f fp = (pXMCurl_f)malloc(sizeof(XMCurl_f));
    if(!fp) return NULL;

    fp->Get = XMCurlGet;

    fp->Post = XMCurlPost;

    fp->Free = XMCurlFree;

    return fp;
}

void XMCurlFunDestroy(pXMCurl_f fp)
{
    if(!fp) return;

    free(fp);
    return ;
}