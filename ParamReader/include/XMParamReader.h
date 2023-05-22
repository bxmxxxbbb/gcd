#ifndef __XMPARAMREADER_H__
#define __XMPARAMREADER_H__

#define XMParamReaderNULL 0
#define XMParamReaderSeparator 1

typedef struct XMParamReader_S
{
    char *Name;
    char *Value;
    char *Help;
    short Type;
}XMParamReader_s, *pXMParamReader_s;

#define XMParamReaderForEach(psXMParamReader, it)\
    for (it = psXMParamReader; it->Name || it->Value || it->Type; ++it)

/**
 * @brief 
 * 
 * @param psXMParamReader
 * @param FilePath
 * @return int
 */
int XMParamReaderByFile(pXMParamReader_s psXMParamReader, const char *FilePath);

/**
 * @brief 
 * 
 * @param psXMParamReader
 * @param BufferCount
 * @param Buffer
 * @return int
 */
int XMParamReader(pXMParamReader_s psXMParamReader, const int BufferCount, const char **Buffer);

/**
 * @brief 
 * 
 * @param psXMParamReader
 */
void XMParamReaderHelp(pXMParamReader_s psXMParamReader);

/**
 * @brief 
 * 
 * @param psXMParamReader
 */
void XMParamReaderShow(pXMParamReader_s psXMParamReader);

typedef struct  XMParamReader_F
{
    void (*Help)(pXMParamReader_s psXMParamReader);
    void (*Clear)(pXMParamReader_s psXMParamReader);

    int (*ReadByFile)(pXMParamReader_s psXMParamReader, const char *FilePath);
    int (*Read)(pXMParamReader_s psXMParamReader, const int BufferCount, const char **Buffer);
}XMParamReader_f, *pXMParamReader_f;

pXMParamReader_f XMParamReaderFunInit();

void XMParamReaderFunDestroy(pXMParamReader_f);
#endif