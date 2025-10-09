#include "FullRainSceneRenderer.h"
#include "Common.h"
#include "JsonReader.h"
#include "RainWithBackgroundSeqPlayer.h"
#include "ScreenQuad.h"
#include "SequenceFramePlayer.h"
#include "ShaderProgram.h"
#include "SingleTexturePlayer.h"
#include "Texture2D.h"
#include "ThickCloudSequencePlayer.h"
#include "TimeUtils.h"
#include "log.h"

using namespace hiveVG;

CFullRainSceneRenderer::CFullRainSceneRenderer() {}

CFullRainSceneRenderer::~CFullRainSceneRenderer()
{
    __deleteSafely(m_pRainSeqPlayer);
    __deleteSafely(m_pThickCloudPlayer);
    __deleteSafely(m_pCloudPlayer);
    __deleteSafely(m_pConfigReader);
}

bool CFullRainSceneRenderer::init()
{
    if (m_pConfigReader == nullptr)
        m_pConfigReader = new CJsonReader(m_ConfigFile);
    __initRainSeqPlayer();

    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }

    m_RenderChannel = ERenderChannel::R;
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    LOGI(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "FullSceneRenderer initialized (main rain ready)");
    return true;
}

void CFullRainSceneRenderer::draw()
{
    m_CurrentTime = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime = m_CurrentTime;

    glClearColor(0.345f, 0.345f, 0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    m_pRainSeqPlayer->setCurrentChannel(static_cast<std::uint8_t>(m_RenderChannel));
    m_pRainSeqPlayer->updateMultiChannelFrame(DeltaTime, m_RenderChannel);
    m_pRainSeqPlayer->draw(m_pScreenQuad);

    if (m_CloudInitialized && m_CloudVisible && m_pCloudPlayer)
    {
        m_pCloudPlayer->updateCloudLerpMultiChannelFrame(DeltaTime);
        if (m_RenderChannel == ERenderChannel::R || m_RenderChannel == ERenderChannel::G)
        {
            m_pCloudPlayer->drawCloudLerpMultiChannelFrame(m_pScreenQuad);
        }
    }

    if (m_ThickCloudInitialized && m_ThickCloudVisible && m_pThickCloudPlayer)
    {
        m_pThickCloudPlayer->updateQuantizationFrame(DeltaTime);
        if (m_RenderChannel == ERenderChannel::B || m_RenderChannel == ERenderChannel::A)
        {
            m_pThickCloudPlayer->draw(m_pScreenQuad);
        }
    }
}

void CFullRainSceneRenderer::setChannel(ERenderChannel vChannel)
{
    m_RenderChannel = vChannel;

    if (vChannel == ERenderChannel::R)
    {
        if (!m_CloudInitialized)
        {
            __initCloudPlayer();
        }
    }

    if (vChannel == ERenderChannel::B)
    {
        if (!m_ThickCloudInitialized)
        {
            __initThickCloudPlayer();
        }
    }
}

void CFullRainSceneRenderer::toggleCloud()
{
    if (!m_CloudInitialized)
    {
        __initCloudPlayer();
        __initThickCloudPlayer();
        m_CloudVisible = true;
        m_ThickCloudVisible = true;
        return;
    }
    m_CloudVisible = !m_CloudVisible;
    m_ThickCloudVisible = !m_ThickCloudVisible;
}

void CFullRainSceneRenderer::__initRainSeqPlayer()
{
    if (m_pRainSeqPlayer)
        return;
    if (m_pConfigReader == nullptr)
        m_pConfigReader = new CJsonReader(m_ConfigFile);
    Json::Value RainConfig = m_pConfigReader->getObject("Rain");
    Json::Value BackGroundConfig = m_pConfigReader->getObject("Background");
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
    m_pRainSeqPlayer = new CRainWithBackgroundSeqPlayer(RainPath, RainTextureCount, RainOneTextureFrames,
                                                        RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);
    m_pRainSeqPlayer->initBackground(BackImgPath, BackPicType);
}

void CFullRainSceneRenderer::__initCloudPlayer()
{
    if (m_pCloudPlayer)
        return;
    if (m_pConfigReader == nullptr)
        m_pConfigReader = new CJsonReader(m_ConfigFile);
    Json::Value CloudConfig = m_pConfigReader->getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int CloudFrameCount = CloudConfig["frames_count"].asInt();
    int CloudOneTextureFrames = CloudConfig["one_texture_frames"].asInt();
    float CloudPlayFPS = CloudConfig["fps"].asFloat();
    std::string CloudVertexShader = CloudConfig["vertex_shader"].asString();
    std::string CloudFragShader = CloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);
    m_pCloudPlayer =
        new CSequenceFramePlayer(CloudPath, CloudFrameCount, CloudOneTextureFrames, CloudPlayFPS, CloudPicType);
    m_pCloudPlayer->initTextureAndShaderProgram(CloudVertexShader, CloudFragShader);
    m_pCloudPlayer->setWindowSize(m_WindowSize);
    m_pCloudPlayer->setRatioUniform();
    m_CloudInitialized = true;
}

void CFullRainSceneRenderer::__initThickCloudPlayer()
{
    if (m_pThickCloudPlayer) return;
    if (m_pConfigReader == nullptr) m_pConfigReader = new CJsonReader(m_ConfigFile);
    Json::Value ThickCloudConfig = m_pConfigReader->getObject("ThickCloud");
    std::string ThickCloudFramePath = ThickCloudConfig["frames_path"].asString();
    std::string ThickCloudFrameType = ThickCloudConfig["frames_type"].asString();
    int ThickCloudFrameCount = ThickCloudConfig["frames_count"].asInt();
    int ThickCloudOneTextureFrames = ThickCloudConfig["one_texture_frames"].asInt();
    float ThickCloudPlayFPS = ThickCloudConfig["fps"].asFloat();
    std::string ThickCloudVertexShader = ThickCloudConfig["vertex_shader"].asString();
    std::string ThickCloudFragShader = ThickCloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType ThickCloudPicType = EPictureType::FromString(ThickCloudFrameType);
    m_pThickCloudPlayer = new CThickCloudSequencePlayer(ThickCloudFramePath, ThickCloudFrameCount, ThickCloudOneTextureFrames, ThickCloudPlayFPS, ThickCloudPicType);
    
    Json::Value LightningConfig = m_pConfigReader->getObject("LightningWithMask");
    std::string LightningFramePath = LightningConfig["frames_path"].asString();
    std::string LightningFrameType = LightningConfig["frames_type"].asString();
     EPictureType::EPictureType LightningPicType = EPictureType::FromString(LightningFrameType);
    int LightningFrameCount = LightningConfig["frames_count"].asInt();
    int LightningOneTextureFrames = LightningConfig["one_texture_frames"].asInt();
    float LightningPlayFPS = LightningConfig["fps"].asFloat();
    m_pThickCloudPlayer->setLightningAnimationParams(LightningFramePath,LightningFrameCount,LightningOneTextureFrames,LightningPlayFPS,LightningPicType);
    if (m_pThickCloudPlayer->initTextureAndShaderProgram(ThickCloudVertexShader, ThickCloudFragShader))
    {
        m_pThickCloudPlayer->setWindowSize(m_WindowSize);
        m_ThickCloudInitialized = true;
    }
    else
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "ThickCloudPlayer initialization failed.");
    }
}