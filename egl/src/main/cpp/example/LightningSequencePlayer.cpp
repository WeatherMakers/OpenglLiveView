#include "LightningSequencePlayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "log.h"

using namespace hiveVG;

CLightningSequencePlayer::CLightningSequencePlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
    : CSequenceFramePlayer(vTextureRootPath, vSequenceRows, vSequenceCols, vTextureCount, vPictureType)
{}

CLightningSequencePlayer::CLightningSequencePlayer(const std::string& vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType)
        : CSequenceFramePlayer(vTextureRootPath, vTextureCount, vOneTextureFrames, vFrameSeconds, vPictureType)
{
    CJsonReader JsonReader = CJsonReader(m_ConfigFileName);

    Json::Value CloudConfig = JsonReader.getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int   CloudFrameCount = CloudConfig["frames_count"].asInt();
    int   CloudOneTextureFrames = CloudConfig["one_texture_frames"].asInt();
    float CloudPlayFPS    = CloudConfig["fps"].asFloat();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);

    __initCloudTextures(CloudPath, CloudFrameCount, CloudPicType);
    m_CloudFPS          = CloudPlayFPS;
    m_OneCloudTexFrames = CloudOneTextureFrames;
}

CLightningSequencePlayer::~CLightningSequencePlayer()
{
}

void CLightningSequencePlayer::updateFrameAndUV(double vDeltaTime)
{
    if (m_IsWaiting)
    {
        m_WaitTime += vDeltaTime;
        if (m_WaitTime >= m_TargetWaitTime)
        {
            __resetPlayback();
        }
        return;
    }

    if (m_FramePerSecond <= 0.0) return;
    double FrameDuration = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;

    if (m_AccumFrameTime >= FrameDuration)
    {
        m_AccumFrameTime = 0.0;

        if (m_CurrentFrame == m_ValidFrames - 1)
        {
            if (m_CurrentTexture == m_TextureCount - 1)
            {
                m_IsFinished = true;
                m_IsWaiting  = true;
                m_TargetWaitTime = m_WaitDist(m_Rng);
                return;
            }
            else
            {
                m_CurrentTexture++;
            }
        }
        m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
    }
}

void CLightningSequencePlayer::updateQuantizationFrame(double vDeltaTime)
{
    // === 更新云序列帧 ===
    double CloudFrameTime = 1.0 / m_CloudFPS;
    m_AccumCloudTime += vDeltaTime;
    if (m_AccumCloudTime >= CloudFrameTime)
    {
        m_AccumCloudTime = 0.0;
        m_CurrentCloudChannel++;
        if (m_CurrentCloudChannel == m_OneCloudTexFrames - 1)
        {
            m_NextCloudTexture++;
            if (m_NextCloudTexture == m_SeqCloudTextures.size())
            {
                m_NextCloudTexture = 0;
            }
        }
        else if (m_CurrentCloudChannel == m_OneCloudTexFrames)
        {
            m_CurrentCloudTexture = m_NextCloudTexture;
            m_CurrentCloudChannel = 0;
        }
    }
    m_CloudInterpFactor = m_AccumCloudTime / CloudFrameTime;

    // === 处理等待逻辑 ===
    if (m_IsWaiting)
    {
        m_WaitTime += vDeltaTime;
        if (m_WaitTime >= m_TargetWaitTime)
        {
            __resetPlayback();
        }
        return;
    }

    // === 更新闪电帧 ===
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;

    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0;
        m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
        if (m_CurrentChannel == 0)
        {
            m_CurrentTexture++;
            if (m_CurrentTexture >= m_SeqTextures.size() / 2)
            {
                m_CurrentTexture = 0;
                m_IsFinished = true;
                m_IsWaiting = true;
                m_TargetWaitTime = m_WaitDist(m_Rng);
                return;
            }
        }
    }
}

