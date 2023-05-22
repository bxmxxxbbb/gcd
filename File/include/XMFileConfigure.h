#ifndef __XNFILECOFIGURE_H__
#define __XNFILECOFIGURE_H__

typedef struct XMFileConfigure_S
{
    char *Name;
    char *Value;
}XMFileConfigure_s, *pXMFileConfigure_s;

#define XMFileConfigureForEach(psXMFileConfigure,it) \
    for(it = psXMFileConfigure; it->Name || it->Value; ++it)

/**
 * @brief 
 * 
 * @param psXMFileConfigure
 * @param FilePath
 * @return int
 */
int XMFileConfigureRead(pXMFileConfigure_s psXMFileConfigure, const char *FilePath);

/**
 * @brief 
 * 
 * @param psXMFileConfigure
 * @param FilePath
 * @return  int 
 */
int XMFileConfigureWrite(pXMFileConfigure_s psXMFileConfigure,const char *FilePath);

/**
 * @brief 
 * 
 * @param psXMFileConfigure
 */
void XMFileConfigureClear(pXMFileConfigure_s psXMFileConfigure);

/**
 * @brief 
 * 
 * @param psXMFileConfigure
 */
void XMFileConfigureShow(pXMFileConfigure_s psXMFileConfigure);

typedef struct XMFileConfigure_F
{
    void (*Show)(pXMFileConfigure_s psXMFileConfigure);
    void (*Clear)(pXMFileConfigure_s psXMFileConfigure);
    int (*Read)(pXMFileConfigure_s psXMFileConfigure, const char *FilePath);
    int (*Write)(pXMFileConfigure_s psXMFileConfigure, const char *FilePath);
}XMFileConfigure_f, *pXMFileConfigure_f;

void XMFileConfigureFunDestroy(pXMFileConfigure_f);
#endif