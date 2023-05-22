#include "XM_MD5.h"
#include <stdlib.h>
#include <string.h>


#define MD5_LONG unsigned int

#define MD5_CBLOCK 64
#define MD5_LBLOCK (MD5_CBLOCK/4)
#define MD5_DIGEST_LENGTH 16

#define INIT_DATA_A (unsigned long)0x67452301L
#define INIT_DATA_B (unsigned long)0xefcdab89L
#define INIT_DATA_C (unsigned long)0x98badcfeL
#define INIT_DATA_D (unsigned long)0x10325476L

#define ROTATE(a,n) (((a)<<(n))|(((a)&0xffffffff)>>(32-(n))))

#define F(b ,c ,d) ((((c) ^ (d)) & (b)) ^ (d))
#define G(b ,c ,d) ((((b) ^ (c)) & (d)) ^ (c))
#define H(b ,c ,d) ((b) ^ (c) ^ (d))
#define I(b ,c ,d) (((~(d)) | (b)) ^ (c))

#define R0(a, b, c, d, k, s, t){ \
        a += ((k) + (t) + F((b), (c), (d)));\
        a = ROTATE(a , s);\
        a += b;};

#define R1(a, b, c, d, k, s, t){ \
        a += ((k) + (t) + G((b), (c), (d)));\
        a = ROTATE(a, s);\
        a += b;};

#define R2(a, b, c, d, k, s, t){\
        a += ((k) + (t) + H((b), (c), (d)));\
        a = ROTATE(a,s);\
        a += b;};

#define R3(a, b, c, d, k, s, t){\
        a += ((k) + (t) +I((b), (c), (d)));\
        a = ROTATE(a, s);\
        a += b;} ;

#define MD32_REG_T int

#define HOST_c2l(c, l) ( l = (((unsigned long)(*((c)++)))    ),   \
                        l |= (((unsigned long)(*((c)++)))<< 8),   \
                        l |= (((unsigned long)(*((c)++)))<<16),   \
                        l |= (((unsigned long)(*((c)++)))<<24)    )

#define HOST_l2c(l, c) (*((c)++) = (unsigned char)(((l)     )   & 0xff),    \
                        *((c)++) = (unsigned char)(((l) >> 8)   & 0xff),    \
                        *((c)++) = (unsigned char)(((l) >> 16)  & 0xff),    \
                        *((c)++) = (unsigned char)(((l) >> 24)  & 0xff),    \
                        l)

struct  XM_MD5_S
{
    MD5_LONG A, B, C, D;
    MD5_LONG Nl, Nh;
    MD5_LONG data[MD5_LBLOCK];
    unsigned int num;
};

