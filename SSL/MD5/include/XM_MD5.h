#ifndef __XM_MD5_H__
#define __XM_MD5_H__

#include <stdio.h>

typedef struct XM_MD5_S XM_MD5_s ,*pXM_MD5_s;

typedef struct XM_MD5_F
{
    pXM_MD5_s (*Init)();\
    void (*Destroy)(pXM_MD5_s *pspMD5);
    int (*Update)(pXM_MD5_s spMD5 , const void *data , size_t len);
    int (*Final)(pXM_MD5_s spMd5 ,  unsigned char *md);
}XM_MD5_f , *pXM_MD5_f;

pXM_MD5_f XM_MD5FUNInit();

void XM_MD5FunDestroy(pXM_MD5_f *pfpMD5);

#endif