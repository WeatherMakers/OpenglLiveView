//
// Created on 2024/8/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "EglCore.h"
#include "example/EBORectangleExample.h"
#include "example/TriangleExample.h"
#include "example/VBOTriangleExample.h"
#include "example/VAOTriangleExample.h"
#include "log.h"

EglCore::~EglCore() {
    LOGD("执行EglCore析构函数");
    release();
}

void EglCore::release() {
    LOGD("执行EglCore释放函数");
    if (!eglDestroySurface(eglDisplay, eglSurface)) {
        LOGE("销毁eglSurface失败");
    }
    if (!eglDestroyContext(eglDisplay, eglContext)) {
        LOGE("销毁eglContext失败");
    }
    if (!eglTerminate(eglDisplay)) {
        LOGE("销毁eglDisplay失败");
    }
    if (example) {
        glDeleteProgram(example->program);
        delete example;
        example = nullptr;
    }
}

bool EglCore::EglContextInit(void *window, int width, int height) {
    this->width = width;
    this->height = height;
    // 获取EGLDisplay对象：调用eglGetDisplay函数得到EGLDisplay，并加载OpenGL ES库。
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDisplay == EGL_NO_DISPLAY) {
        LOGE("eGLDisplay获取失败");
        return false;
    }
    EGLint major;
    EGLint minor;
    // 初始化EGL连接：调用eglInitialize函数初始化，获取库的版本号。
    if (!eglInitialize(eglDisplay, &major, &minor)) {
        LOGE("eGLDisplay初始化失败");
        return false;
    }
    const EGLint maxConfigSize = 1;
    EGLint numConfigs;
    // 确定渲染表面的配置信息：调用eglChooseConfig函数得到EGLConfig。
    if (!eglChooseConfig(eglDisplay, ATTRIB_LIST, &eglConfig, maxConfigSize, &numConfigs)) {
       LOGE("eglConfig初始化失败");
        return false; 
    }
    eglWindow = reinterpret_cast<EGLNativeWindowType>(window);
    // 创建渲染表面：通过EGLDisplay和EGLConfig，调用eglCreateWindowSurface函数创建渲染表面，得到EGLSurface。
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglWindow, nullptr);
    if (nullptr == eglSurface) {
        LOGE("创建eGLSurface失败");
        return false;
    }
    // 创建渲染上下文：通过EGLDisplay和EGLConfig，调用eglCreateContext函数创建渲染上下文，得到EGLContext。
    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    if (nullptr == eglContext) {
        LOGE("创建eglContext失败");
        return false;
    }
    // 绑定上下文：通过eglMakeCurrent函数将EGLSurface、EGLContext、EGLDisplay三者绑定，接下来就可以使用OpenGL进行绘制了。
    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        LOGE("eglMakeCurrent失败");
        return false;
    }
    prepareDraw();
    return true;
}

void EglCore::prepareDraw() {
    // 设置窗口大小
    glViewport(DEFAULT_X_POSITION, DEFAULT_X_POSITION, width, height);
    // 清屏，将屏幕颜色设置为黑色
    glClearColor(GL_RED_DEFAULT, GL_GREEN_DEFAULT, GL_BLUE_DEFAULT, GL_ALPHA_DEFAULT);
    // 清除颜色缓冲
    glClear(GL_COLOR_BUFFER_BIT);
}

bool EglCore::finishDraw() {
    glFlush();
    glFinish();
    // 交换前后缓冲，将绘制内容显示到屏幕上
    return eglSwapBuffers(eglDisplay, eglSurface);
}

void EglCore::setParams(int params) {
    if (example) {
        // 先释放之前的资源
        delete example;
        example = nullptr;
    }
    switch (params) {
        case TRIANGLE_TYPE:
            example = new TriangleExample();
        break;
        case VBO_TRIANGLE_TYPE:
            example = new VBOTriangleExample();
        break;
        case EBO_TRIANGLE_TYPE:
            example = new EBORectangleExample();
        break;
        case VAO_TRIANGLE_TYPE:
            example = new VAOTriangleExample();
        break;
        default:
            example = new TriangleExample();
        break;
    }
    if (example->init()) {
        example->draw();
        finishDraw();
    }
}