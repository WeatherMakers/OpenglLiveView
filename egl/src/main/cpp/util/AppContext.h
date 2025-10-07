#pragma once

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
    
    private:
        static NativeResourceManager* m_pNativeResManager;
    };
}