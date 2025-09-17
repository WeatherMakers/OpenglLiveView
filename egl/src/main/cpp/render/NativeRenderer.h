#pragma once

#include "napi/native_api.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "OpenGLCommon.h"
#include "example/BaseExample.h"

namespace hiveVG
{
    class CNativeRenderer 
    {
    public:
        static CNativeRenderer* getInstance();
        static napi_value Init(napi_env env, napi_value exports);
        void   renderScene();
    
    private:
        CNativeRenderer();
        ~CNativeRenderer();
        CNativeRenderer(const CNativeRenderer&) = delete;
        CNativeRenderer& operator=(const CNativeRenderer&) = delete;
    
        // NAPI 方法，由 ArkTS 调用，用于传入 XComponent 实例
        static napi_value OnCreate(napi_env env, napi_callback_info info);
        static void OnSurfaceCreated(OH_NativeXComponent*   vComponent, void* vWindow);
        static void OnSurfaceChanged(OH_NativeXComponent*   vComponent, void* vWindow);
        static void OnSurfaceDestroyed(OH_NativeXComponent* vComponent, void* vWindow);
    
        // 成员函数版本的回调处理器
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

        BaseExample* m_pExample = nullptr;
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


