#pragma once
#include "log.h"
#include <cstdio>
#include <rawfile/raw_file_manager.h>

struct RawFile;

namespace hiveVG
{
    class CFileUtils
    {
    public:
        static void*  openFile(const char* vPath, bool vIsLoadFromResource = true);
        static size_t getFileBytes(void* vFile, bool vIsLoadFromResource = true);
        static void   closeFile(void* vFile, bool vIsLoadFromResource = true);

        template <typename T>
        static int readFile(void* vFile, T* vBuffer, size_t vElementCount, bool vIsLoadFromResource = true);
    };

    template <typename T>
    int CFileUtils::readFile(void* vFile, T* vBuffer, size_t vElementCount, bool vIsReadFromResource)
    {
        if (!vFile || !vBuffer)
        {
            LOGE("Invalid parameters.");
            return -1;
        }
        
        if (vIsReadFromResource)
        {
            RawFile* pRawFile = static_cast<RawFile*>(vFile);
            size_t bytesToRead = vElementCount * sizeof(T);
            long bytesRead = OH_ResourceManager_ReadRawFile(pRawFile, vBuffer, bytesToRead);
            if (bytesRead < 0)
            {
                LOGE("Failed to read from resource file");
                return -1;
            }
            return static_cast<int>(bytesRead / sizeof(T));
        }
        else
        {
            FILE* pFile = static_cast<FILE*>(vFile);
            size_t elementsRead = fread(vBuffer, sizeof(T), vElementCount, pFile);
            if (elementsRead != vElementCount && ferror(pFile))
            {
                LOGE("Failed to read from file");
                return -1;
            }
            return static_cast<int>(elementsRead);
        }
    }
}