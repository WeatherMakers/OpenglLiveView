#include "AppContext.h"
#include "log.h"

namespace hiveVG
{
    std::mutex CAppContext::m_AppMutex;
    AAssetManager* CAppContext::m_pAssetManager = nullptr;
    std::string CAppContext::m_StoragePath = "";

    void CAppContext::setAssetManager(void *vAssetManager)
    {
//        std::lock_guard<std::mutex> Lock(m_AppMutex);
//        m_pAssetManager = static_cast<AAssetManager*>(vAssetManager);
    }

    void* CAppContext::getAssetManager()
    {
        if (!m_pAssetManager)
            LOGE("Asset manager does not exist.");
        return m_pAssetManager;
    }

    void CAppContext::setStoragePath(const std::string &vPath)
    {
        std::string StoragePath = vPath;
        if (StoragePath.back() != '/') {
            StoragePath += '/';
        }
        m_StoragePath = StoragePath;
    }

    std::string CAppContext::getStoragePath()
    {
        if(m_StoragePath.empty())
            LOGE("Storage path does not set.");
        return m_StoragePath;
    }
}