static void md5_block_data_order(pXM_MD5_s spMD5 , const void *data_ , size_t num)
{
    const unsigned char *data = data_;
    register unsigned MD32_REG_T A, B, C, D, l;

    unsigned MD32_REG_T XX0, XX1, XX2, XX3, XX4, XX5, XX6, XX7, 
        XX8, XX9, XX10, XX11, XX12, XX13, XX14, XX15;

    #define X(I) XX##I

    A = spMD5->A;
    B = spMD5->B;
    C = spMD5->C;
    D = spMD5->D;

    for (;num--;)
    {
        (void)HOST_c2l(data, l);
        X(0) = l;
        (void)HOST_c2l(data, l);
        X(1) = l;
        /*Round 0*/
        R0(A, B, C, D, X(0), 7, 0xd76aa478L);
        (void)HOST_c2l(data, l);
        X(2) = l;
        R0(D, A, B, C, X(1), 12, 0xe8c7b756L);
        (void)HOST_c2l(data, l);
        X(3) = l;
        R0(C, D, A, B, X(2), 17, 0x242070dbL);
        (void)HOST_c2l(data, l);
        X(4) = l;
        R0(B, C, D, A, X(3), 22, 0xc1bdceeeL);
        (void)HOST_c2l(data, l);
        X(5) = l;
        R0(A, B, C, D, X(4), 7, 0xf57c0fafL);
        (void)HOST_c2l(data, l);
        X(6) = l;
        R0(D, A, B, C, X(5), 12, 0x4787c62aL);
        (void)HOST_c2l(data, l);
        X(7) = l;
        R0(C, D, A, B, X(6), 17, 0xa8304613L);
        (void)HOST_c2l(data, l);
        X(8) = l;
        R0(B, C, D, A, X(7), 22, 0xfd469501L);
        (void)HOST_c2l(data, l);
        X(9) = l;
        R0(A, B, C, D, X(8), 7, 0x698098d8L);
        (void)HOST_c2l(data, l);
        X(10) = l;
        R0(D, A, B, C, X(9), 12, 0x8B44f7afL);
        (void)HOST_c2l(data, l);
        X(11) = l;
        R0(C, D, A, B, X(10), 17, 0xffff5bb1L);
        (void)HOST_c2l(data, l);
        X(12) = l;
        R0(B, C, D, A, X(11), 22, 0x895cd7beL);
        (void)HOST_c2l(data, l);
        X(13) = l;
        R0(A, B, C, D, X(12), 7, 0x6b901122L);
        (void)HOST_c2l(data, l);
        X(14) = l;
        R0(D, A, B, C, X(13), 12, 0xfd987193L);
        (void)HOST_c2l(data, l);
        X(15) = l;
        R0(C, D, A, B, X(14), 17, 0xa679438eL);
        R0(B, C, D, A, X(15), 22, 0x49b40821L);
        /*Round 1*/
        R1(A, B, C, D, X(1), 5, 0xf61e2562L);
        R1(D, A, B, C, X(6), 9, 0xc040b340L);
        R1(C, D, A, B, X(11), 14, 0x265e5a51L);
        R1(B, C, D, A, X(0), 20, 0xe9b6c7aaL);
        R1(A, B, C, D, X(5), 5, 0xd62f105dL);
        R1(D, A, B, C, X(10), 9, 0x02441453L);
        R1(C, D, A, B, X(15), 14, 0xd8a1e681L);
        R1(B, C, D, A, X(4), 20, 0xe7d3fbc8L);
        R1(A, B, C, D, X(9), 5, 0x21e1cde6L);
        R1(D, A, B, C, X(14), 9, 0xc33707d6L);
        R1(C, D, A, B, X(3), 14, 0xf4d50d87L);
        R1(B, C, D, A, X(8), 20, 0x455a14edL);
        R1(A, B, C, D, X(13), 5, 0xa9e3e905L);
        R1(D, A, B, C, X(2), 9, 0xfcefa3f8L);
        R1(C, D, A, B, X(7), 14, 0x676f02d9L);
        R1(B, C, D, A, X(12), 20, 0x8d2a4c8aL);
        /*Round 2*/
        R2(A, B, C, D, X(5), 4, 0xfffa3942L);
        R2(D, A, B, C, X(8), 11, 0x8771f681L);
        R2(C, D, A, B, X(11), 16, 0x6d9d6122L);
        R2(B, C, D, A, X(14), 23, 0xfde5380cL);
        R2(A, B, C, D, X(1), 4, 0xa4beea44L);
        R2(D, A, B, C, X(4), 11, 0x4bdecfa9L);
        R2(C, D, A, B, X(7), 16, 0xf6bb4b60L);
        R2(B, C, D, A, X(10), 23, 0xbebfbc70L);
        R2(A, B, C, D, X(13), 4, 0x289b7ec6L);
        R2(D, A, B, C, X(0), 11, 0xeaa127faL);
        R2(C, D, A, B, X(3), 16, 0xd4ef3085L);
        R2(B, C, D, A, X(6), 23, 0x04881d05L);
        R2(A, B, C, D, X(9), 4, 0xd9d4d039L);
        R2(D, A, B, C, X(12), 11, 0xe6db99e5L);
        R2(C, D, A, B, X(15), 16, 0x1fa27cf8L);
        R2(B, C, D, A, X(2), 23, 0xc4ac5665L);
        /*Round 3*/
        R3(A, B, C, D, X(0), 6, 0xf4292244L);
        R3(D, A, B, C, X(7), 10, 0x432aff97L);
        R3(C, D, A, B, X(14), 15, 0xab9423a7L);
        R3(B, C, D, A, X(5), 21, 0xfc93a039L);
        R3(A, B, C, D, X(12), 6, 0x655b59c3L);
        R3(D, A, B, C, X(3), 10, 0x8f0ccc92L);
        R3(C, D, A, B, X(10), 15, 0xffeff47dL);
        R3(B, C, D, A, X(1) , 21, 0x85845dd1L);
        R3(A, B, C, D, X(8), 6, 0x6fa87e4fL);
        R3(D, A, B, C, X(15), 10, 0xfe2ce6e0L);
        R3(C, D, A, B, X(6), 15, 0xa3014314L);
        R3(B, C, D, A, X(13), 21, 0x4e0811a1L);
        R3(A, B, C, D, X(4), 6, 0xf7537e82L);
        R3(D, A, B, C, X(11), 10, 0xbd3af235L);
        R3(C, D, A, B, X(2), 15, 0x2ad7d2bbL);
        R3(B, C, D, A, X(9), 21, 0xeb86d391L);

        A = spMD5->A += A;
        B = spMD5->B += B;
        C = spMD5->C += C;
        D = spMD5->D += D;
    }
}

