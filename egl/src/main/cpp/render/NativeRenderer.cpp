#include "NativeRenderer.h"
#include "AppContext.h"
#include "log.h"
#include "example/BaseRenderer.h"
#include "example/SinglePlayerRenderer.h"
#include "example/RainSceneRenderer.h"
#include "example/SnowSceneRenderer.h"
#include "example/CloudSceneRenderer.h"
#include "example/FullRainSceneRenderer.h"
#include "example/FullSnowSceneRenderer.h"
#include "example/FullSceneRenderer.h"

using namespace hiveVG;
using hiveVG::detail::__setChannelGeneric;

CNativeRenderer *CNativeRenderer::getInstance()
{
    static CNativeRenderer Instance;
    return &Instance;
}

CBaseRenderer* CNativeRenderer::getCurrentExample()
{
    return getInstance()->m_pExample;
}

CNativeRenderer::CNativeRenderer()
{
    m_pNativeXComponent = nullptr;
    m_pExample = nullptr;
}

CNativeRenderer::~CNativeRenderer() { __deleteSafely(m_pExample); }

napi_value CNativeRenderer::TriggerCloud(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerCloud called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullRainSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullRainSceneRenderer*>(Renderer->m_pExample)->toggleCloud();
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerLightRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerLightRain called.");
    __setChannelGeneric<CFullRainSceneRenderer>(&CFullRainSceneRenderer::setChannel, ERenderChannel::R);
    return nullptr;
}

napi_value CNativeRenderer::TriggerModerateRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerModerateRain called.");
    __setChannelGeneric<CFullRainSceneRenderer>(&CFullRainSceneRenderer::setChannel, ERenderChannel::G);
    return nullptr;
}

napi_value CNativeRenderer::TriggerHeavyRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerHeavyRain called.");
    __setChannelGeneric<CFullRainSceneRenderer>(&CFullRainSceneRenderer::setChannel, ERenderChannel::B);
    return nullptr;
}

napi_value CNativeRenderer::TriggerStormRain(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerStormRain called.");
    __setChannelGeneric<CFullRainSceneRenderer>(&CFullRainSceneRenderer::setChannel, ERenderChannel::A);
    return nullptr;
}


