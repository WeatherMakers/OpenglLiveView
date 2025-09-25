#include "NativeRenderer.h"
#include "AppContext.h"
#include "log.h"
#include "example/BaseRenderer.h"
#include "example/SeqPlayerRenderer.h"
#include "example/SinglePlayerRenderer.h"
#include "example/RainSceneRenderer.h"
#include "example/SnowSceneRenderer.h"
#include "example/CloudSceneRenderer.h"
#include "example/FullSceneRenderer.h"

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

CNativeRenderer::~CNativeRenderer() { __deleteSafely(m_pExample); }
napi_value CNativeRenderer::TriggerLightning(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerLightning called.");
    return nullptr;
}

napi_value CNativeRenderer::TriggerCloud(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerCloud called.");
    return nullptr;
}

napi_value CNativeRenderer::TriggerLightRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerLightRain called.");
    return nullptr;
}

napi_value CNativeRenderer::TriggerModerateRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerModerateRain called.");
    return nullptr;
}

napi_value CNativeRenderer::TriggerHeavyRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerHeavyRain called.");
    return nullptr;
}

napi_value CNativeRenderer::TriggerStormRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerStormRain called.");
    return nullptr;
}

napi_value CNativeRenderer::SetRenderType(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    int32_t Type = 0;
    napi_get_value_int32(env, argv[0], &Type);
    auto Renderer = getInstance();
    Renderer->m_CurrentType = Type;

    // 若已有缓存，直接切换当前引用；否则在合适时机构建
    auto it = Renderer->m_TypeToExample.find(Type);
    if (it != Renderer->m_TypeToExample.end())
    {
        Renderer->m_pExample = it->second;
    } 
    else if (Renderer->m_ContextReady)
    {
        CBaseRenderer *pExample = nullptr;
        switch (Type)
        {
        case SINGLE_RENDER_TYPE:
            pExample = new CSinglePlayerRenderer();
            break;
        case SEQ_RENDER_TYPE:
            pExample = new CSeqPlayerRenderer();
            break;
        case RAIN_RENDER_TYPE:
            pExample = new CRainSceneRenderer();
            break;
        case SNOW_RENDER_TYPE:
            pExample = new CSnowSceneRenderer();
            break;
        case CLOUD_RENDER_TYPE:
            pExample = new CCloudSceneRenderer();
            break;
        case FULL_SCENE_RENDER_TYPE:
            pExample = new CFullSceneRenderer();
            break;
        default:
            pExample = new CSinglePlayerRenderer();
            break;
        }
        if (pExample && pExample->init())
        {
            Renderer->m_TypeToExample[Type] = pExample;
            Renderer->m_pExample = pExample;
        } 
        else
        {
            LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Init example failed in SetRenderType.");
            if (pExample != nullptr)
            {
                delete pExample;
                pExample = nullptr;
            }
        }
    }
    return nullptr;
}

void CNativeRenderer::renderScene()
{
    if (m_EglDisplay == EGL_NO_DISPLAY || m_EglSurface == EGL_NO_SURFACE)
        return;
    
    EGLint Width, Height;
    eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_WIDTH, &Width);
    eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_HEIGHT, &Height);
    
    int renderWidth = Width;
    int renderHeight = Height / 2;
    int offsetX = 0;
    int offsetY = (Height - renderHeight) / 4 * 3;
    
    glViewport(offsetX, offsetY, renderWidth, renderHeight);
    if (m_pExample)
    {
        m_pExample->draw();
    }
    auto SwapResult = eglSwapBuffers(m_EglDisplay, m_EglSurface);
    assert(SwapResult == EGL_TRUE);
}

static void OnFrame(OH_NativeXComponent *component, uint64_t timestamp, uint64_t targetTimestamp)
{
    CNativeRenderer::getInstance()->renderScene();
}

napi_value CNativeRenderer::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"setResourceManager", nullptr, CAppContext::setResourceManager, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setRenderType", nullptr, SetRenderType, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerLightning", nullptr, TriggerLightning, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerCloud", nullptr, TriggerCloud, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerLightRain", nullptr, TriggerLightRain, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerModerateRain", nullptr, TriggerModerateRain, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerHeavyRain", nullptr, TriggerHeavyRain, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerStormRain", nullptr, TriggerStormRain, nullptr, nullptr, nullptr, napi_default, nullptr}
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    napi_value exportInstance = nullptr;
    if (napi_ok != napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance))
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "解析参数出错");
        return nullptr;
    } 
    else
    {
        LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "解析参数成功");
    }
    OH_NativeXComponent *pNativeXComponent = nullptr;
    if (napi_ok != napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&pNativeXComponent)))
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "获取OH_NativeXComponent对象出错");
        return nullptr;
    } 
    else
    {
        LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "获取OH_NativeXComponent对象成功");
    }
    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
    uint64_t size = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (napi_ok != OH_NativeXComponent_GetXComponentId(pNativeXComponent, idStr, &size))
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "获取XComponentId出错");
        return nullptr;
    } 
    else
    {
        LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "获取XComponentId成功");
    }

    auto Renderer = getInstance();
    Renderer->m_NativeXComponent = pNativeXComponent;
    Renderer->m_Callback.OnSurfaceCreated = CNativeRenderer::OnSurfaceCreated;
    Renderer->m_Callback.OnSurfaceChanged = CNativeRenderer::OnSurfaceChanged;
    Renderer->m_Callback.OnSurfaceDestroyed = CNativeRenderer::OnSurfaceDestroyed;
    OH_NativeXComponent_RegisterCallback(Renderer->m_NativeXComponent, &Renderer->m_Callback);
    OH_NativeXComponent_RegisterOnFrameCallback(Renderer->m_NativeXComponent, OnFrame);
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Callbacks registered.");
    return nullptr;
}

