#include "FullSnowSceneRenderer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "RainWithBackgroundSeqPlayer.h"
#include "ThickCloudSequencePlayer.h"
#include "log.h"

using namespace hiveVG;

CFullSnowSceneRenderer::CFullSnowSceneRenderer()
{
}

CFullSnowSceneRenderer::~CFullSnowSceneRenderer()
{
    __deleteSafely(m_pSnowBackgroundPlayer);
    __deleteSafely(m_pSnowForegroundPlayer);
    __deleteSafely(m_pConfigReader);
}

bool CFullSnowSceneRenderer::init()
{
    if (m_pConfigReader == nullptr)
        m_pConfigReader = new CJsonReader(m_ConfigFile);

    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }

    m_RenderChannel = ERenderChannel::R;
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "FullSnowSceneRenderer initialized successfully (lazy loading mode)");
    
    toggleSnowBackground();
    toggleSnowForeground();
    
    return true;
}

void CFullSnowSceneRenderer::draw()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f, 0.345f, 0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    int CurrentFps = m_snowFpsLight;
    switch (m_RenderChannel)
    {
        case ERenderChannel::R: CurrentFps = m_snowFpsLight; break;
        case ERenderChannel::G: CurrentFps = m_snowFpsModerate; break;
        case ERenderChannel::B: CurrentFps = m_snowFpsHeavy; break;
        case ERenderChannel::A: CurrentFps = m_snowFpsStorm; break;
    }

    if (m_SnowBackgroundInitialized && m_SnowBackgroundVisible && m_pSnowBackgroundPlayer)
    {
        m_pSnowBackgroundPlayer->setFrameRate(CurrentFps);
        m_pSnowBackgroundPlayer->updateMultiChannelFrame(DeltaTime, m_RenderChannel);
        m_pSnowBackgroundPlayer->drawMultiChannelFrame(m_pScreenQuad);
    }

    if (m_SnowForegroundInitialized && m_SnowForegroundVisible && m_pSnowForegroundPlayer)
    {
        m_pSnowForegroundPlayer->setFrameRate(CurrentFps);
        m_pSnowForegroundPlayer->updateMultiChannelFrame(DeltaTime, m_RenderChannel);
        m_pSnowForegroundPlayer->drawMultiChannelFrame(m_pScreenQuad);
    }
}

void CFullSnowSceneRenderer::setChannel(ERenderChannel vChannel)
{
    m_RenderChannel = vChannel;
    if (!m_SnowBackgroundInitialized) { __initSnowBackgroundPlayer(); }
    if (!m_SnowForegroundInitialized) { __initSnowForegroundPlayer(); }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Setting render channel to %d", static_cast<int>(vChannel));
}

void CFullSnowSceneRenderer::toggleSnowBackground()
{
    if (!m_SnowBackgroundInitialized)
    {
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Initializing Snow Background player (toggle)...");
        __initSnowBackgroundPlayer();
        m_SnowBackgroundVisible = true;
    }
    else
    {
        m_SnowBackgroundVisible = !m_SnowBackgroundVisible;
    }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Background visibility toggled to %d", m_SnowBackgroundVisible);
}

void CFullSnowSceneRenderer::toggleSnowForeground()
{
    if (!m_SnowForegroundInitialized)
    {
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Initializing Snow Foreground player (toggle)...");
        __initSnowForegroundPlayer();
        m_SnowForegroundVisible = true;
    }
    else
    {
        m_SnowForegroundVisible = !m_SnowForegroundVisible;
    }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Foreground visibility toggled to %d", m_SnowForegroundVisible);
}

void CFullSnowSceneRenderer::__initSnowBackgroundPlayer()
{
    if (m_pSnowBackgroundPlayer) return;
    Json::Value Config = m_pConfigReader->getObject("SnowBackground");
    std::string FramesPath = Config["frames_path"].asString();
    std::string FramesType = Config["frames_type"].asString();
    int FramesCount = Config["frames_count"].asInt();
    int OneTextureFrames = Config["one_texture_frames"].asInt();
    float Fps = Config["fps"].asFloat();
    std::string VertexShader = Config["vertex_shader"].asString();
    std::string FragShader = Config["fragment_shader"].asString();
    EPictureType::EPictureType PicType = EPictureType::FromString(FramesType);
    m_pSnowBackgroundPlayer = new CSequenceFramePlayer(FramesPath, FramesCount, OneTextureFrames, Fps, PicType);
    m_pSnowBackgroundPlayer->initTextureAndShaderProgram(VertexShader, FragShader);
    m_pSnowBackgroundPlayer->setWindowSize(m_WindowSize);
    m_SnowBackgroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Background Player initialized.");
}

void CFullSnowSceneRenderer::__initSnowForegroundPlayer()
{
    if (m_pSnowForegroundPlayer) return;
    Json::Value Config = m_pConfigReader->getObject("SnowForeground");
    std::string FramesPath = Config["frames_path"].asString();
    std::string FramesType = Config["frames_type"].asString();
    int FramesCount = Config["frames_count"].asInt();
    int OneTextureFrames = Config["one_texture_frames"].asInt();
    float Fps = Config["fps"].asFloat();
    std::string VertexShader = Config["vertex_shader"].asString();
    std::string FragShader = Config["fragment_shader"].asString();
    EPictureType::EPictureType PicType = EPictureType::FromString(FramesType);
    m_pSnowForegroundPlayer = new CSequenceFramePlayer(FramesPath, FramesCount, OneTextureFrames, Fps, PicType);
    m_pSnowForegroundPlayer->initTextureAndShaderProgram(VertexShader, FragShader);
    m_pSnowForegroundPlayer->setWindowSize(m_WindowSize);
    m_SnowForegroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Foreground Player initialized.");
}