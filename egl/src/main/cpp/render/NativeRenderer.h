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
        static napi_value Init(napi_env env, napi_value exports);
        static napi_value SetRenderType(napi_env env, napi_callback_info info);
        static napi_value TriggerLightning(napi_env env, napi_callback_info info);
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
        void   renderScene();
    
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
    
        OH_NativeXComponent* m_NativeXComponent = nullptr;
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
}


