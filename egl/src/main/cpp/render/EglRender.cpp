//
// Created on 2024/8/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "EglRender.h"
#include "example/BaseExample.h"
#include "example/TriangleExample.h"
#include "example/ImageExample.h"
#include "log.h"

EglRender *EglRender::instance;

EglRender::~EglRender()
{
    LOGD("执行EglRender析构函数");
    if (eglCore)
    {
        delete eglCore;
        eglCore = nullptr;
    }
}

void OnSurfaceCreated(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceCreated");
    if (nullptr == component || nullptr == window)
    {
        LOGE("component or window is null");
        return;
    }
    uint64_t width;
    uint64_t height;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentSize(component, window, &width, &height))
    {
        LOGE("获取尺寸失败");
        return;
    }
    EglRender::getInstance()->eglCore->EglContextInit(window, width, height);
}

void OnSurfaceChanged(OH_NativeXComponent *component, void *window) { LOGD("OnSurfaceChanged"); }

void OnSurfaceDestroyed(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceDestroyed");
    EglRender *instance = EglRender::getInstance();
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

EglRender::EglRender()
{
    eglCore = new EglCore();
    callback.OnSurfaceCreated = OnSurfaceCreated;
    callback.OnSurfaceChanged = OnSurfaceChanged;
    callback.OnSurfaceDestroyed = OnSurfaceDestroyed;
    resourceManager = nullptr;
    currentImageMode = 0;
}

EglRender *EglRender::getInstance()
{
    if (instance == nullptr)
    {
        instance = new EglRender();
    }
    return instance;
}

void EglRender::Export(napi_env env, napi_value exports)
{
    // 缓存env以便后续在C++层读取rawfile
    this->env = env;
    napi_value exportInstance = nullptr;
    if (napi_ok != napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance))
    {
        LOGE("解析参数出错");
        return;
    }
    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_ok != napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)))
    {
        LOGE("获取OH_NativeXComponent对象出错");
        return;
    }
    // 获取id
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t size = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (napi_ok != OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &size))
    {
        LOGE("获取XComponentId出错");
        return;
    }
    // 注册回调
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &callback);
}
napi_value EglRender::setContext(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        return nullptr;
    }
    // 初始化原生资源管理器
    NativeResourceManager* rm = OH_ResourceManager_InitNativeResourceManager(env, args[0]);
    if (rm == nullptr) {
        LOGE("InitNativeResourceManager 失败");
        return nullptr;
    }
    EglRender::getInstance()->resourceManager = rm;
    return nullptr;
}

napi_value EglRender::setParams(napi_env env, napi_callback_info info)
{
    // 声明参数的个数，由于ArkTS会传递过来一个参数，参数的个数就为1
    size_t argc = 1;
    // 声明参数数组
    napi_value args[1] = {nullptr};
    // 将ArkTS传入的参数并依次放入参数数组中
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    // 获取参数
    int params;
    napi_get_value_int32(env, args[0], &params);
    
    // 记录当前图像模式
    EglRender::getInstance()->currentImageMode = params;
    LOGD("setParams: 设置图像模式为 %d", params);
    
    EglCore *eglCore = EglRender::getInstance()->eglCore;
    eglCore->setParams(params);
    return nullptr;
}

napi_value EglRender::setImage(napi_env env, napi_callback_info info)
{
    LOGD("EglRender::setImage 开始");
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    if (argc < 1) {
        LOGE("setImage: 参数不足");
        return nullptr;
    }
    // 获取ArrayBuffer数据
    void* data = nullptr;
    size_t byteLength = 0;
    napi_get_arraybuffer_info(env, args[0], &data, &byteLength);
    LOGD("setImage: 获取到数据 data=%p, byteLength=%zu", data, byteLength);
    if (data == nullptr || byteLength == 0) {
        LOGE("setImage: 数据为空");
        return nullptr;
    }
    // 交给EglCore的示例去处理（若不是图像示例，则临时创建）
    EglCore* core = EglRender::getInstance()->eglCore;
    if (core == nullptr) {
        LOGE("setImage: eglCore为空");
        return nullptr;
    }
    if (core->example == nullptr) {
        LOGD("setImage: 创建图像示例");
        core->setParams(IMAGE_TYPE);
    }
    // 这里不直接暴露example类型，使用虚方法扩展较合适。
    // 为简化，动态转换并调用（确保已是CImageExample）。
    CImageExample* img = dynamic_cast<CImageExample*>(core->example);
    if (img) {
        LOGD("setImage: 调用setImageFromMemory");
        img->setImageFromMemory(reinterpret_cast<unsigned char*>(data), static_cast<int>(byteLength));
        LOGD("setImage: 调用draw");
        img->draw();
        LOGD("setImage: 调用present");
        core->present();
        LOGD("setImage: 完成");
    } else {
        LOGE("setImage: 动态转换失败，不是CImageExample");
    }
    return nullptr;
}