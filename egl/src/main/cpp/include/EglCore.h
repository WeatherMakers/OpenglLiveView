//
// Created on 2024/8/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_EGLCORE_H
#define OPENGL_EGLCORE_H
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include "example/BaseExample.h"
class EglCore {

private:
    int width;
    int height;
    EGLNativeWindowType eglWindow;
    EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    EGLContext eglContext = EGL_NO_CONTEXT;
    EGLConfig eglConfig = EGL_NO_CONFIG_KHR;
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

#endif //OPENGL_EGLCORE_H
