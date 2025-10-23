#pragma once

#include "napi/native_api.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "OpenGLCommon.h"
#include "example/BaseRenderer.h"
#include <unordered_map>

namespace hiveVG
{
    class CNativeRenderer 
    {
    public:
        static CNativeRenderer* getInstance();
        void   renderScene();
        static CBaseRenderer* getCurrentExample();
    
        static napi_value Init(napi_env env, napi_value exports);
        static napi_value SetRenderType(napi_env env, napi_callback_info info);
    
        //  全雨景 && 全雪景相关 NAPI 函数
        static napi_value TriggerCloud(napi_env env, napi_callback_info info);
        static napi_value TriggerLightRain(napi_env env, napi_callback_info info);
        static napi_value TriggerModerateRain(napi_env env, napi_callback_info info);
        static napi_value TriggerHeavyRain(napi_env env, napi_callback_info info);
        static napi_value TriggerStormRain(napi_env env, napi_callback_info info);
        static napi_value TriggerSnowBackground(napi_env env, napi_callback_info info);
        static napi_value TriggerSnowForeground(napi_env env, napi_callback_info info);
        static napi_value TriggerLightSnow(napi_env env, napi_callback_info info);
        static napi_value TriggerModerateSnow(napi_env env, napi_callback_info info);
        static napi_value TriggerHeavySnow(napi_env env, napi_callback_info info);
        static napi_value TriggerStormSnow(napi_env env, napi_callback_info info);
        
        // 全场景相关 NAPI 函数
        static napi_value TriggerFullSceneRainLight(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneRainModerate(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneRainHeavy(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneRainStorm(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneRainCloud(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneSnowLight(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneSnowModerate(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneSnowHeavy(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneSnowStorm(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneSnowBackground(napi_env env, napi_callback_info info);
        static napi_value TriggerFullSceneSnowForeground(napi_env env, napi_callback_info info);
        static napi_value ChangeFPS(napi_env env, napi_callback_info info);
        static napi_value GetFPS(napi_env env, napi_callback_info info);
        void   renderScene();
    
        // 背景自适应相关NAPI函数
        static napi_value TriggerColorSetting(napi_env env, napi_callback_info info);
        static napi_value TriggerCloudThicknessSetting(napi_env env, napi_callback_info info);
        static napi_value TriggerBackgroundSetting(napi_env env, napi_callback_info info);
        static napi_value TriggerColorSelfAdjustment(napi_env env, napi_callback_info info);
        static napi_value OnStartupColorSelfAdjustment(napi_env env, napi_callback_info info);
    private:
        CNativeRenderer();
        ~CNativeRenderer();
        CNativeRenderer(const CNativeRenderer&) = delete;
        CNativeRenderer& operator=(const CNativeRenderer&) = delete;
    
        static void OnSurfaceCreated(OH_NativeXComponent*   vComponent, void* vWindow);
        static void OnSurfaceChanged(OH_NativeXComponent*   vComponent, void* vWindow);
        static void OnSurfaceDestroyed(OH_NativeXComponent* vComponent, void* vWindow);
        void HandleOnSurfaceCreated(void* vWindow);
        void HandleOnSurfaceChanged(uint64_t vWidth, uint64_t vHeight);
        void HandleOnSurfaceDestroyed();
    
        bool EglInitialization(void* vWindow);
        void EglTermination();
    
        template<typename T>
        void __deleteSafely(T*& vPointer);
    
        OH_NativeXComponent* m_pNativeXComponent = nullptr;
        OH_NativeXComponent_Callback m_Callback;
 
        EGLDisplay m_EglDisplay = EGL_NO_DISPLAY;
        EGLContext m_EglContext = EGL_NO_CONTEXT;
        EGLConfig  m_EglConfig  = nullptr;
        EGLSurface m_EglSurface = EGL_NO_SURFACE;
        bool m_ContextReady = false;
        int  m_CurrentType = 0;
        CBaseRenderer* m_pExample = nullptr;
        std::unordered_map<int, CBaseRenderer*> m_TypeToExample;
    };

    template<typename T>
    void CNativeRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }

    // 内部工具：模板放在头文件，置于 detail 命名空间避免污染公共API
    namespace detail
    {
        template <typename RendererT, typename MemberFn>
        inline void __setChannelGeneric(MemberFn fn, hiveVG::ERenderChannel vChannel)
        {
            auto Example = hiveVG::CNativeRenderer::getCurrentExample();
            if (Example && dynamic_cast<RendererT*>(Example))
            {
                (static_cast<RendererT*>(Example)->*fn)(vChannel);
            }
        }
    }
}


