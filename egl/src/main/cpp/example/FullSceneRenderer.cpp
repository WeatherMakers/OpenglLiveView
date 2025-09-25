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

bool CFullSceneRenderer::init()
{
        std::string FileName   = "configs/RainMultiChannelSeqConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);

    Json::Value RainConfig = JsonReader.getObject("Rain");
    std::string RainPath   =  RainConfig["frames_path"].asString();
    std::string RainFrameType = RainConfig["frames_type"].asString();
    int RainTextureCount   =  RainConfig["frames_count"].asInt();
    int RainOneTextureFrames = RainConfig["one_texture_frames"].asInt();
    float RainFramePerSecond = RainConfig["fps"].asFloat();
    std::string RainVertexShader = RainConfig["vertex_shader"].asString();
    std::string RainFragShader   = RainConfig["fragment_shader"].asString();
    EPictureType::EPictureType RainPictureType = EPictureType::FromString(RainFrameType);

    Json::Value BackGroundConfig = JsonReader.getObject("Background");
    std::string BackImgPath      = BackGroundConfig["frames_path"].asString();
    std::string BackFrameType    = BackGroundConfig["frames_type"].asString();
    std::string BackVertexShader = BackGroundConfig["vertex_shader"].asString();
    std::string BackFragShader   = BackGroundConfig["fragment_shader"].asString();
    EPictureType::EPictureType BackPicType = EPictureType::FromString(BackFrameType);

    Json::Value CloudConfig = JsonReader.getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int   CloudFrameCount = CloudConfig["frames_count"].asInt();
    int   CloudOneTextureFrames = CloudConfig["one_texture_frames"].asInt();
    float CloudPlayFPS    = CloudConfig["fps"].asFloat();
    std::string CloudVertexShader = CloudConfig["vertex_shader"].asString();
    std::string CloudFragShader   = CloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);

    Json::Value SmallRaindropConfig = JsonReader.getObject("SmallRaindrop");
    std::string SmallRaindropFramePath = SmallRaindropConfig["frames_path"].asString();
    std::string SmallRaindropFrameType = SmallRaindropConfig["frames_type"].asString();
    int         SmallRaindropFrameCount = SmallRaindropConfig["frames_count"].asInt();
    int         SmallRaindropTextureFrames = SmallRaindropConfig["one_texture_frames"].asInt();
    float       SmallRaindropPlayFPS    = SmallRaindropConfig["fps"].asFloat();
    std::string SmallRaindropVertexShader = SmallRaindropConfig["vertex_shader"].asString();
    std::string SmallRaindropFragShader   = SmallRaindropConfig["fragment_shader"].asString();
    EPictureType::EPictureType SmallRaindropPicType = EPictureType::FromString(SmallRaindropFrameType);

    Json::Value BigRaindropConfig = JsonReader.getObject("BigRaindrop");
    std::string BigRaindropFramePath = BigRaindropConfig["frames_path"].asString();
    std::string BigRaindropFrameType = BigRaindropConfig["frames_type"].asString();
    int         BigRaindropFrameCount = BigRaindropConfig["frames_count"].asInt();
    int         BigRaindropTextureFrames = BigRaindropConfig["one_texture_frames"].asInt();
    float       BigRaindropPlayFPS    = BigRaindropConfig["fps"].asFloat();
    std::string BigRaindropVertexShader = BigRaindropConfig["vertex_shader"].asString();
    std::string BigRaindropFragShader   = BigRaindropConfig["fragment_shader"].asString();
    EPictureType::EPictureType BigRaindropPicType = EPictureType::FromString(BigRaindropFrameType);

    Json::Value LightningConfig           = JsonReader.getObject("LightningWithMask");
    std::string LightningFramePath        = LightningConfig["frames_path"].asString();
    std::string LightningFrameType        = LightningConfig["frames_type"].asString();
    int         LightningFrameCount       = LightningConfig["frames_count"].asInt();
    int         LightningOneTextureFrames = LightningConfig["one_texture_frames"].asInt();
    std::string LightningPlayMode         = LightningConfig["play_mode"].asString();
    float       LightningPlayFPS          = LightningConfig["fps"].asFloat();
    bool        LightningIsLoop           = LightningConfig["loop"].asBool();
    bool        LightningInFront          = LightningConfig["lightning_front"].asBool();
    std::string LightningVertexShader     = LightningConfig["vertex_shader"].asString();
    std::string LightningFragShader       = LightningConfig["fragment_shader"].asString();
    EPictureType::EPictureType LightningPicType = EPictureType::FromString(LightningFrameType);
    EPlayType::EPlayType LightningPlayType = EPlayType::FromString(LightningPlayMode);

    m_pCloudPlayer = new CSequenceFramePlayer(CloudPath, CloudFrameCount, CloudOneTextureFrames, CloudPlayFPS, CloudPicType);
    m_pCloudPlayer->initTextureAndShaderProgram(CloudVertexShader, CloudFragShader);
    m_pCloudPlayer->setWindowSize(m_WindowSize);
    m_pCloudPlayer->setRatioUniform();

    m_pLightningPlayer = new CLightningSequencePlayer(LightningFramePath, LightningFrameCount, LightningOneTextureFrames, LightningPlayFPS, LightningPicType);
    if(!m_pLightningPlayer->initTextureAndShaderProgram(LightningVertexShader, LightningFragShader))
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "LightningPlayer initialization failed.");
        return false;
    }
    m_pLightningPlayer->setWindowSize(m_WindowSize);
    m_pLightningPlayer->setFrameRate(LightningPlayFPS);
    m_pLightningPlayer->setLoopPlayback(LightningIsLoop);
    m_pLightningPlayer->setLightningMode(LightningInFront);
    if (LightningPlayType == EPlayType::PARTIAL)
    {
        glm::vec2   LightningUVOffset     = glm::vec2(LightningConfig["position"]["x"].asFloat(),
                                                      LightningConfig["position"]["y"].asFloat());
        float       LightningScale        = LightningConfig["scale"].asFloat();

        m_pLightningPlayer->setScreenUVOffset(LightningUVOffset);
        m_pLightningPlayer->setScreenUVScale(glm::vec2(LightningScale, LightningScale));
    }

    m_pRainSeqPlayer = new CNightSceneSequencePlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);
    m_pRainSeqPlayer->initBackground(BackImgPath, BackPicType);

    m_pSmallRaindropPlayer = new CSequenceFramePlayer(SmallRaindropFramePath, SmallRaindropFrameCount, SmallRaindropTextureFrames, SmallRaindropPlayFPS, SmallRaindropPicType);
    m_pSmallRaindropPlayer->initTextureAndShaderProgram(SmallRaindropVertexShader, SmallRaindropFragShader);

    m_pBigRaindropPlayer = new CSequenceFramePlayer(BigRaindropFramePath, BigRaindropFrameCount, BigRaindropTextureFrames, BigRaindropPlayFPS, BigRaindropPicType);
    m_pBigRaindropPlayer->initTextureAndShaderProgram(BigRaindropVertexShader, BigRaindropFragShader);

    m_pScreenQuad   = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::FULL_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    return true;
}

