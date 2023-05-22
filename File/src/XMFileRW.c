#include "XMFileRW.h"

#define CP printf("checkpoint %d\n",__LINE__);

int XMFileRWRead(const char *FilePath, char **Data, int *DataLen)
{
    if (!FilePath || !Data || !DataLen) return -1;

    int rv = 0;
    char *Buf = NULL;
    {
        FILE *Reader = NULL ;
        do 
        {
            Reader = fopen(FilePath , "r");
            if(!Reader) {rv = -1 ; break;}

            fseek(Reader, 0 , SEEK_END);
            *DataLen = ftell(Reader);
            rewind(Reader);

            if(!*DataLen) break;

            Buf = (char *)malloc(sizeof(char) * (*DataLen + 1));
            *DataLen = fread(Buf , sizeof(char) , (size_t) * DataLen, Reader);
            Buf[*DataLen] = 0;
        }while(0);
        if(Reader) fclose(Reader);
    }

    if(rv < 0)
    {
        if(Buf) free(Buf);
        return -1;
    }
    *Data = Buf;

    return 0;
}

int XMFileRWWrite(const char *FilePath,const char *Data,const int DataLen)
{
    if(!FilePath || !Data) return -1;

    int rv = 0;
    {
        FILE *Reader = NULL;
        do
        {
            Reader = fopen(FilePath, "w");
            if(!Reader){rv = -1;break;}
            fwrite(Data,sizeof(char),DataLen,Reader);
        }while(0);
        if(Reader) fclose(Reader);
    }
    return rv;
}

int XMFileRWWriteAppend(const char *FilePath,const char *Data,const int DataLen)
{
    if(!FilePath || !Data) return -1;
    int rv = 0;
    {
        FILE *Reader = NULL;
        do
        {
            Reader = fopen(FilePath, "a");
            if(!Reader)
            {
                Reader = fopen(FilePath, "w");
            }
            fwrite(Data,sizeof(char), DataLen,Reader);
        }while(0);
        if(Reader) fclose(Reader);
    }

    return rv;
}

void XMFileRWFree(char *Data)
{
    if(!Data) return;
    free(Data);
    return ;
}

pXMFileRW_f XMFileRWFunInit()
{
    pXMFileRW_f pf = (pXMFileRW_f)malloc(sizeof(XMFileRW_f));
    if(!pf) return NULL;

    pf->Read = XMFileRWRead;
    pf->Write = XMFileRWWrite;
    pf->WriteAppend = XMFileRWWriteAppend;
    pf->Free = XMFileRWFree;

    return pf;
}

void XMFileRWFunDestroy(pXMFileRW_f pf)
{
    if (!pf) return ;
    free(pf);
    return ;
}