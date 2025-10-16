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
            LOGE(TAG_KEYWORD::FILE_UTILS_TAG, "NativeResourceManager is not initialized");
            return nullptr;
        }
        
        RawFile* pRawFile = OH_ResourceManager_OpenRawFile(pNativeResManager, vPath);
        if (!pRawFile)
        {
            LOGE(TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open resource file: %{public}s", vPath);
            return nullptr;
        }
        return pRawFile;
    }
    else
    {
        FILE* pFile = fopen(vPath, "rb");
        if (!pFile)
        {
            LOGE(TAG_KEYWORD::FILE_UTILS_TAG, "Failed to open file: %{public}s", vPath);
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

bool CFileUtils::readFileToBuffer(const std::string &vFilePath, std::unique_ptr<unsigned char[]> &oBuffer, size_t &oSize)
{
    bool IsReadFromAssetManager = true;
    auto pResource = CFileUtils::openFile(vFilePath.c_str(), IsReadFromAssetManager);
    if (!pResource)
    {
        IsReadFromAssetManager = false;
        pResource = CFileUtils::openFile(vFilePath.c_str(), IsReadFromAssetManager);
    }
    if (!pResource)
    {
        LOGE(TAG_KEYWORD::FILE_UTILS_TAG, "Failed to read file: %{public}s", vFilePath.c_str());
        return false;
    }

    oSize = CFileUtils::getFileBytes(pResource, IsReadFromAssetManager);
    oBuffer = std::make_unique<unsigned char[]>(oSize);
    int Flag = CFileUtils::readFile<unsigned char>(pResource, oBuffer.get(), oSize, IsReadFromAssetManager);
    CFileUtils::closeFile(pResource, IsReadFromAssetManager);
    if (Flag < 0 || oSize == 0)
    {
        LOGE(TAG_KEYWORD::FILE_UTILS_TAG, "Failed to read file: %{public}s", vFilePath.c_str());
        return false;
    }
    return true;
}
