#include "FullSceneRenderer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SequenceFramePlayer.h"
#include "RainWithBackgroundSeqPlayer.h"
#include "LightningSequencePlayer.h"
#include "log.h"

using namespace hiveVG;

CFullSceneRenderer::CFullSceneRenderer()
{
}

CFullSceneRenderer::~CFullSceneRenderer()
{
    // 雨景播放器
    __deleteSafely(m_pRainSeqPlayer);
    __deleteSafely(m_pCloudPlayer);
    __deleteSafely(m_pLightningPlayer);
    
    // 雪景播放器
    __deleteSafely(m_pBackgroundPlayer);
    __deleteSafely(m_pSnowBackgroundPlayer);
    __deleteSafely(m_pSnowForegroundPlayer);
    
    __deleteSafely(m_pConfigReader);
}

bool CFullSceneRenderer::init()
{
    if (m_pConfigReader == nullptr)
        m_pConfigReader = new CJsonReader(m_ConfigFile);

    // 初始化雨景主序列播放器
    __initRainSeqPlayer();

    // 初始化雪景背景播放器
    __initBackgroundPlayer();

    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }

    m_RainRenderChannel = ERenderChannel::R;
    m_SnowRenderChannel = ERenderChannel::R;
    m_RainLastFrameTime = CTimeUtils::getCurrentTime();
    m_SnowLastFrameTime = CTimeUtils::getCurrentTime();
    
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "FullSceneRenderer initialized successfully (lazy loading mode)");
    return true;
}

