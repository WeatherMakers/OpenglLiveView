#include "EglRender.h"
#include "ScreenQuad.h"
#include "log.h"

using namespace hiveVG;

EglRender *EglRender::m_pInstance = nullptr;

EglRender::~EglRender()
{
    if (m_pEglCore)
    {
        delete m_pEglCore;
        m_pEglCore = nullptr;
    }
}

static void OnFrame(OH_NativeXComponent *component, uint64_t timestamp, uint64_t targetTimestamp)
{
    EglRender::getInstance()->m_pEglCore->renderScene();
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
    CScreenQuad::getOrCreate();
    // 注册帧回调，每帧都会调用 OnFrame 函数
    OH_NativeXComponent_RegisterOnFrameCallback(component, OnFrame);
}

void OnSurfaceChanged(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceChanged - 屏幕尺寸变化");
    if (nullptr == component || nullptr == window)
    {
        LOGE("component or window is null in OnSurfaceChanged");
        return;
    }
    uint64_t Width;
    uint64_t Height;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentSize(component, window, &Width, &Height))
    {
        LOGE("获取新尺寸失败");
        return;
    }
    LOGD("OnSurfaceChanged - 新尺寸: %{public}ld x %{public}ld", Width, Height);
}

void OnSurfaceDestroyed(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceDestroyed");
    EglRender *m_pInstance = EglRender::getInstance();
    if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
    CScreenQuad::destroy();
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
    LOGI("执行 Export.");
    napi_value exportInstance = nullptr;
    if (napi_ok != napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance))
    {
        LOGE("解析参数出错");
        return;
    } else
    {
        LOGI("解析参数成功");
    }
    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_ok != napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)))
    {
        LOGE("获取OH_NativeXComponent对象出错");
        return;
    } else
    {
        LOGI("获取OH_NativeXComponent对象成功");
    }
    // 获取id
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t size = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (napi_ok != OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &size))
    {
        LOGE("获取XComponentId出错");
        return;
    } 
    else
    {
        LOGI("获取XComponentId成功");
    }
    // 注册回调
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &Callback);
}

napi_value EglRender::setRenderType(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    int params;
    napi_get_value_int32(env, args[0], &params);
    EglCore *m_pEglCore = EglRender::getInstance()->m_pEglCore;
    m_pEglCore->setRenderType(params);
    return nullptr;
}