// 静态回调函数，当 surface 创建时被系统调用
void CNativeRenderer::OnSurfaceCreated(OH_NativeXComponent *vComponent, void *vWindow)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "OnSurfaceCreated callback triggered.");
    uint64_t Width, Height;
    OH_NativeXComponent_GetXComponentSize(vComponent, vWindow, &Width, &Height);

    auto Renderer = getInstance();
    // 初始化 EGL 环境
    if (Renderer->EglInitialization(vWindow))
    {
        // EGL 成功后，初始化渲染内容
        Renderer->HandleOnSurfaceCreated(vWindow);
        Renderer->HandleOnSurfaceChanged(Width, Height);
    }
}

void CNativeRenderer::OnSurfaceChanged(OH_NativeXComponent *vComponent, void *vWindow)
{
    LOGD(TAG_KEYWORD::NATIVE_RENDERER_TAG, "OnSurfaceChanged callback triggered.");
    uint64_t Width, Height;
    OH_NativeXComponent_GetXComponentSize(vComponent, vWindow, &Width, &Height);
    getInstance()->HandleOnSurfaceChanged(Width, Height);
}

void CNativeRenderer::OnSurfaceDestroyed(OH_NativeXComponent *vComponent, void *vWindow)
{
    LOGD(TAG_KEYWORD::NATIVE_RENDERER_TAG, "OnSurfaceDestroyed callback triggered.");
    getInstance()->HandleOnSurfaceDestroyed();
}

void CNativeRenderer::HandleOnSurfaceCreated(void *vWindow)
{
    if (m_pExample == nullptr && m_CurrentType > 0)
    {
        auto it = m_TypeToExample.find(m_CurrentType);
        if (it != m_TypeToExample.end())
        {
            m_pExample = it->second;
        } 
        else
        {
            switch (m_CurrentType)
            {
            case SINGLE_RENDER_TYPE:
                m_pExample = new CSinglePlayerRenderer();
                break;
            case SEQ_RENDER_TYPE:
                m_pExample = new CSeqPlayerRenderer();
                break;
            case RAIN_RENDER_TYPE:
                m_pExample = new CRainSceneRenderer();
                break;
            case SNOW_RENDER_TYPE:
                m_pExample = new CSnowSceneRenderer();
                break;
            case CLOUD_RENDER_TYPE:
                m_pExample = new CCloudSceneRenderer();
                break;
            default:
                m_pExample = new CSinglePlayerRenderer();
                break;
            }
            if (m_pExample && m_pExample->init())
            {
                m_TypeToExample[m_CurrentType] = m_pExample; // 缓存
            } else
            {
                LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Init example failed on surface created.");
                __deleteSafely(m_pExample);
            }
        }
    }

    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Render example created and initialized.");
}

void CNativeRenderer::HandleOnSurfaceChanged(uint64_t vWidth, uint64_t vHeight)
{
    if (m_EglDisplay != EGL_NO_DISPLAY)
    {
        int renderWidth = vWidth;
        int renderHeight = vHeight / 2;
        int offsetX = 0;
        int offsetY = (vHeight - renderHeight) / 4 * 3;
        
        glViewport(offsetX, offsetY, renderWidth, renderHeight);
        glClearColor(0.0, 1.0, 1.0, 1.0);
        LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Viewport updated to %{public}lu x %{public}lu, render area: %{public}d x %{public}d at (%{public}d, %{public}d)", 
             vWidth, vHeight, renderWidth, renderHeight, offsetX, offsetY);
    }
}

void CNativeRenderer::HandleOnSurfaceDestroyed()
{
    for (auto &kv : m_TypeToExample)
    {
        if (kv.second)
        {
            delete kv.second;
            kv.second = nullptr;
        }
    }
    m_TypeToExample.clear();
    m_pExample = nullptr;
    m_ContextReady = false;
    EglTermination();
}

bool CNativeRenderer::EglInitialization(void *vWindow)
{
    m_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_EglDisplay == EGL_NO_DISPLAY)
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Failed to get EGL display.");
        return false;
    }

    EGLint Major, Minor;
    if (!eglInitialize(m_EglDisplay, &Major, &Minor))
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Failed to initialize EGL.");
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
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Failed to choose EGL config.");
        return false;
    }

    m_EglSurface = eglCreateWindowSurface(m_EglDisplay, m_EglConfig, reinterpret_cast<EGLNativeWindowType>(vWindow), nullptr);
    if (m_EglSurface == EGL_NO_SURFACE)
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Failed to create EGL window surface. EGL error: %{public}d", eglGetError());
        return false;
    }

    const EGLint ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    m_EglContext = eglCreateContext(m_EglDisplay, m_EglConfig, EGL_NO_CONTEXT, ContextAttribs);
    if (m_EglContext == EGL_NO_CONTEXT)
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Failed to create EGL context. EGL error: %{public}d", eglGetError());
        return false;
    }

    if (!eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext))
    {
        LOGE(TAG_KEYWORD::NATIVE_RENDERER_TAG, "Failed to make EGL context current. EGL error: %{public}d", eglGetError());
        return false;
    }

    m_ContextReady = true;
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "EGL Initialized successfully. Version: %{public}d.%{public}d", Major, Minor);
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
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "EGL terminated.");
}