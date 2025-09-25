#include "FullSceneRenderer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "NightSceneSequencePlayer.h"
#include "LightningSequencePlayer.h"
#include "log.h"

using namespace hiveVG;

CFullSceneRenderer::CFullSceneRenderer()
{
}

CFullSceneRenderer::~CFullSceneRenderer()
{
    __deleteSafely(m_pRainSeqPlayer);
    __deleteSafely(m_pLightningPlayer);
    __deleteSafely(m_pCloudPlayer);
    __deleteSafely(m_pSmallRaindropPlayer);
    __deleteSafely(m_pBigRaindropPlayer);
    __deleteSafely(m_pConfigReader);
}

bool CFullSceneRenderer::init()
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

void CFullSceneRenderer::draw()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    m_pRainSeqPlayer->setCurrentChannel(static_cast<std::uint8_t>(m_RenderChannel));
    m_pRainSeqPlayer->updateMultiChannelFrame(DeltaTime, m_RenderChannel);
    m_pRainSeqPlayer->draw(m_pScreenQuad);
    
    auto renderRainBlock = [&](CSequenceFramePlayer* vPlayer,
        bool vIsInitialized,
        ERenderChannel vChannel1, int vFps1,
        ERenderChannel vChannel2, int vFps2)
    {
        if (!vIsInitialized || !vPlayer) return;
        if (m_RenderChannel != vChannel1 && m_RenderChannel != vChannel2) return;
        vPlayer->updateMultiChannelFrame(DeltaTime, m_RenderChannel);
        vPlayer->setFrameRate(m_RenderChannel == vChannel1 ? vFps1 : vFps2);
        vPlayer->drawMultiChannelKTX(m_pScreenQuad);
    };
    
    renderRainBlock(m_pSmallRaindropPlayer, m_SmallRainDropInitialized,
        ERenderChannel::R, 13,
        ERenderChannel::G, 18);
    
    renderRainBlock(m_pBigRaindropPlayer, m_BigRainDropInitialized,
        ERenderChannel::B, 10,
        ERenderChannel::A, 20);
    
    if (m_CloudInitialized && m_CloudVisible && m_pCloudPlayer)
    {
        m_pCloudPlayer->updateLerpQuantFrame(DeltaTime);
        if (m_RenderChannel == ERenderChannel::R || m_RenderChannel == ERenderChannel::G)
        {
            m_pCloudPlayer->drawInterpolationWithFiltering(m_pScreenQuad);
        }
    }
    
    if (m_LightningInitialized && m_LightningVisible && m_pLightningPlayer)
    {
        m_pLightningPlayer->updateQuantizationFrame(DeltaTime);
        if (m_RenderChannel == ERenderChannel::B || m_RenderChannel == ERenderChannel::A)
        {
            m_pLightningPlayer->draw(m_pScreenQuad);
        }
    }
}

void CFullSceneRenderer::setChannel(ERenderChannel vChannel)
{
    m_RenderChannel = vChannel;

    if (vChannel == ERenderChannel::R)
    {
        if (!m_SmallRainDropInitialized) { __initSmallRainDropPlayer(); }
        if (!m_CloudInitialized)         { __initCloudPlayer(); }
    }

    if (vChannel == ERenderChannel::B)
    {
        if (!m_BigRainDropInitialized)  { __initBigRainDropPlayer(); }
        if (!m_LightningInitialized)    { __initLightningPlayer(); }
    }
}

void CFullSceneRenderer::toggleCloud()
{
    if (!m_CloudInitialized)
    {
        __initCloudPlayer();
        m_CloudVisible = true;
        return;
    }
    m_CloudVisible = !m_CloudVisible;
}

void CFullSceneRenderer::toggleLightning()
{
    if (!m_LightningInitialized)
    {
        __initLightningPlayer();
        m_LightningVisible = true;
        return;
    }
    m_LightningVisible = !m_LightningVisible;
}

