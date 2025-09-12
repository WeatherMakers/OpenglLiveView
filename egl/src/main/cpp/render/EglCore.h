#pragma once

#include "OpenGLCommon.h"
#include "example/BaseExample.h"

namespace hiveVG
{
    class EglCore
    {
    public:
        ~EglCore();
        BaseExample *m_pExample;
        bool initEglContext(void *window, int width, int height);
        void setParams(int vParams);
        void present();
    
    private:
        int Width;
        int Height;
        EGLNativeWindowType EglWindow;
        EGLDisplay EglDisplay = EGL_NO_DISPLAY;
        EGLContext EglContext = EGL_NO_CONTEXT;
        EGLConfig EglConfig = EGL_NO_CONFIG_KHR;
        EGLSurface EglSurface = EGL_NO_SURFACE;
        GLuint Program;
        void prepareDraw();
        void release();
        bool finishDraw();
    };
}
