//
// Created on 2024/8/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "EglRender.h"
#include "example/BaseExample.h"
#include "example/TriangleExample.h"
#include "log.h"
#include "example/TriangleExample.h"

EglRender* EglRender::instance;

EglRender::~EglRender() {
    LOGD("执行EglRender析构函数");
    if (eglCore) {
        delete eglCore;
        eglCore = nullptr;
    }
}

void OnSurfaceCreated(OH_NativeXComponent *component, void *window) {
    LOGD("OnSurfaceCreated");
    if (nullptr == component || nullptr == window) {
        LOGE("component or window is null");
        return;
    }
    uint64_t width;
    uint64_t height;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentSize(component, window, &width, &height)) {
        LOGE("获取尺寸失败");
        return;
    }
    EglRender::getInstance()->eglCore->EglContextInit(window, width, height);
}

void OnSurfaceChanged(OH_NativeXComponent *component, void *window) {
    LOGD("OnSurfaceChanged");
}

void OnSurfaceDestroyed(OH_NativeXComponent *component, void *window) {
    LOGD("OnSurfaceDestroyed");
    EglRender* instance = EglRender::getInstance();
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

EglRender::EglRender() {
    eglCore = new EglCore();
    callback.OnSurfaceCreated = OnSurfaceCreated;
    callback.OnSurfaceChanged = OnSurfaceChanged;
    callback.OnSurfaceDestroyed = OnSurfaceDestroyed;
}

EglRender* EglRender::getInstance() {
    if (instance == nullptr) {
        instance = new EglRender();
    }
    return instance;
}

void EglRender::Export(napi_env env, napi_value exports) {
    napi_value exportInstance = nullptr;
    if (napi_ok != napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance)) {
        LOGE("解析参数出错");
        return;
    }
    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_ok != napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent))) {
        LOGE("获取OH_NativeXComponent对象出错");
        return;
    }
    // 获取id
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t size = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (napi_ok != OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &size)) {
        LOGE("获取XComponentId出错");
        return;
    }
    // 注册回调
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &callback);
}

napi_value EglRender::setParams(napi_env env, napi_callback_info info) {
    // 声明参数的个数，由于ArkTS会传递过来一个参数，参数的个数就为1
    size_t argc = 1;
    // 声明参数数组
    napi_value args[1] = {nullptr};
    // 将ArkTS传入的参数并依次放入参数数组中
    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);
    // 获取参数
    int params;
    napi_get_value_int32(env, args[0], &params);
    EglCore* eglCore = EglRender::getInstance()->eglCore;
    eglCore->setParams(params);
    return nullptr;
}