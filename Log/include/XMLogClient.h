#ifndef __XMLOGCLIENT_H__
#define __XMLOGCLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "XMFileConfigure.h"
#include "XMSocketTool.h"
#include <pthread.h>

enum XMLogType_E
{
    XMLOGInfo;
    XMLOGDebug;
    XMLOGWARN;
    XMLOGError;
};

typedef struct XMLog_S XMLog_s, *pXMLog_s;

int _XMLogNew(pXMFileConfigure_s psXMLogConfigure);

static inline pXMLog_s XMLogNew(const char *ConfigPath)
{
    if(!ConfigPath) return NULL;

    XMFileConfigure_F psXMLogConfigure[] = 
    {
        {"LogLevel", 0},
        {"LogMode" , 0},
        {"ServerIP", 0},
        {"ServerPort", 0},
        {"FileName", 0},
        {"FileMaxSize", 0},
        {"FileDurations", 0},
        {"CertificatePath", 0},
        {0,0}
    };

    XMFileConfigureRead(psXMLogConfigure, ConfigurePath);
    
    pXMLog_s ps = _XMLogNew(psXMLogConfigure);

    XMFileConfigureClear(psXMLogConfigure);

    return ps;
}

void XMLogDestroy(pXMLog_s ps);

int XMLogWrite(pXMLog_s ps , const int LogLevel, const int DataLen, const char *ForMat, ...);

#endif