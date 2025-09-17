#include "NativeRenderer.h"
#include "AppContext.h"
#include "log.h"

using namespace hiveVG;

CNativeRenderer *CNativeRenderer::getInstance()
{
    static CNativeRenderer instance;
    return &instance;
}

CNativeRenderer::CNativeRenderer()
{
    m_NativeXComponent = nullptr;
    m_pExample = nullptr;
}

CNativeRenderer::~CNativeRenderer()
{
    __deleteSafely(m_pExample);
}

static void OnFrame(OH_NativeXComponent* component, uint64_t timestamp, uint64_t targetTimestamp)
{
    CNativeRenderer::getInstance()->renderScene(); 
}

napi_value CNativeRenderer::OnCreate(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    OH_NativeXComponent* pComponent = nullptr;
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&pComponent));
    auto Renderer = getInstance();
    Renderer->m_NativeXComponent = pComponent;
    
    Renderer->m_Callback.OnSurfaceCreated = CNativeRenderer::OnSurfaceCreated;
    Renderer->m_Callback.OnSurfaceChanged = CNativeRenderer::OnSurfaceChanged;
    Renderer->m_Callback.OnSurfaceDestroyed = CNativeRenderer::OnSurfaceDestroyed;
    OH_NativeXComponent_RegisterCallback(Renderer->m_NativeXComponent, &Renderer->m_Callback);
    OH_NativeXComponent_RegisterOnFrameCallback(Renderer->m_NativeXComponent, OnFrame);

    LOGI("NativeRenderer::OnCreate: Callbacks registered.");
    return nullptr;
}

// 静态回调函数，当 surface 创建时被系统调用
void CNativeRenderer::OnSurfaceCreated(OH_NativeXComponent *vComponent, void *vWindow)
{
    LOGD("OnSurfaceCreated callback triggered.");
    uint64_t width, height;
    OH_NativeXComponent_GetXComponentSize(vComponent, vWindow, &width, &height);

    auto renderer = getInstance();
    // 初始化 EGL 环境
    if (renderer->EglInitialization(vWindow))
    {
        // EGL 成功后，初始化渲染内容
        renderer->HandleOnSurfaceCreated(vWindow);
        // 设置初始视口
        renderer->HandleOnSurfaceChanged(width, height);
    }
}

void CNativeRenderer::OnSurfaceChanged(OH_NativeXComponent *vComponent, void *vWindow)
{
    LOGD("OnSurfaceChanged callback triggered.");
    uint64_t width, height;
    OH_NativeXComponent_GetXComponentSize(vComponent, vWindow, &width, &height);
    getInstance()->HandleOnSurfaceChanged(width, height);
}

void CNativeRenderer::OnSurfaceDestroyed(OH_NativeXComponent *vComponent, void *vWindow)
{
    LOGD("OnSurfaceDestroyed callback triggered.");
    getInstance()->HandleOnSurfaceDestroyed();
}

void CNativeRenderer::HandleOnSurfaceCreated(void *vWindow)
{
    // TODO:创建并初始化渲染示例

    LOGI("Render example created and initialized.");
}

void CNativeRenderer::HandleOnSurfaceChanged(uint64_t vWidth, uint64_t vHeight)
{
    if (m_EglDisplay != EGL_NO_DISPLAY)
    {
        glViewport(0, 0, vWidth, vHeight);
        LOGI("Viewport updated to %{public}lu x %{public}lu", vWidth, vHeight);
    }
}

void CNativeRenderer::HandleOnSurfaceDestroyed()
{
    if (m_pExample)
    {
        delete m_pExample;
        m_pExample = nullptr;
    }

    EglTermination();
}

void CNativeRenderer::renderScene()
{
}

bool CNativeRenderer::EglInitialization(void *vWindow)
{
    m_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_EglDisplay == EGL_NO_DISPLAY)
    {
        LOGE("Failed to get EGL display.");
        return false;
    }

    EGLint Major, Minor;
    if (!eglInitialize(m_EglDisplay, &Major, &Minor))
    {
        LOGE("Failed to initialize EGL.");
        return false;
    }

    EGLint Attribs[] = {EGL_RENDERABLE_TYPE,
                        EGL_OPENGL_ES3_BIT,
                        EGL_SURFACE_TYPE,
                        EGL_WINDOW_BIT,
                        EGL_RED_SIZE,
                        8,
                        EGL_GREEN_SIZE,
                        8,
                        EGL_BLUE_SIZE,
                        8,
                        EGL_ALPHA_SIZE,
                        8,
                        EGL_DEPTH_SIZE,
                        16,
                        EGL_STENCIL_SIZE,
                        8,
                        EGL_NONE};

    EGLint NumConfigs;
    if (!eglChooseConfig(m_EglDisplay, Attribs, &m_EglConfig, 1, &NumConfigs) || NumConfigs == 0)
    {
        LOGE("Failed to choose EGL config.");
        return false;
    }

    m_EglSurface = eglCreateWindowSurface(m_EglDisplay, m_EglConfig, reinterpret_cast<EGLNativeWindowType>(vWindow), nullptr);
    if (m_EglSurface == EGL_NO_SURFACE)
    {
        LOGE("Failed to create EGL window surface. EGL error: %{public}d", eglGetError());
        return false;
    }

    const EGLint ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    m_EglContext = eglCreateContext(m_EglDisplay, m_EglConfig, EGL_NO_CONTEXT, ContextAttribs);
    if (m_EglContext == EGL_NO_CONTEXT)
    {
        LOGE("Failed to create EGL context. EGL error: %{public}d", eglGetError());
        return false;
    }

    if (!eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext))
    {
        LOGE("Failed to make EGL context current. EGL error: %{public}d", eglGetError());
        return false;
    }

    LOGI("EGL Initialized successfully. Version: %{public}d.%{public}d", Major, Minor);
    return true;
}

void CNativeRenderer::EglTermination()
{
    if (m_EglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_EglContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_EglDisplay, m_EglContext);
            m_EglContext = EGL_NO_CONTEXT;
        }
        if (m_EglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(m_EglDisplay, m_EglSurface);
            m_EglSurface = EGL_NO_SURFACE;
        }
        eglTerminate(m_EglDisplay);
        m_EglDisplay = EGL_NO_DISPLAY;
    }
    LOGI("EGL terminated.");
}

napi_value CNativeRenderer::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"onCreate", nullptr, OnCreate, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setResourceManager", nullptr, CAppContext::setResourceManager, nullptr, nullptr, nullptr, napi_default, nullptr}
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
