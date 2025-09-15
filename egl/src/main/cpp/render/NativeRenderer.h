#pragma once

#include "napi/native_api.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "OpenGLCommon.h"
#include "example/SinglePlayerExample.h"

namespace hiveVG
{
    class NativeRenderer 
    {
    public:
        static NativeRenderer* getInstance();
        static napi_value Init(napi_env env, napi_value exports);
    
    private:
        NativeRenderer();
        ~NativeRenderer();
        NativeRenderer(const NativeRenderer&) = delete;
        NativeRenderer& operator=(const NativeRenderer&) = delete;
    
        // NAPI 方法，由 ArkTS 调用，用于传入 XComponent 实例
        static napi_value OnCreate(napi_env env, napi_callback_info info);
        static void OnSurfaceCreated(OH_NativeXComponent* component, void* window);
        static void OnSurfaceChanged(OH_NativeXComponent* component, void* window);
        static void OnSurfaceDestroyed(OH_NativeXComponent* component, void* window);
        static void OnFrame(OH_NativeXComponent* component, void* window);
    
        // 成员函数版本的回调处理器
        void HandleOnSurfaceCreated(void* window);
        void HandleOnSurfaceChanged(uint64_t width, uint64_t height);
        void HandleOnSurfaceDestroyed();
        // 帧回调的处理器，这将是我们的新“渲染循环体”
        void HandleOnFrame();
        bool EglInitialization(void* window);
        void EglTermination();
    
        OH_NativeXComponent* m_nativeXComponent = nullptr;
        OH_NativeXComponent_Callback m_callback;
 
        EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
        EGLContext m_eglContext = EGL_NO_CONTEXT;
        EGLConfig  m_eglConfig  = nullptr;
        EGLSurface m_eglSurface = EGL_NO_SURFACE;

        CSinglePlayerExample* m_pExample = nullptr;
    };
}


