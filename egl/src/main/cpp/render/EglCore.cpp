#include "EglCore.h"
#include "example/EBORectangleExample.h"
#include "example/TriangleExample.h"
#include "example/VAOTriangleExample.h"
#include "example/ImageExample.h"
#include "example/VBOTriangleExample.h"
#include "log.h"

using namespace hiveVG;

EglCore::~EglCore()
{
    LOGD("执行EglCore析构函数");
    release();
}

void EglCore::release()
{
    LOGD("执行EglCore释放函数");

    if (!eglDestroySurface(EglDisplay, EglSurface))
    {
        LOGE("销毁eglSurface失败");
    }
    if (!eglDestroyContext(EglDisplay, EglContext))
    {
        LOGE("销毁eglContext失败");
    }
    if (!eglTerminate(EglDisplay))
    {
        LOGE("销毁eglDisplay失败");
    }
    if (m_pExample)
    {
        glDeleteProgram(m_pExample->program);
        delete m_pExample;
        m_pExample = nullptr;
    }
}

bool EglCore::initEglContext(void *window, int width, int height)
{
    this->Width = width;
    this->Height = height;
    // 获取EGLDisplay对象：调用eglGetDisplay函数得到EGLDisplay，并加载OpenGL ES库。
    EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EglDisplay == EGL_NO_DISPLAY)
    {
        LOGE("eGLDisplay获取失败");
        return false;
    }
    EGLint major;
    EGLint minor;
    // 初始化EGL连接：调用eglInitialize函数初始化，获取库的版本号。
    if (!eglInitialize(EglDisplay, &major, &minor))
    {
        LOGE("eGLDisplay初始化失败");
        return false;
    }
    const EGLint maxConfigSize = 1;
    EGLint numConfigs;
    // 确定渲染表面的配置信息：调用eglChooseConfig函数得到EGLConfig。
    if (!eglChooseConfig(EglDisplay, ATTRIB_LIST, &EglConfig, maxConfigSize, &numConfigs))
    {
        LOGE("eglConfig初始化失败");
        return false;
    }
    EglWindow = reinterpret_cast<EGLNativeWindowType>(window);
    // 创建渲染表面：通过EGLDisplay和EGLConfig，调用eglCreateWindowSurface函数创建渲染表面，得到EGLSurface。
    EglSurface = eglCreateWindowSurface(EglDisplay, EglConfig, EglWindow, nullptr);
    if (nullptr == EglSurface)
    {
        LOGE("创建eGLSurface失败");
        return false;
    }
    // 创建渲染上下文：通过EGLDisplay和EGLConfig，调用eglCreateContext函数创建渲染上下文，得到EGLContext。
    EglContext = eglCreateContext(EglDisplay, EglConfig, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    if (nullptr == EglContext)
    {
        LOGE("创建eglContext失败");
        return false;
    }
    // 绑定上下文：通过eglMakeCurrent函数将EGLSurface、EGLContext、EGLDisplay三者绑定，接下来就可以使用OpenGL进行绘制了。
    if (!eglMakeCurrent(EglDisplay, EglSurface, EglSurface, EglContext))
    {
        LOGE("eglMakeCurrent失败");
        return false;
    }
    prepareDraw();
    return true;
}

void EglCore::prepareDraw()
{
    glViewport(DEFAULT_X_POSITION, DEFAULT_X_POSITION, Width, Height);
    glClearColor(GL_RED_DEFAULT, GL_GREEN_DEFAULT, GL_BLUE_DEFAULT, GL_ALPHA_DEFAULT);
    glClear(GL_COLOR_BUFFER_BIT);
}

bool EglCore::finishDraw()
{
    glFlush();
    glFinish();
    return eglSwapBuffers(EglDisplay, EglSurface);
}

void EglCore::present()
{
    finishDraw();
}

void EglCore::setParams(int vParams)
{
    if (m_pExample)
    {
        delete m_pExample;
        m_pExample = nullptr;
    }
    switch (vParams)
    {
    case TRIANGLE_TYPE:
        m_pExample = new TriangleExample();
        break;
    case VBO_TRIANGLE_TYPE:
        m_pExample = new VBOTriangleExample();
        break;
    case EBO_TRIANGLE_TYPE:
        m_pExample = new EBORectangleExample();
        break;
    case VAO_TRIANGLE_TYPE:
        m_pExample = new VAOTriangleExample();
        break;
    case IMAGE_TYPE:
        m_pExample = new CImageExample();
        break;
    case IMAGE_FROM_NATIVE_TYPE:
        m_pExample = new CImageExample();
        break;
    default:
        m_pExample = new TriangleExample();
        break;
    }
    if (m_pExample->init())
    {
        m_pExample->draw();
        finishDraw();
    }
}