void CFullSceneRenderer::__initRainSeqPlayer()
{
    if (m_pRainSeqPlayer) return;
    if (m_pConfigReader == nullptr) m_pConfigReader = new CJsonReader(m_ConfigFile);
    Json::Value RainConfig = m_pConfigReader->getObject("Rain");
    Json::Value BackGroundConfig = m_pConfigReader->getObject("Background");
    std::string RainPath   = RainConfig["frames_path"].asString();
    std::string RainFrameType = RainConfig["frames_type"].asString();
    int RainTextureCount   = RainConfig["frames_count"].asInt();
    int RainOneTextureFrames = RainConfig["one_texture_frames"].asInt();
    float RainFramePerSecond = RainConfig["fps"].asFloat();
    std::string RainVertexShader = RainConfig["vertex_shader"].asString();
    std::string RainFragShader   = RainConfig["fragment_shader"].asString();
    EPictureType::EPictureType RainPictureType = EPictureType::FromString(RainFrameType);
    std::string BackImgPath      = BackGroundConfig["frames_path"].asString();
    std::string BackFrameType    = BackGroundConfig["frames_type"].asString();
    EPictureType::EPictureType BackPicType = EPictureType::FromString(BackFrameType);
    m_pRainSeqPlayer = new CNightSceneSequencePlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);
    m_pRainSeqPlayer->initBackground(BackImgPath, BackPicType);
}

void CFullSceneRenderer::__initSmallRainDropPlayer()
{
    if (m_pSmallRaindropPlayer) return;
    if (m_pConfigReader == nullptr) m_pConfigReader = new CJsonReader(m_ConfigFile);
    Json::Value SmallRaindropConfig = m_pConfigReader->getObject("SmallRaindrop");
    std::string SmallRaindropFramePath = SmallRaindropConfig["frames_path"].asString();
    std::string SmallRaindropFrameType = SmallRaindropConfig["frames_type"].asString();
    int SmallRaindropFrameCount = SmallRaindropConfig["frames_count"].asInt();
    int SmallRaindropTextureFrames = SmallRaindropConfig["one_texture_frames"].asInt();
    float SmallRaindropPlayFPS = SmallRaindropConfig["fps"].asFloat();
    std::string SmallRaindropVertexShader = SmallRaindropConfig["vertex_shader"].asString();
    std::string SmallRaindropFragShader = SmallRaindropConfig["fragment_shader"].asString();
    EPictureType::EPictureType SmallRaindropPicType = EPictureType::FromString(SmallRaindropFrameType);
    m_pSmallRaindropPlayer = new CSequenceFramePlayer(SmallRaindropFramePath, SmallRaindropFrameCount, SmallRaindropTextureFrames, SmallRaindropPlayFPS, SmallRaindropPicType);
    m_pSmallRaindropPlayer->initTextureAndShaderProgram(SmallRaindropVertexShader, SmallRaindropFragShader);
    m_SmallRainDropInitialized = true;
}

void CFullSceneRenderer::__initBigRainDropPlayer()
{
    if (m_pBigRaindropPlayer) return;
    if (m_pConfigReader == nullptr) m_pConfigReader = new CJsonReader(m_ConfigFile);
    Json::Value BigRaindropConfig = m_pConfigReader->getObject("BigRaindrop");
    std::string BigRaindropFramePath = BigRaindropConfig["frames_path"].asString();
    std::string BigRaindropFrameType = BigRaindropConfig["frames_type"].asString();
    int BigRaindropFrameCount = BigRaindropConfig["frames_count"].asInt();
    int BigRaindropTextureFrames = BigRaindropConfig["one_texture_frames"].asInt();
    float BigRaindropPlayFPS = BigRaindropConfig["fps"].asFloat();
    std::string BigRaindropVertexShader = BigRaindropConfig["vertex_shader"].asString();
    std::string BigRaindropFragShader = BigRaindropConfig["fragment_shader"].asString();
    EPictureType::EPictureType BigRaindropPicType = EPictureType::FromString(BigRaindropFrameType);
    m_pBigRaindropPlayer = new CSequenceFramePlayer(BigRaindropFramePath, BigRaindropFrameCount, BigRaindropTextureFrames, BigRaindropPlayFPS, BigRaindropPicType);
    m_pBigRaindropPlayer->initTextureAndShaderProgram(BigRaindropVertexShader, BigRaindropFragShader);
    m_BigRainDropInitialized = true;
}

void CFullSceneRenderer::__initCloudPlayer()
{
    if (m_pCloudPlayer) return;
    if (m_pConfigReader == nullptr) m_pConfigReader = new CJsonReader(m_ConfigFile);
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
}

void CFullSceneRenderer::__initLightningPlayer()
{
    if (m_pLightningPlayer) return;
    if (m_pConfigReader == nullptr) m_pConfigReader = new CJsonReader(m_ConfigFile);
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
    }
    else
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "LightningPlayer initialization failed.");
    }
}