// 雪景相关 NAPI 函数
napi_value CNativeRenderer::TriggerSnowBackground(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerSnowBackground called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSnowSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSnowSceneRenderer*>(Renderer->m_pExample)->toggleSnowBackground();
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerSnowForeground(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerSnowForeground called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSnowSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSnowSceneRenderer*>(Renderer->m_pExample)->toggleSnowForeground();
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerLightSnow(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerLightSnow called - setting R channel (13fps).");
    __setChannelGeneric<CFullSnowSceneRenderer>(&CFullSnowSceneRenderer::setChannel, ERenderChannel::R);
    return nullptr;
}

napi_value CNativeRenderer::TriggerModerateSnow(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerModerateSnow called - setting G channel (18fps).");
    __setChannelGeneric<CFullSnowSceneRenderer>(&CFullSnowSceneRenderer::setChannel, ERenderChannel::G);
    return nullptr;
}

napi_value CNativeRenderer::TriggerHeavySnow(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerHeavySnow called - setting B channel (23fps).");
    __setChannelGeneric<CFullSnowSceneRenderer>(&CFullSnowSceneRenderer::setChannel, ERenderChannel::B);
    return nullptr;
}

napi_value CNativeRenderer::TriggerStormSnow(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerStormSnow called - setting A channel (28fps).");
    __setChannelGeneric<CFullSnowSceneRenderer>(&CFullSnowSceneRenderer::setChannel, ERenderChannel::A);
    return nullptr;
}

// === 全场景相关 NAPI 函数 ===
napi_value CNativeRenderer::TriggerFullSceneRainLight(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneRainLight called - setting R channel (13fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setRainChannel, ERenderChannel::R);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneRainModerate(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneRainModerate called - setting G channel (18fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setRainChannel, ERenderChannel::G);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneRainHeavy(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneRainHeavy called - setting B channel (10fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setRainChannel, ERenderChannel::B);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneRainStorm(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneRainStorm called - setting A channel (20fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setRainChannel, ERenderChannel::A);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneRainCloud(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneRainCloud called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->toggleCloud();
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneSnowLight(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneSnowLight called - setting R channel (13fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setSnowChannel, ERenderChannel::R);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneSnowModerate(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneSnowModerate called - setting G channel (18fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setSnowChannel, ERenderChannel::G);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneSnowHeavy(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneSnowHeavy called - setting B channel (23fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setSnowChannel, ERenderChannel::B);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneSnowStorm(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneSnowStorm called - setting A channel (28fps).");
    __setChannelGeneric<CFullSceneRenderer>(&CFullSceneRenderer::setSnowChannel, ERenderChannel::A);
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneSnowBackground(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneSnowBackground called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->toggleSnowBackground();
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerFullSceneSnowForeground(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerFullSceneSnowForeground called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->toggleSnowForeground();
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerColorSetting(napi_env env, napi_callback_info info){
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    double colorValue = 0;
    napi_get_value_double(env, argv[0], &colorValue);
    colorValue /= 100.0;
    
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerColorSetting called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->setColor(float(colorValue));
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerCloudThicknessSetting(napi_env env, napi_callback_info info){
    size_t argc = 1;
    napi_value argv[1];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    double colorValue = 0;
    napi_get_value_double(env, argv[0], &colorValue);
    colorValue /= 100.0;
    
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerCloudThicknessSetting called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->setCloudThickness(float(colorValue));
    }
    return nullptr;
}

napi_value CNativeRenderer::TriggerBackgroundSetting(napi_env env, napi_callback_info info){
    size_t argc = 3;
    napi_value argv[3];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    double  colorR = 0, colorG = 0, colorB = 0;
    napi_get_value_double(env, argv[0], &colorR);
    napi_get_value_double(env, argv[1], &colorG);
    napi_get_value_double(env, argv[2], &colorB);
    
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerBackgroundSetting called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->setBackgroundColor(colorR, colorG, colorB);
    }
    return nullptr;
}


napi_value CNativeRenderer::TriggerColorSelfAdjustment(napi_env env, napi_callback_info info){
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "TriggerColorSelfAdjustment called.");
    auto Renderer = getInstance();
    napi_value ReturnValue = nullptr;
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        auto FullSceneRenderer = static_cast<CFullSceneRenderer*>(Renderer->m_pExample);
        FullSceneRenderer->updateBackgroundLumin();
        float Value = FullSceneRenderer->adjustRainColor();
        napi_create_double(env, Value, &ReturnValue);
    }
    return ReturnValue;
}

napi_value CNativeRenderer::OnStartupColorSelfAdjustment(napi_env env, napi_callback_info info){
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "OnStartupColorSelfAdjustment called.");
    auto Renderer = getInstance();
    if(Renderer->m_pExample == nullptr){
        Renderer->m_pExample = new CFullSceneRenderer();
        Renderer->m_pExample->init();
    }
    auto Result = TriggerColorSelfAdjustment(env, info);
    delete Renderer->m_pExample;
    Renderer->m_pExample = nullptr;
    return Result;
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
        case RAIN_RENDER_TYPE:
            pExample = new CRainSceneRenderer();
            break;
        case CLOUD_RENDER_TYPE:
            pExample = new CCloudSceneRenderer();
            break;
        case SNOW_RENDER_TYPE:
            pExample = new CSnowSceneRenderer();
            break;
        case FULL_SCENE_RENDER_TYPE:
            pExample = new CFullRainSceneRenderer();
            break;
        case FULL_SNOW_SCENE_RENDER_TYPE:
            pExample = new CFullSnowSceneRenderer();
            break;
        case FULL_ALL_SCENE_RENDER_TYPE:
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

napi_value CNativeRenderer::ChangeFPS(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "ChangeFPS called.");
    auto Renderer = getInstance();
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        size_t argc = 1;
        napi_value argv[1];
        napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
        int32_t fps = 0;
        napi_get_value_int32(env, argv[0], &fps);

        static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->setFPS(fps);
    }
    return nullptr;
}

napi_value CNativeRenderer::GetFPS(napi_env env, napi_callback_info info)
{
    LOGI(TAG_KEYWORD::NATIVE_RENDERER_TAG, "GetFPS called.");
    auto Renderer = getInstance();
    int fps = 0;
    if (Renderer->m_pExample && dynamic_cast<CFullSceneRenderer*>(Renderer->m_pExample))
    {
        fps = static_cast<CFullSceneRenderer*>(Renderer->m_pExample)->getFPS();
    }
    napi_value result;
    napi_create_int32(env, fps, &result);
    return result;
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
        {"triggerCloud", nullptr, TriggerCloud, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerLightRain", nullptr, TriggerLightRain, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerModerateRain", nullptr, TriggerModerateRain, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerHeavyRain", nullptr, TriggerHeavyRain, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerStormRain", nullptr, TriggerStormRain, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerSnowBackground", nullptr, TriggerSnowBackground, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerSnowForeground", nullptr, TriggerSnowForeground, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerLightSnow", nullptr, TriggerLightSnow, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerModerateSnow", nullptr, TriggerModerateSnow, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerHeavySnow", nullptr, TriggerHeavySnow, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerStormSnow", nullptr, TriggerStormSnow, nullptr, nullptr, nullptr, napi_default, nullptr},
        
        // 全场景相关 NAPI 函数
        {"triggerFullSceneRainLight", nullptr, TriggerFullSceneRainLight, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneRainModerate", nullptr, TriggerFullSceneRainModerate, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneRainHeavy", nullptr, TriggerFullSceneRainHeavy, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneRainStorm", nullptr, TriggerFullSceneRainStorm, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneRainCloud", nullptr, TriggerFullSceneRainCloud, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneSnowLight", nullptr, TriggerFullSceneSnowLight, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneSnowModerate", nullptr, TriggerFullSceneSnowModerate, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneSnowHeavy", nullptr, TriggerFullSceneSnowHeavy, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneSnowStorm", nullptr, TriggerFullSceneSnowStorm, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneSnowBackground", nullptr, TriggerFullSceneSnowBackground, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneSnowForeground", nullptr, TriggerFullSceneSnowForeground, nullptr, nullptr, nullptr, napi_default, nullptr},
    
        // 背景自适应相关 NAPI 函数
        {"triggerColorSetting", nullptr, TriggerColorSetting, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerCloudThicknessSetting", nullptr, TriggerCloudThicknessSetting, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerBackgroundSetting", nullptr, TriggerBackgroundSetting, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerColorSelfAdjustment", nullptr, TriggerColorSelfAdjustment, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onStartupColorSelfAdjustment", nullptr, OnStartupColorSelfAdjustment, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"triggerFullSceneSnowForeground", nullptr, TriggerFullSceneSnowForeground, nullptr, nullptr, nullptr, napi_default, nullptr},
        
        // FPS调节
        {"changeFPS", nullptr, ChangeFPS, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getFPS", nullptr, GetFPS, nullptr, nullptr, nullptr, napi_default, nullptr},
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
    Renderer->m_pNativeXComponent = pNativeXComponent;
    Renderer->m_Callback.OnSurfaceCreated = CNativeRenderer::OnSurfaceCreated;
    Renderer->m_Callback.OnSurfaceChanged = CNativeRenderer::OnSurfaceChanged;
    Renderer->m_Callback.OnSurfaceDestroyed = CNativeRenderer::OnSurfaceDestroyed;
    OH_NativeXComponent_RegisterCallback(Renderer->m_pNativeXComponent, &Renderer->m_Callback);
    OH_NativeXComponent_RegisterOnFrameCallback(Renderer->m_pNativeXComponent, OnFrame);
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