CFullSceneRenderer::~CFullSceneRenderer()
{
    __deleteSafely(m_pRainSeqPlayer);
    __deleteSafely(m_pLightningPlayer);
    __deleteSafely(m_pCloudPlayer);
    __deleteSafely(m_pSmallRaindropPlayer);
    __deleteSafely(m_pBigRaindropPlayer);
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

    m_pSmallRaindropPlayer->updateMultiChannelFrame(DeltaTime, m_RenderChannel);
    m_pBigRaindropPlayer->updateMultiChannelFrame(DeltaTime, m_RenderChannel);

    if (m_RenderChannel == ERenderChannel::R)
    {
        m_pSmallRaindropPlayer->setFrameRate(13);
        m_pSmallRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
    else if (m_RenderChannel == ERenderChannel::G)
    {
        m_pSmallRaindropPlayer->setFrameRate(18);
        m_pSmallRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
    else if (m_RenderChannel == ERenderChannel::B)
    {
        m_pBigRaindropPlayer->setFrameRate(10);
        m_pBigRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
    else if (m_RenderChannel == ERenderChannel::A)
    {
        m_pBigRaindropPlayer->setFrameRate(20);
        m_pBigRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }

    m_pCloudPlayer->updateLerpQuantFrame(DeltaTime);
    m_pLightningPlayer->updateQuantizationFrame(DeltaTime);
    if (m_RenderChannel == ERenderChannel::R || m_RenderChannel == ERenderChannel::G)
    {
        m_pCloudPlayer->drawInterpolationWithFiltering(m_pScreenQuad);
    }
    else if (m_RenderChannel == ERenderChannel::B || m_RenderChannel == ERenderChannel::A)
    {
        m_pLightningPlayer->draw(m_pScreenQuad);
    }

    m_pLightningPlayer->updateQuantizationFrame(DeltaTime);
    m_pLightningPlayer->draw(m_pScreenQuad);
}