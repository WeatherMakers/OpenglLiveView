#include "EglCore.h"
#include "example/EBORectangleExample.h"
#include "example/TriangleExample.h"
#include "example/VAOTriangleExample.h"
#include "example/ImageExample.h"
#include "example/VBOTriangleExample.h"
#include "example/SinglePlayerExample.h"
#include "example/SeqPlayerExample.h"
#include "log.h"

using namespace hiveVG;

EglCore::~EglCore()
{
    if (m_EglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_EglContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_EglDisplay, m_EglContext);
            m_EglContext = EGL_NO_CONTEXT;
        }
        if (m_EglSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(m_EglDisplay, m_EglSurface);
            m_EglSurface = EGL_NO_SURFACE;
        }
        eglTerminate(m_EglDisplay);
        m_EglDisplay = EGL_NO_DISPLAY;
    }
    __deleteSafely(m_pExample);
}

bool EglCore::initEglContext(void *vWindow, int vWidth, int vHeight)
{
    this->m_WindowWidth = vWidth;
    this->m_WindowHeight = vHeight;
    m_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_EglDisplay == EGL_NO_DISPLAY)
    {
        LOGE("eGLDisplay获取失败");
        return false;
    }
    EGLint Major;
    EGLint Minor;
    if (!eglInitialize(m_EglDisplay, &Major, &Minor))
    {
        LOGE("eGLDisplay初始化失败");
        return false;
    }
    const EGLint MaxConfigSize = 1;
    EGLint NumConfigs;
    if (!eglChooseConfig(m_EglDisplay, ATTRIB_LIST, &m_EglConfig, MaxConfigSize, &NumConfigs))
    {
        LOGE("eglConfig初始化失败");
        return false;
    }
    m_EglWindow = reinterpret_cast<EGLNativeWindowType>(vWindow);
    m_EglSurface = eglCreateWindowSurface(m_EglDisplay, m_EglConfig, m_EglWindow, nullptr);
    if (nullptr == m_EglSurface)
    {
        LOGE("创建eGLSurface失败");
        return false;
    }
    m_EglContext = eglCreateContext(m_EglDisplay, m_EglConfig, EGL_NO_CONTEXT, CONTEXT_ATTRIBS);
    if (nullptr == m_EglContext)
    {
        LOGE("创建eglContext失败");
        return false;
    }
    if (!eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext))
    {
        LOGE("eglMakeCurrent失败");
        return false;
    }
    return true;
}

void EglCore::setRenderType(int vParams)
{
    m_RenderType = vParams;
    if (m_pExample)
    {
        delete m_pExample;
        m_pExample = nullptr;
    }
    switch (m_RenderType)
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
    case SINGLE_PLAYER_TYPE:
        m_pExample = new CSinglePlayerExample();
        break;
    case SEQ_PLAYER_TYPE:
        m_pExample = new CSeqPlayerExample();
        break;
    default:
        m_pExample = new TriangleExample();
        break;
    }
    if (!m_pExample->init())
    {
        LOGE("init Example Error!");
    }
}

void EglCore::renderScene()
{
    __updateRenderArea();
    if (m_pExample != nullptr)
    {
        m_pExample->draw();
    }
    auto SwapResult = eglSwapBuffers(m_EglDisplay, m_EglSurface);
    assert(SwapResult == EGL_TRUE);
}

void EglCore::__updateRenderArea()
{
    EGLint Width, Height;
    eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_WIDTH, &Width);
    eglQuerySurface(m_EglDisplay, m_EglSurface, EGL_HEIGHT, &Height);

//    int ViewportY = Height / 5 * 3;
//    int ViewportHeight = Height / 5;

    if (Width != m_WindowWidth || Height != m_WindowHeight)
    {
        m_WindowWidth  = Width;
        m_WindowHeight = Height;
//        glViewport(0, ViewportY, Width, ViewportHeight);
        glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    }
}