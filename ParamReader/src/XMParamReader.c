#include "XMParamReader.h"
#include "XMFileRW.h"

#include <stdio.h>
#include <cJSON.h>

#define CP printf("%d\n", __LINE__);

void XMParamReaderShow(pXMParamReader_s ps)
{
    if(!ps) return ;

    int flag = 0;
    printf("{\n");
    pXMParamReader_s tmp = NULL;
    XMParamReaderForEach(ps, tmp)
    {
        if(tmp->Type == XMParamReaderSeparator)
        {
            if(flag == tmp->Type) flag = XMParamReaderNULL;
            else 
            {
                printf("\t\"%s\":\n",tmp->Name);
                flag = XMParamReaderSeparator;
            }
            continue;
        }
        if(flag == XMParamReaderSeparator)
            printf("\t\t\"%s\" = \"%s\"\n",tmp->Name, tmp->Value);
        else 
            printf("\t\"%s\" = \"%s\"\n",tmp->Name, tmp->Value);
    }
    printf("}\n");

    return ;
}

void XMParamReaderHelp(pXMParamReader_s ps)
{
    if(!ps) return;

    pXMParamReader_s tmp = NULL;
    XMParamReaderForEach(ps, tmp);
    if(!tmp->Help) printf("[%s]:%s\n",tmp->Name, tmp->Help);
    return;
}

int XMParamReaderByFile(pXMParamReader_s ps, const char *FilePath)
{
    if (!FilePath) return -1;

    int rv = 0;

    char *Data = NULL;
    cJSON *DataJson = NULL;
    do
    {
        rv = XMFileRWReader(FilePath, &Data);
        if(rv < 0) break;

        DataJson = cJSON_Parse(Data);
        if(!DataJson) {rv = -1; break;}

        cJSON *Object = NULL , *String = NULL;
        pXMParamReader_s tmp = NULL;
        XMParamReaderForEach(ps, tmp)
        {
            if(tmp->Type == XMParamReaderSeparator)
            {
                if(!Object)
                {
                    Object = cJSON_GetObjectItem(DataJson,tmp->Name);
                    if(!Object) while ((++tmp)->Type != XMParamReaderSeparator);
                } 
                else 
                    Object = NULL;
                continue;
            }
            if(Object)
                String = cJSON_GetObjectItem(Object, tmp->Name);
            else 
                String = cJSON_GetObjectItem(DataJson, tmp->Name);
            if(!String || !String->valuestring) continue;

            int tLen = strlen(String->valuestring);
            if(!tmp->Value) tmp->Value = (char *)malloc(sizeof(char) * (tLen + 1));

            snprintf(tmp->Value, tLen + 1, "%s", String->valuestring);
        }
        if(rv < 0) break;
    } while (0);
    XMFileRWFree(Data);
    cJSON_Delete(DataJson);

    return rv ;
}

int XMParamReader(pXMParamReader_s ps, const int BufferCount, const char **Buffer)
{
    if(!Buffer) return -1;
    char **Tmp = NULL;
    pXMParamReader_s it = NULL;

    int i = 0, NowPos = 0;
    while (i < BufferCount)
    {
        if(Buffer[i][0] == '-')
        {
            if(Buffer[i][1] == (char)'-')
            {
                NowPos = 2;
                XMParamReaderForEach(ps , it)
                {
                    if(!strncmp(it->Name , Buffer[i] + NowPos, strlen(it->Name)))
                    {
                        NowPos += strlen(it->Name);
                        if(Buffer[i][NowPos] == '-')
                        {
                            NowPos++;
                            while ((++it)->Type != XMParamReaderSeparator)
                            {
                                if(!strcmp(it->Name, Buffer[i] + NowPos))
                                {
                                    Tmp = &it->Value;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            else 
            {
                NowPos = 1;
                XMParamReaderForEach(ps, it)
                {
                    if(it->Type == XMParamReaderSeparator) while ((++it)->Type != XMParamReaderSeparator);
                    if(!strcmp(it->Name, Buffer[i] + NowPos))
                    {
                        Tmp = &it->Value;
                        break;
                    }
                }
            }
        }
        else 
        {
            if(Tmp)
            {
                int tLen = strlen(Buffer[i]);
                if(!(*Tmp)) *Tmp = (char *)malloc(sizeof(char) * (tLen) + 1);
                snprintf(*Tmp , tLen + 1 , "%s", Buffer[i]);
            }
            Tmp = NULL;
        }
        i++;
    }
    return 0;
}

pXMParamReader_f XMParamReaderFunInit()
{
    pXMParamReader_f pf = (pXMParamReader_f)malloc(sizeof(XMParamReader_f));
    if(!pf) return NULL;

    pf->Help = XMParamReaderShow;
    pf->Clear = XMParamReaderHelp;
    pf->ReadByFile = XMParamReaderByFile;
    pf->Read = XMParamReader;

    return pf;
}

void XMParamReaderFunDestroy(pXMParamReader_f pf)
{
    if(!pf) return ;

    free(pf);

    return;
}