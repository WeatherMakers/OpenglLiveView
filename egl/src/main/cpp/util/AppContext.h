#pragma once

#include <mutex>

class AAssetManager;

namespace hiveVG
{
    class CAppContext
    {
    public:
        CAppContext() = delete;
        ~CAppContext() = delete;

        static void  setAssetManager(void *vAssetManager);
        static void* getAssetManager();
        static void setStoragePath(const std::string &vPath);
        static std::string getStoragePath();

    private:
        static std::mutex     m_AppMutex;
        static AAssetManager* m_pAssetManager;
        static std::string    m_StoragePath;
    };
}