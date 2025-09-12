#pragma once

#include "EglCore.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "napi/native_api.h"
#include <rawfile/raw_file_manager.h>

class EglRender 
{
private:
    static EglRender* instance;
    
public:
    EglCore* eglCore;
    void Export(napi_env env, napi_value exports);
    EglRender();
    ~EglRender();
    OH_NativeXComponent_Callback callback;
    napi_env env;
    NativeResourceManager* resourceManager;
    int currentImageMode;
    static EglRender* getInstance();
    static napi_value setParams(napi_env env, napi_callback_info info);
    static napi_value setImage(napi_env env, napi_callback_info info);
};