void CFullSceneRenderer::draw()
{
    m_RainCurrentTime = CTimeUtils::getCurrentTime();
    m_SnowCurrentTime = CTimeUtils::getCurrentTime();
    double RainDeltaTime = m_RainCurrentTime - m_RainLastFrameTime;
    double SnowDeltaTime = m_SnowCurrentTime - m_SnowLastFrameTime;
    m_RainLastFrameTime = m_RainCurrentTime;
    m_SnowLastFrameTime = m_SnowCurrentTime;

    glClearColor(0.345f, 0.345f, 0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // === 雪景渲染 ===
    // 雪景背景播放器 - 始终渲染（仅在雪景激活时）
    if (m_SnowActive && m_pBackgroundPlayer)
    {
        m_pBackgroundPlayer->updateSeqKTXFrame(SnowDeltaTime);
        m_pBackgroundPlayer->drawSeqKTX(m_pScreenQuad);
    }

    // 根据雪景通道设置不同的fps：R=13, G=18, B=23, A=28
    int SnowCurrentFps = 13; // 默认R通道
    switch (m_SnowRenderChannel)
    {
        case ERenderChannel::R: SnowCurrentFps = 13; break;
        case ERenderChannel::G: SnowCurrentFps = 18; break;
        case ERenderChannel::B: SnowCurrentFps = 23; break;
        case ERenderChannel::A: SnowCurrentFps = 28; break;
    }

    // 雪景背景 - 受可见性控制，按通道同步（仅在雪景激活时）
    if (m_SnowActive && m_SnowBackgroundInitialized && m_SnowBackgroundVisible && m_pSnowBackgroundPlayer)
    {
        m_pSnowBackgroundPlayer->updateMultiChannelFrame(SnowDeltaTime, m_SnowRenderChannel);
        m_pSnowBackgroundPlayer->setFrameRate(SnowCurrentFps);
        m_pSnowBackgroundPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }

    // 雪景前景 - 受可见性控制，按通道同步（仅在雪景激活时）
    if (m_SnowActive && m_SnowForegroundInitialized && m_SnowForegroundVisible && m_pSnowForegroundPlayer)
    {
        m_pSnowForegroundPlayer->updateMultiChannelFrame(SnowDeltaTime, m_SnowRenderChannel);
        m_pSnowForegroundPlayer->setFrameRate(SnowCurrentFps);
        m_pSnowForegroundPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }

    // === 雨景渲染 ===
    // 雨景主序列播放器 - 始终渲染（仅在雨景激活时）
    if (m_RainActive && m_RainSeqInitialized && m_pRainSeqPlayer)
    {
        m_pRainSeqPlayer->updateMultiChannelFrame(RainDeltaTime, m_RainRenderChannel);
        m_pRainSeqPlayer->draw(m_pScreenQuad);
    }



    // 云朵效果 - 在R和G通道显示（可见性开关，仅在雨景激活时）
    if (m_RainActive && m_CloudInitialized && m_CloudVisible && m_pCloudPlayer)
    {
        m_pCloudPlayer->updateLerpQuantFrame(RainDeltaTime);
        if (m_RainRenderChannel == ERenderChannel::R || m_RainRenderChannel == ERenderChannel::G)
        {
            m_pCloudPlayer->drawInterpolationWithFiltering(m_pScreenQuad);
        }
    }

    // 闪电效果 - 在B和A通道显示（可见性开关，仅在雨景激活时）
    if (m_RainActive && m_LightningInitialized && m_LightningVisible && m_pLightningPlayer)
    {
        m_pLightningPlayer->updateQuantizationFrame(RainDeltaTime);
        if (m_RainRenderChannel == ERenderChannel::B || m_RainRenderChannel == ERenderChannel::A)
        {
            m_pLightningPlayer->draw(m_pScreenQuad);
        }
    }
}

void CFullSceneRenderer::setRainChannel(ERenderChannel vChannel)
{
    m_RainRenderChannel = vChannel;
    m_RainActive = true;
    m_SnowActive = false;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Setting rain render channel to %d, Rain active, Snow disabled", static_cast<int>(vChannel));
}

void CFullSceneRenderer::toggleCloud()
{
    if (!m_CloudInitialized)
    {
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Initializing Cloud player (toggle)...");
        __initCloudPlayer();
        m_CloudVisible = true;
    }
    else
    {
        m_CloudVisible = !m_CloudVisible;
    }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Cloud visibility toggled to %d", m_CloudVisible);
}

void CFullSceneRenderer::toggleLightning()
{
    if (!m_LightningInitialized)
    {
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Initializing Lightning player (toggle)...");
        __initLightningPlayer();
        m_LightningVisible = true;
    }
    else
    {
        m_LightningVisible = !m_LightningVisible;
    }
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Lightning visibility toggled to %d", m_LightningVisible);
}

void CFullSceneRenderer::setSnowChannel(ERenderChannel vChannel)
{
    m_SnowRenderChannel = vChannel;
    m_SnowActive = true;
    m_RainActive = false;
    if (!m_BackgroundInitialized) { __initBackgroundPlayer(); }
    if (!m_SnowBackgroundInitialized) { __initSnowBackgroundPlayer(); }
    if (!m_SnowForegroundInitialized) { __initSnowForegroundPlayer(); }
    
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Setting snow render channel to %{public}d, Snow active, Rain disabled", static_cast<int>(vChannel));
}

void CFullSceneRenderer::toggleSnowBackground()
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

void CFullSceneRenderer::toggleSnowForeground()
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

void CFullSceneRenderer::__initRainSeqPlayer()
{
    if (m_pRainSeqPlayer) return;
    Json::Value RainConfig = m_pConfigReader->getObject("Rain");
    Json::Value BackGroundConfig = m_pConfigReader->getObject("RainBackground");
    std::string RainPath = RainConfig["frames_path"].asString();
    std::string RainFrameType = RainConfig["frames_type"].asString();
    int RainTextureCount = RainConfig["frames_count"].asInt();
    int RainOneTextureFrames = RainConfig["one_texture_frames"].asInt();
    float RainFramePerSecond = RainConfig["fps"].asFloat();
    std::string RainVertexShader = RainConfig["vertex_shader"].asString();
    std::string RainFragShader = RainConfig["fragment_shader"].asString();
    EPictureType::EPictureType RainPictureType = EPictureType::FromString(RainFrameType);
    std::string BackImgPath = BackGroundConfig["frames_path"].asString();
    std::string BackFrameType = BackGroundConfig["frames_type"].asString();
    EPictureType::EPictureType BackPicType = EPictureType::FromString(BackFrameType);
    m_pRainSeqPlayer = new CRainWithBackgroundSeqPlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);
    m_pRainSeqPlayer->initBackground(BackImgPath, BackPicType);
    m_RainSeqInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Rain Player initialized.");
}

