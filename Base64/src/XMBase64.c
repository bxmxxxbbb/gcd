#include "XMBase64.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SC_base64_de_isCharValid(character) (('+' <= character) && (character <= 'z') && (base64_de_table[character - '+'] != -1))

static const char base64_en_table[64] =
{
    'A','B','C','D','E','F','G','H',
    'I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X',
    'Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n',
    'o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3',
    '4','5','6','7','8','9','+','/',
};

///ASCII order for BASE 64 decode , -1 in unused character
static const signed char base64_de_table[80] = 
{
    /* '+', ',', '-', '.', '/', '0', '1', '2',*/
        62,  -1,  -1,  -1,  63,  52,  53,  54,

    /* '3', '4', '5', '6', '7', '8', '9', ':', */
        55,  56,  57,  58,  59,  60,  61,  -1,

    /* ';', '<', '=', '>', '?', '@', 'A', 'B', */
        -1,  -1,  0,  -1,  -1,  -1,   0,   1,
    
    /* 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', */
         2,   3,   4,   5,   6,   7,   8,   9,

    /* 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', */
        10,  11,  12,  13,  14,  15,  16,  17,

    /* 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', */
        18,  19,  20,  21,  22,  23,  24,  25,

    /* '[', '\', ']', '^', '_', '`', 'a', 'b', */
        -1,  -1,  -1,  -1,  -1,  -1,  26,  27,

    /* 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', */
        28,  29,  30,  31,  32,  33,  34,  35,

    /* 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', */
        36,  37,  38,  39,  40,  41,  42,  43,

    /* 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',*/
        44,  45,  46,  47,  48,  49,  50,  51 
};

int XMBase64Encode(const char *In, const int InLen , char **Out)
{
    if (NULL == In || InLen <= 0 || !Out) return -1;

    int OutLen = InLen /3*4;
    switch(InLen % 3)
    {
        case 2: 
        case 1:
            OutLen += 4;
        case 0:
            break;
    }

    *Out =(char *)malloc(sizeof(char) * (OutLen + 1));
    if (!*Out) return -1;

    OutLen = 0;

    int i;
    for (i = 0; i < InLen ; i++)
    {
        switch(i%3)
        {
            case 0:
            {
                (*Out)[OutLen++] = base64_en_table[In[i] >> 2 & 0x3F];
                break;
            }
            case 1:
            {
                (*Out)[OutLen++] = base64_en_table[((In[i - 1] << 4) & 0x30) + ((In[i] >> 4) & 0x0F)];
                break;
            }
            case 2:
            {
                (*Out)[OutLen++] = base64_en_table[((In[i -1 ] << 2) &0x3C) + ((In[i] >> 6) & 0x03)];
                (*Out)[OutLen++] = base64_en_table[In[i] & 0x3F];
                break;
            }
        }
    }

    switch(InLen % 3)
    {
        case 1: /*Add two zero byte padding */
        {
            (*Out)[OutLen++] = base64_en_table[(In[InLen -1] << 4) & 0x3C];
            (*Out)[OutLen++] = '=';
            (*Out)[OutLen++] = '=';
            break;
        }
        case 2: /*Add one zero byte padding */
        {
            (*Out)[OutLen++] = base64_en_table[(In[InLen - 1] << 2) & 0x3C];
            (*Out)[OutLen++] = '=';
            break ;
        }
    }
    (*Out)[OutLen] = 0;

    return 0;
}

int XMBase64Decode(const char *In, char **Out, int *OutLen)
{
    if (NULL == In) return -1;
    int InLen = strlen(In);
    if (InLen <= 0 || InLen % 4 != 0) return -1;

    /*check char input validty(must be in 64 specified characters). */
    int i = 0;
    while((i < InLen) && SC_base64_de_isCharValid(In[i])) i++;

    *OutLen = (InLen / 4) * 3;
    if(In[InLen] == '=') (*OutLen)--;
    if(In[InLen -1] == '=') (*OutLen)--;
    *Out = (char *)malloc(sizeof(char) * (*OutLen + 1));
    if(!*Out) return -1;

    *OutLen = 0;
    for(i = 0; i < InLen; i += 4)
    {
        int index0 = base64_de_table[In[i + 0] - '+'];
        int index1 = base64_de_table[In[i + 1] - '+'];
        int index2 = base64_de_table[In[i + 2] - '+'];
        int index3 = base64_de_table[In[i + 3] - '+'];

        (*Out)[(*OutLen)++] = ((index0 << 2) & 0xFC) + ((index1 >> 4) & 0x03);
        if ('=' != In[i + 2])
            (*Out)[(*OutLen)++] = ((index1 << 4) & 0xF0) + ((index2 >> 2) & 0x0F);
        if ('=' != In[i + 3])
            (*Out)[(*OutLen)++] = ((index2 << 6) & 0xC0) + ((index3 >> 0) & 0x3F);
    }
    (*Out)[(*OutLen)] = 0;
    return 0;
}

void XMBase64Free(char *Param)
{
    if (Param) free(Param);
    return ;
}

pXMBase64_f XMBase64FunInit()
{
    pXMBase64_f fp = (pXMBase64_f)malloc(sizeof(XMBase64_f));
    if(!fp) return NULL;

    fp->Encode = XMBase64Encode;
    fp->Decode = XMBase64Decode;
    fp->Free = XMBase64Free;

    return fp;
}

void XMBase64FunDestroy(pXMBase64_f fp)
{
    if (fp) free(fp);
    return ;
}
