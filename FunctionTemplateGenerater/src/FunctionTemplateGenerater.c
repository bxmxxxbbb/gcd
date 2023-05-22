#include "FunctionTemplateGenerater.h"
#include "XMString.h"

#define CP printf("checkpoint %d\n",__LINE__);


int main(int argc, char *argv[])
{
    int rv = 0;

    if(argc < 3)
    {
            printf("FirstParam:JsonFilePath, SecondParam:GeneratePath\n");
            return 0;
    }

    char *FileName = NULL;
    char *JsonData = NULL;
    cJSON *Json = NULL;
    pXMString_s psHStr = NULL;
    pXMString_s psFilePath = NULL;
    do
    {
        rv = XMFileRWReader(argv[1] , &JsonData);
        if(rv < 0) {printf("ERROR: XMFileRWReader %d\n",__LINE__); break;}

        Json = cJSON_Parse(JsonData);
        if(!Json) {printf("ERROR: cJSON_Parse %d\n",__LINE__); break;}

        {
            cJSON *tmp = cJSON_GetObjectItem(Json, "FileName");
            if(!tmp) {printf("ERROR: cJSON_GetObjectItem FileName %d\n",__LINE__); break;}

            FileName = cJSON_GetStringValue(tmp);
            if(!FileName) { printf("ERROR: cJSON_GetStringValue FileName %d\n",__LINE__); break;}
        }
        {
            psHStr = XMStringNew();
            if(!psHStr) if(!FileName) { printf("ERROR: XMString %d\n",__LINE__); break;}

            XMStringAppend(psHStr, "#ifndef __%s_H__\n", FileName);
            XMStringAppend(psHStr, "#define __%s_H__\n\n",FileName);

            XMStringAppend(psHStr, "#ifdef __cplusplus\nextern \"C\"\n{\n#endif\n\n",FileName);

            cJSON *StructList = cJSON_GetObjectItem(Json, "Struct");
            if(StructList)
            {
                cJSON *Struct = NULL;
                cJSON_ArrayForEach(Struct, StructList)
                {
                    // if(!cJSON_IsArray(Struct)) continue;

                    cJSON *Name = cJSON_GetObjectItem(Struct, "Name");
                    if(!Name) {printf("ERROR: cJSON_GetObjectItem %d\n", __LINE__); continue;}

                    cJSON *ParamList = cJSON_GetObjectItem(Struct, "Param");
                    if(!ParamList) {printf("ERROR: cJSON_GetObjectItem %d\n",__LINE__); continue;}

                    XMStringAppend(psHStr, "typedef struct %s_S\n",Name->valuestring);
                    XMStringAppend(psHStr, "{\n");

                    cJSON *Param = NULL;
                    cJSON_ArrayForEach(Param, ParamList)
                    {
                        cJSON *Type = cJSON_GetObjectItem(Param, "Type");
                        XMStringAppend(psHStr, "    %s", Type->valuestring);

                        cJSON *Name = cJSON_GetObjectItem(Param, "Name");
                        if(psHStr->pcBuffer[psHStr->iBufferLen - 1] != '*') XMStringAppend(psHStr," ");
                        XMStringAppend(psHStr,"%s;\n",Name->valuestring);
                    
                    }

                    XMStringAppend(psHStr, "}%s_s, *p%s_s;\n\n",Name->valuestring, Name->valuestring);
                }
            }

            cJSON *FunctionList = cJSON_GetObjectItem(Json, "Function");
            if(FunctionList)
            {
                cJSON *Function = NULL;
                cJSON_ArrayForEach(Function,FunctionList)
                {
                    cJSON *ReturnType = cJSON_GetObjectItem(Function, "ReturnType");
                    if(!ReturnType) {printf("ERROR: cJSON_GetObjectItem %d\n", __LINE__); continue;}

                    cJSON *Name = cJSON_GetObjectItem(Function, "Name");
                    if(!Name) {printf("ERROR: cJSON_GetObjectItem %d\n", __LINE__); continue;}

                    cJSON *ParamList = cJSON_GetObjectItem(Function, "Param");
                    if(!ParamList) {printf("ERROR: cJSON_GetObjectItem %d\n", __LINE__); continue;}

                    XMStringAppend(psHStr,"%s %s%s(",ReturnType->valuestring, FileName, Name->valuestring);

                    int flag = 0;
                    cJSON *Param = NULL;
                    cJSON_ArrayForEach(Param,ParamList);
                    {
                        cJSON *Type = cJSON_GetObjectItem(Param, "Type");
                        if(!flag++)
                            XMStringAppend(psHStr, "%s",Type->valuestring);
                        else    
                            XMStringAppend(psHStr, ",%s",Type->valuestring);
                        
                        cJSON *Name = cJSON_GetObjectItem(Param, "Name");
                        if(psHStr->pcBuffer[psHStr->iBufferLen - 1] != '*') XMStringAppend(psHStr," ");
                        XMStringAppend(psHStr,"%s",Name->valuestring);
                    }
                    XMStringAppend(psHStr,");\n\n");
                }
            }
            FunctionList = cJSON_GetObjectItem(Json, "Function");
            if(FunctionList)
            {
                XMStringAppend(psHStr, "typedef struct %s_F\n{\n",FileName);

                cJSON *Function = NULL;
                cJSON_ArrayForEach(Function,FunctionList);
                {
                    cJSON *ReturnType = cJSON_GetObjectItem(Function, "ReturnType");
                    if(!ReturnType) {printf("ERROR: cJSON_GetObjectItem %d\n",__LINE__); continue;}
                    
                    cJSON *Name = cJSON_GetObjectItem(Function, "Name");
                    if(!Name) {printf("ERROR: cJSON_GetObjectItem %d\n",__LINE__); continue;}

                    cJSON *ParamList = cJSON_GetObjectItem(Function , "Param");
                    if(!ParamList) {printf("ERROR: cJSON_GetObjectItem %d\n",__LINE__); continue;}

                XMStringAppend(psHStr,"    %s (*%s)(",ReturnType->valuestring, Name->valuestring);

                int flag = 0;
                cJSON *Param = NULL;
                cJSON_ArrayForEach(Param, ParamList)
                {
                    cJSON *Type = cJSON_GetObjectItem(Param , "Type");
                    if(!flag++)
                        XMStringAppend(psHStr, "%s" ,Type->valuestring);
                    else 
                        XMStringAppend(psHStr, ", %s",Type->valuestring); 
                    
                    cJSON *Name = cJSON_GetObjectItem(Param,"Name");
                    if(psHStr->pcBuffer[psHStr->iBufferLen - 1] != '*') XMStringAppend(psHStr," ");
                    XMStringAppend(psHStr, "%s",Name->valuestring);
                }
                XMStringAppend(psHStr,");\n");
                }
                XMStringAppend(psHStr,"}%s_f, *p%s_f;\n\n",FileName,FileName);
            }
            XMStringAppend(psHStr,"#ifdef __cplusplus\n}\n#endif\n\n",FileName);
            XMStringAppend(psHStr,"#endif\n",FileName);

            psFilePath = XMStingNew();
            if(!psFilePath) {printf("ERROR:XMStringNew %d\n",__LINE__);break;}

            XMStringSet(psFilePath,"%s%s.h",argv[2],FileName);

            rv = XMStringRWWrite(psFilePath->pcBuffer,psHStr->pcBuffer);
            if(rv < 0){printf("ERROR :  XMFileRWWrite %d\n",__LINE__); break;}
        }
    }while(0);
    
    return 0;
} 