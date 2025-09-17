#pragma once

#include "OpenGLCommon.h"
#include "example/BaseExample.h"

namespace hiveVG
{
    class EglCore
    {
    public:
        ~EglCore();
        bool initEglContext(void *vWindow, int vWidth, int vHeight);
        void setRenderType(int vParams);
        void renderScene();
    
    private:
        void __updateRenderArea();
    
        template<typename T>
        void __deleteSafely(T*& vPointer);
    
        int m_RenderType = -1;
        int m_WindowWidth;
        int m_WindowHeight;
        EGLNativeWindowType m_EglWindow;
        EGLDisplay m_EglDisplay = EGL_NO_DISPLAY;
        EGLContext m_EglContext = EGL_NO_CONTEXT;
        EGLConfig  m_EglConfig  = EGL_NO_CONFIG_KHR;
        EGLSurface m_EglSurface = EGL_NO_SURFACE;
        EGLContext m_SharedEglContext = EGL_NO_CONTEXT;
        BaseExample *m_pExample = nullptr;
    };

    template<typename T>
    void EglCore::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}
