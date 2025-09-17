#include "EglCore.h"
#include "example/ImageExample.h"
#include "example/SeqPlayerExample.h"
#include "example/SinglePlayerExample.h"
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
    LOGI("Init window = %{public}p, w = %{public}d, h = %{public}d.", vWindow, vWidth, vHeight);
    m_WindowWidth = vWidth;
    m_WindowHeight = vHeight;
    m_EglWindow = reinterpret_cast<EGLNativeWindowType>(vWindow);

    constexpr EGLint Attributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    auto Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(Display, nullptr, nullptr);

    EGLint NumConfigs;
    eglChooseConfig(Display, Attributes, nullptr, 0, &NumConfigs);

    std::unique_ptr<EGLConfig[]> pSupportedConfigs(new EGLConfig[NumConfigs]);
    eglChooseConfig(Display, Attributes, pSupportedConfigs.get(), NumConfigs, &NumConfigs);

    auto pConfig = *std::find_if(
            pSupportedConfigs.get(),
            pSupportedConfigs.get() + NumConfigs,
            [&Display](const EGLConfig &Config)
            {
                EGLint Red, Green, Blue, Depth;
                if (eglGetConfigAttrib(Display, Config, EGL_RED_SIZE, &Red)
                    && eglGetConfigAttrib(Display, Config, EGL_GREEN_SIZE, &Green)
                    && eglGetConfigAttrib(Display, Config, EGL_BLUE_SIZE, &Blue)
                    && eglGetConfigAttrib(Display, Config, EGL_DEPTH_SIZE, &Depth))
                {

                    LOGI( "Found pConfig with Red: %{public}d, Green: %{public}d, Blue: %{public}d, Depth: %{public}d", Red, Green, Blue, Depth);
                    return Red == 8 && Green == 8 && Blue == 8 && Depth == 24;
                }
                return false;
            });

    LOGI( "Found %{public}d configs", NumConfigs);

    EGLint Format;
    eglGetConfigAttrib(Display, pConfig, EGL_NATIVE_VISUAL_ID, &Format);
    EGLSurface Surface = eglCreateWindowSurface(Display, pConfig, m_EglWindow, nullptr);

    EGLint ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLContext Context = eglCreateContext(Display, pConfig, nullptr, ContextAttribs);

    auto MadeCurrent = eglMakeCurrent(Display, Surface, Surface, Context);
    assert(MadeCurrent);

    m_EglDisplay = Display;
    m_EglSurface = Surface;
    m_EglContext = Context;

    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);  // 设置清屏颜色
    return true;
}

void EglCore::setRenderType(int vParams)
{
    m_RenderType = vParams;
    if (m_pExample != nullptr)
    {
        delete m_pExample;
        m_pExample = nullptr;
    }
    switch (m_RenderType)
    {
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
        m_pExample = new CImageExample();
        break;
    }
    if (!m_pExample->init())
    {
        LOGE("init Example Error!");
        __deleteSafely(m_pExample);
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
        m_WindowWidth = Width;
        m_WindowHeight = Height;
//        glViewport(0, ViewportY, Width, ViewportHeight);
        glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    } else
    {
        glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    }
}