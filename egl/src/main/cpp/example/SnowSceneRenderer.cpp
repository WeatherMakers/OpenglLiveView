#include "SnowSceneRenderer.h"
#include "SequenceFramePlayer.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "log.h"

using namespace hiveVG;

CSnowSceneRenderer::CSnowSceneRenderer() {}

CSnowSceneRenderer::~CSnowSceneRenderer()
{
    if (m_pConfigReader) { delete m_pConfigReader; m_pConfigReader = nullptr; }
    if (m_pSnowBackgroundPlayer) { delete m_pSnowBackgroundPlayer; m_pSnowBackgroundPlayer = nullptr; }
    if (m_pBackgroundPlayer)     { delete m_pBackgroundPlayer;     m_pBackgroundPlayer     = nullptr; }
    if (m_pSnowForegroundPlayer) { delete m_pSnowForegroundPlayer; m_pSnowForegroundPlayer = nullptr; }
}

bool CSnowSceneRenderer::init()
{
    if (!m_pConfigReader) m_pConfigReader = new CJsonReader(m_ConfigFile);

    __initBackgroundPlayer();
    __initSnowBackgroundPlayer();
    __initSnowForegroundPlayer();

    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::SNOW_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }

    LOGI(TAG_KEYWORD::SNOW_SCENE_RENDERER_TAG, "SnowSceneRenderer initialized successfully");
    return true;
}

void CSnowSceneRenderer::draw()
{
    glClearColor(0.345f, 0.345f, 0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    if (m_pSnowBackgroundPlayer)
    {
        m_pSnowBackgroundPlayer->updateQuantizationFrame(DeltaTime);
        m_pSnowBackgroundPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }

    if (m_pBackgroundPlayer)
    {
        m_pBackgroundPlayer->updateSeqKTXFrame(DeltaTime);
        m_pBackgroundPlayer->drawSeqKTX(m_pScreenQuad);
    }

    if (m_pSnowForegroundPlayer)
    {
        m_pSnowForegroundPlayer->updateQuantizationFrame(DeltaTime);
        m_pSnowForegroundPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
}

void CSnowSceneRenderer::__initBackgroundPlayer()
{
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
        LOGE(TAG_KEYWORD::SNOW_SCENE_RENDERER_TAG, "Failed to init Background player");
    }
    m_pBackgroundPlayer->setFrameRate(Fps);
}

void CSnowSceneRenderer::__initSnowBackgroundPlayer()
{
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
    if (!m_pSnowBackgroundPlayer->initTextureAndShaderProgram(VertexShader, FragShader))
    {
        LOGE(TAG_KEYWORD::SNOW_SCENE_RENDERER_TAG, "Failed to init SnowBackground player");
    }
    m_pSnowBackgroundPlayer->setFrameRate(Fps);
}

void CSnowSceneRenderer::__initSnowForegroundPlayer()
{
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
    if (!m_pSnowForegroundPlayer->initTextureAndShaderProgram(VertexShader, FragShader))
    {
        LOGE(TAG_KEYWORD::SNOW_SCENE_RENDERER_TAG, "Failed to init SnowForeground player");
    }
    m_pSnowForegroundPlayer->setFrameRate(Fps);
}
