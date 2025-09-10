//
// Created on 2024/8/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_EGLRENDER_H
#define OPENGL_EGLRENDER_H

#include "EglCore.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "napi/native_api.h"
#include <string>
#include <rawfile/raw_file_manager.h>
using namespace std;
class EglRender {
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
    static napi_value setContext(napi_env env, napi_callback_info info);
};

#endif //OPENGL_EGLRENDER_H
