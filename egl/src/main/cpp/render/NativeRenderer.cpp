#include "NativeRenderer.h"
#include "AppContext.h"
#include "log.h"

using namespace hiveVG;

NativeRenderer *NativeRenderer::getInstance()
{
    static NativeRenderer instance;
    return &instance;
}

NativeRenderer::NativeRenderer()
{
    m_nativeXComponent = nullptr;
    m_pExample = nullptr;
}

NativeRenderer::~NativeRenderer()
{
    if (m_pExample)
    {
        delete m_pExample;
        m_pExample = nullptr;
    }
}

napi_value NativeRenderer::OnCreate(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    OH_NativeXComponent *component = nullptr;
    // 从 napi_value 中解析出 OH_NativeXComponent 指针
    napi_unwrap(env, argv[0], reinterpret_cast<void **>(&component));

    auto renderer = getInstance();
    renderer->m_nativeXComponent = component;

    // 注册 VSync 帧回调
//    renderer->m_onFrameCallback.onFrame = NativeRenderer::OnFrame;
//    OH_NativeXComponent_RegisterOnFrameCallback(renderer->m_nativeXComponent, &renderer->m_onFrameCallback);

    // 注册生命周期回调
    renderer->m_callback.OnSurfaceCreated = NativeRenderer::OnSurfaceCreated;
    renderer->m_callback.OnSurfaceChanged = NativeRenderer::OnSurfaceChanged;
    renderer->m_callback.OnSurfaceDestroyed = NativeRenderer::OnSurfaceDestroyed;
    OH_NativeXComponent_RegisterCallback(renderer->m_nativeXComponent, &renderer->m_callback);

    LOGI("NativeRenderer::OnCreate: Callbacks registered.");
    return nullptr;
}

// 静态回调函数，当 surface 创建时被系统调用
void NativeRenderer::OnSurfaceCreated(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceCreated callback triggered.");
    uint64_t width, height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);

    auto renderer = getInstance();
    // 初始化 EGL 环境
    if (renderer->EglInitialization(window))
    {
        // EGL 成功后，初始化渲染内容
        renderer->HandleOnSurfaceCreated(window);
        // 设置初始视口
        renderer->HandleOnSurfaceChanged(width, height);
    }
}

void NativeRenderer::OnSurfaceChanged(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceChanged callback triggered.");
    uint64_t width, height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    getInstance()->HandleOnSurfaceChanged(width, height);
}

void NativeRenderer::OnSurfaceDestroyed(OH_NativeXComponent *component, void *window)
{
    LOGD("OnSurfaceDestroyed callback triggered.");
    getInstance()->HandleOnSurfaceDestroyed();
}

void NativeRenderer::OnFrame(OH_NativeXComponent *component, void *window)
{
    getInstance()->HandleOnFrame();
}

// 处理 OnSurfaceCreated 的成员函数
void NativeRenderer::HandleOnSurfaceCreated(void *window)
{
    // 创建并初始化渲染示例
    m_pExample = new CSinglePlayerExample();
    if (!m_pExample || !m_pExample->init())
    {
        LOGE("Failed to create or init example.");
        if (m_pExample)
        {
            delete m_pExample;
            m_pExample = nullptr;
        }
        return;
    }
    LOGI("Render example created and initialized.");
}

void NativeRenderer::HandleOnSurfaceChanged(uint64_t width, uint64_t height)
{
    if (m_eglDisplay != EGL_NO_DISPLAY)
    {
        glViewport(0, 0, width, height);
        LOGI("Viewport updated to %{public}lu x %{public}lu", width, height);
    }
}

void NativeRenderer::HandleOnSurfaceDestroyed()
{
    if (m_pExample)
    {
        delete m_pExample;
        m_pExample = nullptr;
    }

    EglTermination();
}

void NativeRenderer::HandleOnFrame()
{
    if (m_pExample != nullptr && m_eglDisplay != EGL_NO_DISPLAY)
    {
        m_pExample->draw();

        if (!eglSwapBuffers(m_eglDisplay, m_eglSurface))
        {
            LOGE("eglSwapBuffers failed. EGL error: %{public}d", eglGetError());
        }
    }
}

bool NativeRenderer::EglInitialization(void *window)
{
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_eglDisplay == EGL_NO_DISPLAY)
    {
        LOGE("Failed to get EGL display.");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(m_eglDisplay, &major, &minor))
    {
        LOGE("Failed to initialize EGL.");
        return false;
    }

    EGLint attribs[] = {EGL_RENDERABLE_TYPE,
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

    EGLint numConfigs;
    if (!eglChooseConfig(m_eglDisplay, attribs, &m_eglConfig, 1, &numConfigs) || numConfigs == 0)
    {
        LOGE("Failed to choose EGL config.");
        return false;
    }

    m_eglSurface =
        eglCreateWindowSurface(m_eglDisplay, m_eglConfig, reinterpret_cast<EGLNativeWindowType>(window), nullptr);
    if (m_eglSurface == EGL_NO_SURFACE)
    {
        LOGE("Failed to create EGL window surface. EGL error: %{public}d", eglGetError());
        return false;
    }

    const EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, contextAttribs);
    if (m_eglContext == EGL_NO_CONTEXT)
    {
        LOGE("Failed to create EGL context. EGL error: %{public}d", eglGetError());
        return false;
    }

    if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
    {
        LOGE("Failed to make EGL context current. EGL error: %{public}d", eglGetError());
        return false;
    }

    LOGI("EGL Initialized successfully. Version: %{public}d.%{public}d", major, minor);
    return true;
}

void NativeRenderer::EglTermination()
{
    if (m_eglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_eglContext != EGL_NO_CONTEXT)
            eglDestroyContext(m_eglDisplay, m_eglContext);
        if (m_eglSurface != EGL_NO_SURFACE)
            eglDestroySurface(m_eglDisplay, m_eglSurface);
        eglTerminate(m_eglDisplay);

        m_eglDisplay = EGL_NO_DISPLAY;
        m_eglContext = EGL_NO_CONTEXT;
        m_eglSurface = EGL_NO_SURFACE;
        m_eglConfig = nullptr;
    }
    LOGI("EGL terminated.");
}

napi_value NativeRenderer::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"onCreate", nullptr, OnCreate, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setResourceManager", nullptr, CAppContext::setResourceManager, nullptr, nullptr, nullptr, napi_default,
         nullptr}};
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
