#pragma once

#include <mutex>
#include <rawfile/raw_file_manager.h>

class AAssetManager;

namespace hiveVG
{
    class CAppContext
    {
    public:
        CAppContext() = delete;
        ~CAppContext() = delete;
        
        static napi_value setResourceManager(napi_env env, napi_callback_info info);
        static NativeResourceManager* getResourceManager();
        static void  setStoragePath(const std::string &vPath);
        static std::string getStoragePath();
    
    private:
        static std::mutex     m_AppMutex;
        static NativeResourceManager* m_pNativeResManager;
        static std::string    m_StoragePath;
    };
}