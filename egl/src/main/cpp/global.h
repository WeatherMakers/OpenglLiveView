#pragma once

#include <napi/native_api.h>
#include <rawfile/raw_file_manager.h>

class Global {
public:
    static napi_value Init(napi_env env, napi_callback_info info);

    static NativeResourceManager *mNativeResMgr;
};