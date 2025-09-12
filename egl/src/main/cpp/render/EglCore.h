#pragma once

#include "OpenGLCommon.h"
#include "example/BaseExample.h"

class EglCore {
private:
    int width;
    int height;
    EGLNativeWindowType eglWindow;
    EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    EGLContext eglContext = EGL_NO_CONTEXT;
    EGLConfig  eglConfig  = EGL_NO_CONFIG_KHR;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    GLuint program;
    void prepareDraw();
    void release();
    bool finishDraw();
    
public:
    ~EglCore();
    BaseExample* example;
    bool EglContextInit(void *window, int width, int height);
    void setParams(int params);
    void present();
};
