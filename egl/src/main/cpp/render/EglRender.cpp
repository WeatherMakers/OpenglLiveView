#include "EglRender.h"
#include "example/BaseExample.h"
#include "example/ImageExample.h"
#include "example/TriangleExample.h"
#include "log.h"

using namespace hiveVG;

EglRender* EglRender::m_pInstance;

EglRender::~EglRender()
{
    LOGD("执行EglRender析构函数");
    if (m_pEglCore)
    {
        delete m_pEglCore;
        m_pEglCore = nullptr;
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
    EglRender::getInstance()->m_pEglCore->initEglContext(window, width, height);
}

void OnSurfaceChanged(OH_NativeXComponent *component, void *window) { LOGD("OnSurfaceChanged"); }

void OnSurfaceDestroyed(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceDestroyed");
    EglRender *m_pInstance = EglRender::getInstance();
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

EglRender::EglRender()
{
    m_pEglCore = new EglCore();
    Callback.OnSurfaceCreated = OnSurfaceCreated;
    Callback.OnSurfaceChanged = OnSurfaceChanged;
    Callback.OnSurfaceDestroyed = OnSurfaceDestroyed;
}

EglRender *EglRender::getInstance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new EglRender();
    }
    return m_pInstance;
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
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &Callback);
}

napi_value EglRender::setParams(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    int params;
    napi_get_value_int32(env, args[0], &params);
    EglCore *m_pEglCore = EglRender::getInstance()->m_pEglCore;
    m_pEglCore->setParams(params);
    return nullptr;
}