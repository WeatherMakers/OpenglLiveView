#pragma once

#include "EglCore.h"
#include "napi/native_api.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <rawfile/raw_file_manager.h>

namespace hiveVG
{
    class EglRender
    {
    public:
        EglRender();
        ~EglRender();
    
        void   Export(napi_env env, napi_value exports);
        static EglRender* getInstance();
        static napi_value setRenderType(napi_env env, napi_callback_info info);
    
        EglCore* m_pEglCore;
        OH_NativeXComponent_Callback Callback;
    
    private:     
        static EglRender* m_pInstance;
    };
}