void CFullSceneRenderer::__initCloudPlayer()
{
    if (m_pCloudPlayer) return;
    Json::Value CloudConfig = m_pConfigReader->getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int CloudFrameCount = CloudConfig["frames_count"].asInt();
    int CloudOneTextureFrames = CloudConfig["one_texture_frames"].asInt();
    float CloudPlayFPS = CloudConfig["fps"].asFloat();
    std::string CloudVertexShader = CloudConfig["vertex_shader"].asString();
    std::string CloudFragShader = CloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);
    m_pCloudPlayer = new CSequenceFramePlayer(CloudPath, CloudFrameCount, CloudOneTextureFrames, CloudPlayFPS, CloudPicType);
    m_pCloudPlayer->initTextureAndShaderProgram(CloudVertexShader, CloudFragShader);
    m_pCloudPlayer->setWindowSize(m_WindowSize);
    m_pCloudPlayer->setRatioUniform();
    m_CloudInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Cloud Player initialized.");
}

void CFullSceneRenderer::__initLightningPlayer()
{
    if (m_pLightningPlayer) return;
    Json::Value LightningConfig = m_pConfigReader->getObject("LightningWithMask");
    std::string LightningFramePath = LightningConfig["frames_path"].asString();
    std::string LightningFrameType = LightningConfig["frames_type"].asString();
    int LightningFrameCount = LightningConfig["frames_count"].asInt();
    int LightningOneTextureFrames = LightningConfig["one_texture_frames"].asInt();
    float LightningPlayFPS = LightningConfig["fps"].asFloat();
    std::string LightningVertexShader = LightningConfig["vertex_shader"].asString();
    std::string LightningFragShader = LightningConfig["fragment_shader"].asString();
    EPictureType::EPictureType LightningPicType = EPictureType::FromString(LightningFrameType);
    m_pLightningPlayer = new CLightningSequencePlayer(LightningFramePath, LightningFrameCount, LightningOneTextureFrames, LightningPlayFPS, LightningPicType);
    if (m_pLightningPlayer->initTextureAndShaderProgram(LightningVertexShader, LightningFragShader))
    {
        m_pLightningPlayer->setWindowSize(m_WindowSize);
        m_LightningInitialized = true;
        LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Lightning Player initialized.");
    }
    else
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "LightningPlayer initialization failed.");
    }
}

void CFullSceneRenderer::__initBackgroundPlayer()
{
    if (m_pBackgroundPlayer) return;
    Json::Value Config = m_pConfigReader->getObject("Background");
    std::string FramesPath = Config["frames_path"].asString();
    std::string FramesType = Config["frames_type"].asString();
    int FramesCount = Config["frames_count"].asInt();
    float Fps = Config["fps"].asFloat();
    std::string VertexShader = Config["vertex_shader"].asString();
    std::string FragShader = Config["fragment_shader"].asString();
    EPictureType::EPictureType PicType = EPictureType::FromString(FramesType);
    int SeqRows = 1, SeqCols = 1;
    m_pBackgroundPlayer = new CSequenceFramePlayer(FramesPath, SeqRows, SeqCols, FramesCount, PicType);
    if (!m_pBackgroundPlayer->initTextureAndShaderProgram(VertexShader, FragShader))
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Failed to init Background player");
    }
    m_pBackgroundPlayer->setFrameRate(Fps);
    m_BackgroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Background Player initialized.");
}

void CFullSceneRenderer::__initSnowBackgroundPlayer()
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
    m_pSnowBackgroundPlayer->setRatioUniform();
    m_SnowBackgroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Background Player initialized.");
}

void CFullSceneRenderer::__initSnowForegroundPlayer()
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
    m_pSnowForegroundPlayer->setRatioUniform();
    m_SnowForegroundInitialized = true;
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Snow Foreground Player initialized.");
}
