#include "FileUtils.h"
#include "Common.h"
#include "log.h"
#include "AppContext.h"

using namespace hiveVG;

void *CFileUtils::openFile(const char *vPath, bool vIsLoadFromResource)
{
    if(vIsLoadFromResource)
    {
        auto* pNativeResManager = static_cast<NativeResourceManager*>(CAppContext::getResourceManager());
        if (!pNativeResManager)
        {
            LOGE("NativeResourceManager is not initialized");
            return nullptr;
        }
        
        RawFile* pRawFile = OH_ResourceManager_OpenRawFile(pNativeResManager, vPath);
        if (!pRawFile)
        {
            LOGE("Failed to open resource file: %{public}s", vPath);
            return nullptr;
        }
        return pRawFile;
    }
    else
    {
        FILE* pFile = fopen(vPath, "rb");
        if (!pFile)
        {
            LOGE("Failed to open file: %{public}s", vPath);
            return nullptr;
        }
        return pFile;
    }
}

size_t CFileUtils::getFileBytes(void* vFile, bool vIsLoadFromResource)
{
    if(!vFile)
        return 0;
    if(vIsLoadFromResource)
    {
        RawFile* pRawFile = static_cast<RawFile*>(vFile);
        long fileSize = OH_ResourceManager_GetRawFileSize(pRawFile);
        return static_cast<size_t>(fileSize);
    }
    else
    {
        FILE* pFile = static_cast<FILE*>(vFile);

        fseek(pFile, 0, SEEK_END);
        long Size = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);
        return static_cast<size_t>(Size);
    }
}

void CFileUtils::closeFile(void* vFile, bool vIsLoadFromResource)
{
    if(!vFile)
        return;
    if(vIsLoadFromResource)
        OH_ResourceManager_CloseRawFile(static_cast<RawFile*>(vFile));
    else
        fclose(static_cast<FILE*>(vFile));
}