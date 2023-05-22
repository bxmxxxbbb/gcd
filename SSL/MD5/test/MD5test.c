#include "XM_MD5.h"

int main()
{
    unsigned char md[1024];

    pXM_MD5_f fpMD5 = XM_MD5FUNInit();

    pXM_MD5_s spMD5 = fpMD5->Init();

    fpMD5->Update(spMD5, "abc", 3);
    fpMD5->Final(spMD5, md);
    for(int i = 0 ; i < 16; i++)
    {
        printf("%02x\n", md[i]);
    }

    fpMD5->Destroy(&spMD5);

    XM_MD5FunDestroy(&fpMD5);
}