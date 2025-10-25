#include "SequenceFramePlayer.h"
#include <random>
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "log.h"

#define M_PI 3.14159265358979323846

using namespace hiveVG;

CSequenceFramePlayer::CSequenceFramePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
        : m_TextureRootPath(vTextureRootPath), m_SequenceRows(vSequenceRows), m_SequenceCols(vSequenceCols),m_TextureCount(vTextureCount), m_TextureType(vPictureType)
{
    m_ValidFrames = m_SequenceRows * m_SequenceCols;
}

CSequenceFramePlayer::CSequenceFramePlayer(const std::string& vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType)
        : m_TextureRootPath(vTextureRootPath), m_TextureCount(vTextureCount), m_OneTextureFrames(vOneTextureFrames), m_FramePerSecond(vFrameSeconds), m_TextureType(vPictureType)
{
}

CSequenceFramePlayer::~CSequenceFramePlayer()
{
    for (int i = m_SeqTextures.size() - 1; i >= 0; i--)
    {
        if (m_SeqTextures[i])
        {
            delete m_SeqTextures[i];
            m_SeqTextures[i] = nullptr;
            m_SeqTextures.pop_back();
        }
    }
    if (m_pSequenceShaderProgram)
    {
        delete m_pSequenceShaderProgram;
        m_pSequenceShaderProgram = nullptr;
    }
}

bool CSequenceFramePlayer::initShaderProgram(const std::string& vVertexShaderPath, const std::string& vFragShaderShaderPath)
{
    m_pSequenceShaderProgram = CShaderProgram::createProgram(vVertexShaderPath, vFragShaderShaderPath);

    if (!m_pSequenceShaderProgram)
    {
        LOGE(TAG_KEYWORD::SEQFRAME_PALYER_TAG, "[%{public}s] ShaderProgram init Failed.", m_TextureRootPath.c_str());
        return false;
    }
    assert(m_pSequenceShaderProgram != nullptr);
    LOGI(TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%{public}s frames load Succeed. Program Created Succeed.", m_TextureRootPath.c_str());
    return true;
}

bool CSequenceFramePlayer::initTextureAndShaderProgram(const std::string &vVertexShaderPath, const std::string &vFragShaderShaderPath)
{
    if (!m_TextureRootPath.empty() && m_TextureRootPath.back() != '/')
        m_TextureRootPath += '/';

    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)       PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)  PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::ASTC) PictureSuffix = ".astc";

    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexturePath = m_TextureRootPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;
        CTexture2D* pSequenceTexture = CTexture2D::loadTexture(TexturePath, m_SequenceWidth, m_SequenceHeight, m_TextureType);
        if (!pSequenceTexture)
        {
            LOGE(TAG_KEYWORD::SEQFRAME_PALYER_TAG, "Error loading texture from path [%{public}s].", TexturePath.c_str());
            return false;
        }
        m_SeqTextures.push_back(pSequenceTexture);
    }
    m_SeqSingleTexWidth  = m_SequenceWidth / m_SequenceCols;
    m_SeqSingleTexHeight = m_SequenceHeight / m_SequenceRows;

    // Decide shader program: use provided paths if non-empty, else use defaults by picture type
    if (!vVertexShaderPath.empty() && !vFragShaderShaderPath.empty())
    {
        m_pSequenceShaderProgram = CShaderProgram::createProgram(vVertexShaderPath, vFragShaderShaderPath);
    }
    else
    {
        if (m_TextureType == EPictureType::PNG)
            m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertPNG, SeqTexPlayFragPNG);
        else if (m_TextureType == EPictureType::JPG)
            m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertPNG, SeqTexPlayFragJPG);
        else if (m_TextureType == EPictureType::ASTC)
            m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertASTC, SeqTexPlayFragASTC);
    }

    if (!m_pSequenceShaderProgram)
    {
        LOGE(TAG_KEYWORD::SEQFRAME_PALYER_TAG, "[%{public}s] ShaderProgram init Failed.", m_TextureRootPath.c_str());
        return false;
    }
    assert(m_pSequenceShaderProgram != nullptr);
    LOGI(TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%{public}s frames load Succeed. Program Created Succeed.", m_TextureRootPath.c_str());
    return true;
}

void CSequenceFramePlayer::updateSeqFrame(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
    }
}

void CSequenceFramePlayer::drawSeqFrame(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 1);
    m_pSequenceShaderProgram->setUniform("SliderColor", m_uniformColorValue);
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::updateMultiChannelFrame(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0.0;
        m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
        if (m_CurrentChannel == 0)
        {
            m_CurrentTexture++;
            if (m_SeqTextures.size() == m_CurrentTexture)
                m_CurrentTexture = 0;
        }
    }
}

void CSequenceFramePlayer::drawMultiChannelFrame(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 0);
    m_pSequenceShaderProgram->setUniform("channelIndex", m_CurrentChannel);
    m_pSequenceShaderProgram->setUniform("SliderColor", m_uniformColorValue);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::updateMultiChannelFrame(double vDeltaTime, ERenderChannel vRenderChannel)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
        m_CurrentChannel = static_cast<std::uint8_t>(vRenderChannel);
    }
}

void CSequenceFramePlayer::updateCloudLerpMultiChannelFrame(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0.0f;
        m_CurrentChannel++;
        if (m_CurrentChannel == m_OneTextureFrames - 1)
        {
            m_NextTexture++;
            if (m_NextTexture == m_TextureCount)
            {
                m_NextTexture = 0;
                m_IsFinished = true;
            }
        }
        else if (m_CurrentChannel == m_OneTextureFrames)
        {
            m_CurrentTexture = m_NextTexture;
            m_CurrentChannel = 0;
        }
    }
    m_InterpolationFactor = m_AccumFrameTime / FrameTime;
}

void CSequenceFramePlayer::drawCloudLerpMultiChannelFrame(CScreenQuad *vQuad)
{
    //  render cloud using shader with interpolation and filtering
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("Factor", m_InterpolationFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.01f);
    m_pSequenceShaderProgram->setUniform("cloudUVOffset", glm::vec2(0.0,0.75));
    m_pSequenceShaderProgram->setUniform("cloudUVScale", glm::vec2(1,0.4));
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentChannel);
    m_pSequenceShaderProgram->setUniform("TexelSize", glm::vec2( 1.0f / m_SeqSingleTexWidth, 1.0f / m_SeqSingleTexHeight));
    m_pSequenceShaderProgram->setUniform("SliderColor", m_uniformColorValue);
    // 云朵厚度Slider
    m_pSequenceShaderProgram->setUniform("CloudThickness", m_CloudThickness);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_NextTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::setRatioUniform()
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("cloudUVOffset", glm::vec2(0.0,1.2));
    m_pSequenceShaderProgram->setUniform("cloudUVScale", glm::vec2(1,0.3));
}