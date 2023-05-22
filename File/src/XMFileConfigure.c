#include "XMFileConfigure.h"
#include "XMFileRW.h"
#include <stdio.h>
#include <cJSON.h>

#define CP printf("%d\n",__LINE__);

void XMFileConfigureShow(pXMFileConfigure_s ps)
{
    if(!ps) return;

    printf("{\n");
    pXMFileConfigure_s tmp = NULL;
    XMFileConfigureForEach(ps, tmp);
    printf("\t\"%s\" = \"%s\"\n", tmp->Name,tmp->Value);
    printf("}\n");

    return ;
}

void XMFileConfigureClear(pXMFileConfigure_s ps )
{
    if(!ps) return ;

    pXMFileConfigure_s tmp = NULL;
    XMFileConfigureForEach(ps , tmp);
        if(tmp->Value){
             free(tmp->Value);
             tmp->Value = NULL;
        }
        return;
}

int XMFileConfigcureRead(pXMFileConfigure_s ps, const char *FilePath)
{
    if ( !FilePath ) return -1;

    int rv = 0;
    char *Data = NULL;
    cJSON *DataJson = NULL;
    do
    {
        rv  = XMFileRWReader(FilePath, &Data);
        if(rv < 0 ) break;

        DataJson = cJSON_Parse(Data);
        if(!DataJson) break;

        cJSON *TmpJson = NULL;
        pXMFileConfigure_s tmp = NULL;
        XMFileConfigcureForEach(ps, tmp);
        {
            TmpJson = cJSON_GetObjectItem(DataJson,tmp->Name);
            if(!TmpJson){rv = -1; break;}

            tmp->Value = strdup(cJSON_GetStringValue(TmpJson));
        }
        if(rv < 0) break;
    } while (0);
    
    XMFileRWFree(Data);
    cJSON_Delete(DataJson);

    return rv;
}

int XMFileConfigureWrite(pXMFileConfigure_s ps , const char *FilePath)
{
    if (!ps || !FilePath) return -1;

    int rv = 0;
    cJSON *DataJson = NULL;
    char *Data = NULL;
    do
    {
        DataJson = cJSON_CreateObject();
        pXMFileConfigure_s tmp = NULL;
        XMFileConfigcureForEach(ps , tmp);
        if(!cJSON_AddItemToObject(DataJson, tmp->Name, cJSON_CreateString(tmp->Value?tmp->Value:"")))
        {
            rv = -1; break;
        }
        if(rv < 0 ) break;

        Data = cJSON_Print(DataJson);

        rv = XMFileRWWrite(FilePath,Data);
        if(rv < 0) break; 
    } while (0);

    cJSON_Delete(DataJson);
    cJSON_free(Data);

    return rv;    
}

pXMFileConfigure_f XMFileConfigureFunInit()
{
    pXMFileConfigure_f pf = (pXMFileConfigure_f)malloc(sizeof(XMFileConfigure_f));

    if(!pf) return NULL;

    pf->Show = XMFileConfigureShow;
    pf->Clear = XMFileConfigureClear;
    pf->Read = XMFileConfigureRead;
    pf->Write = XMFileConfigureWrite;

    return pf;
}

void XMFileConfigureFunDestroy(pXMFileConfigure_f pf)
{
    if(!!pf) return;

    free(pf);

    return ;
}