void CLightningSequencePlayer::draw(CScreenQuad *vQuad)
{
    float RotationAngle = glm::radians(static_cast<float>(m_RotationAngle));
    float FlashProgress = (float)m_CurrentTexture / ((float)m_TextureCount / 2.0f - 1.0f);
    FlashProgress = glm::clamp(FlashProgress, 0.0f, 1.0f);
    if (FlashProgress >= 0.9) FlashProgress = 0;
    int BindTextureIndex = m_CurrentTexture + (m_LightningInFront ? 0 : 8);

    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("rotationAngle", RotationAngle);
    m_pSequenceShaderProgram->setUniform("screenUVOffset", m_ScreenUVOffset);
    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale);
    m_pSequenceShaderProgram->setUniform("cloudUVOffset", glm::vec2(0.0,0.75));
    m_pSequenceShaderProgram->setUniform("cloudUVScale", glm::vec2(1,0.4));
    m_pSequenceShaderProgram->setUniform("isFinish", m_IsFinished);

    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("LightningSequenceTexture", 2);
    m_pSequenceShaderProgram->setUniform("FlashProgress", FlashProgress);
    m_pSequenceShaderProgram->setUniform("FlashColor", glm::vec3(1.0f));
    m_pSequenceShaderProgram->setUniform("FlashAlpha", 0.3f);
    m_pSequenceShaderProgram->setUniform("LightningInFront", m_LightningInFront);
    m_pSequenceShaderProgram->setUniform("ChannelIndex", m_CurrentChannel);
    m_pSequenceShaderProgram->setUniform("Factor", m_CloudInterpFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.01f);
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentCloudChannel);
    m_pSequenceShaderProgram->setUniform("TexelSize", glm::vec2(1.0f / static_cast<float>(m_CloudSingleTexWidth), 1.0f / static_cast<float>(m_CloudSingleTexHeight)));
    
    glActiveTexture(GL_TEXTURE0);
    m_SeqCloudTextures[m_CurrentCloudTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_SeqCloudTextures[m_NextCloudTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE2);
    m_SeqTextures[BindTextureIndex]->bindTexture();
    vQuad->bindAndDraw();
}

bool CLightningSequencePlayer::initTextureAndShaderProgram(const std::string &vVertexShaderPath, const std::string &vFragShaderShaderPath)
{
    return CSequenceFramePlayer::initTextureAndShaderProgram(vVertexShaderPath, vFragShaderShaderPath);
}

void CLightningSequencePlayer::__resetPlayback()
{

    m_IsFinished = false;
    m_IsWaiting  = false;
    m_WaitTime   = 0.0;
    m_CurrentFrame   = 0;
    m_CurrentTexture = 0;
    m_CurrentChannel = 0;

    __randomizeLightningParameters();
}

void CLightningSequencePlayer::__randomizeLightningParameters()
{
    m_ScreenUVScale.x = m_ScaleDist(m_Rng);
    m_ScreenUVScale.y = m_ScaleDist(m_Rng);

    float MaxOffsetX = 1.0f - m_ScreenUVScale.x;
    std::uniform_real_distribution<float> OffsetXDist(-0.8f, MaxOffsetX);
    m_ScreenUVOffset.x = OffsetXDist(m_Rng);

    float MinOffsetY = std::lerp(1.2f, 0.6f, (m_ScreenUVScale.y - m_ScaleMin) / (m_ScaleMax - m_ScaleMin));
    float MaxOffsetY = std::min(MinOffsetY + 0.1f, 1.3f);

    std::uniform_real_distribution<float> OffsetYDist(MinOffsetY, MaxOffsetY);
    m_ScreenUVOffset.y = OffsetYDist(m_Rng);

    m_LightningInFront = m_BoolDist(m_Rng) == 1;
}

void CLightningSequencePlayer::__initCloudTextures(const std::string& vCloudPath, int vFrameCount, EPictureType::EPictureType vCloudPicType)
{
    int TexWidth, TexHeight;
    std::string CloudPath;

    if (!vCloudPath.empty() && vCloudPath.back() != '/')
        CloudPath = vCloudPath + '/';

    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)       PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)  PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::ASTC) PictureSuffix = ".astc";

    for (int i = 0; i < vFrameCount; i++)
    {
        std::string TexturePath = CloudPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;
        CTexture2D* pSequenceTexture = CTexture2D::loadTexture(TexturePath, TexWidth, TexHeight, vCloudPicType);
        if (!pSequenceTexture)
        {
            LOGE(TAG_KEYWORD::LIGHTNING_SEQUENCE_PLAYER_TAG, "Error loading texture from path [%{public}s].", TexturePath.c_str());
            return ;
        }
        m_SeqCloudTextures.push_back(pSequenceTexture);
    }
    m_CloudSingleTexWidth  = TexWidth;
    m_CloudSingleTexHeight = TexHeight;
}