static pXM_MD5_s Init()
{
    pXM_MD5_s spMD5 = (pXM_MD5_s)malloc(sizeof(XM_MD5_s));
    memset(spMD5, 0, sizeof(XM_MD5_s));
    spMD5->A = INIT_DATA_A;
    spMD5->B = INIT_DATA_B;
    spMD5->C = INIT_DATA_C;
    spMD5->D = INIT_DATA_D;
    return spMD5;
}

static void Destroy(pXM_MD5_s *pspMD5)
{
    if (!pspMD5 || !*pspMD5) return;

    free(*pspMD5);
    *pspMD5 = NULL;
    return;
}

static int Update(pXM_MD5_s spMD5, const void *data_, size_t len)
{
    if(len == 0) return 0;
    const unsigned char *data = data_;
    unsigned char *p;
    MD5_LONG l;
    size_t n;

    l = (spMD5->Nl + (((MD5_LONG) len) << 3)) & 0xffffffffUL;
    if(l < spMD5->Nl)           /*overflow*/
        spMD5->Nh++;
    spMD5->Nh += (MD5_LONG) (len >> 29); /*might cause compiler warning on *16-bit*/
    spMD5->Nl = l;

    n = spMD5->num;
    if(n != 0 )
    {
        p = (unsigned char *)spMD5->data;
        if(len >= MD5_CBLOCK || len + n >= MD5_CBLOCK)
        {
            memcpy(p + n, data, MD5_CBLOCK - n);
            md5_block_data_order(spMD5, p, 1);
            n = MD5_CBLOCK - n;
            data += n;
            len -= n;
            spMD5->num = 0;
            memset(p, 0, MD5_CBLOCK); /*keep it zeroed */
        }
        else 
        {
            memcpy(p + n, data, len);
            spMD5->num += (unsigned int)len;
            return 0;        
        }
    }
    
    n = len / MD5_CBLOCK;
    if(n > 0 )
    {
        md5_block_data_order(spMD5, data, n);
        n *= MD5_CBLOCK;
        data += n;
        len -= n;
    }

    if( len != 0)
    {
        p = (unsigned char *)spMD5->data;
        spMD5->num = (unsigned int)len;
        memcpy(p, data, len);
    }
    return 0;
}

static int Final(pXM_MD5_s spMD5, unsigned char *md)
{
    unsigned char *p = (unsigned char *)spMD5->data;
    size_t  n = spMD5->num;

    p[n] = 0x80; /*there is always room for one*/
    n++;

    if(n > (MD5_CBLOCK - 8))
    {
        memset(p + n, 0, MD5_CBLOCK - n);
        n = 0;
        md5_block_data_order(spMD5, p, 1);
    }
    memset(p + n, 0, MD5_CBLOCK - 8 -n);

    p += MD5_CBLOCK - 8;

    (void)HOST_l2c(spMD5->Nl, p);
    (void)HOST_l2c(spMD5->Nh, p);

    p -= MD5_CBLOCK;
    md5_block_data_order(spMD5, p, 1);
    spMD5->num = 0;
    memset(p, 0, MD5_CBLOCK);

    do
    {
        unsigned long ll;
        ll = spMD5->A;
        (void)HOST_l2c(ll, md);
        ll = spMD5->B;
        (void)HOST_l2c(ll, md);
        ll = spMD5->C;
        (void)HOST_l2c(ll, md);
        ll = spMD5->D;
        (void)HOST_l2c(ll, md);
    }while (0);

    return 0;
}

pXM_MD5_f XM_MD5FUNInit()
{
    pXM_MD5_f fpMD5 = (pXM_MD5_f)malloc(sizeof(XM_MD5_f));

    fpMD5->Init = Init;
    fpMD5->Destroy = Destroy;
    fpMD5->Update = Update;
    fpMD5->Final = Final;

    return fpMD5;
}

void XM_MD5FunDestroy(pXM_MD5_f *pfpMD5)
{
    if(!pfpMD5 || !*pfpMD5) return ;
    free(*pfpMD5) ;
    *pfpMD5 = NULL;
    return;
}