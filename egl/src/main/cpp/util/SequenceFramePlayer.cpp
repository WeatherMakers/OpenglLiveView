#include "SequenceFramePlayer.h"
#include <random>
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "log.h"

#define M_PI 3.14159265358979323846

using namespace hiveVG;

CSequenceFramePlayer::CSequenceFramePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
        : m_SequenceRows(vSequenceRows), m_SequenceCols(vSequenceCols), m_TextureRootPath(vTextureRootPath), m_TextureCount(vTextureCount), m_TextureType(vPictureType)
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

bool CSequenceFramePlayer::initTextureAndShaderProgram()
{
    if (m_TextureType == EPictureType::PNG)
        m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertPNG, SeqTexPlayFragPNG);
    else if (m_TextureType == EPictureType::JPG)
        m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertPNG, SeqTexPlayFragJPG);
    else if (m_TextureType == EPictureType::ASTC)
        m_pSequenceShaderProgram = CShaderProgram::createProgram(SeqTexPlayVertKTX, SeqTexPlayFragKTX);

    if (!m_pSequenceShaderProgram)
    {
        LOGE(TAG_KEYWORD::SEQFRAME_PALYER_TAG, "[%{public}s] ShaderProgram init Failed.", m_TextureRootPath.c_str());
        return false;
    }
    assert(m_pSequenceShaderProgram != nullptr);
    
    if (!m_TextureRootPath.empty() && m_TextureRootPath.back() != '/')
        m_TextureRootPath += '/';
    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)        PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)   PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::ASTC)  PictureSuffix = ".astc";

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
    
//    std::vector<std::string> TexturePaths;
//    TexturePaths.reserve(m_TextureCount);
//    for (int i = 0; i < m_TextureCount; i++) 
//    {
//        std::string TexturePath = m_TextureRootPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;
//        TexturePaths.push_back(TexturePath);
//    }
//
//    m_SeqTextures = CTexture2D::loadTexturesBatch(TexturePaths); 
    
    m_SeqSingleTexWidth  = m_SequenceWidth / m_SequenceCols;
    m_SeqSingleTexHeight = m_SequenceHeight / m_SequenceRows;
    
    LOGI(TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%{public}s frames load Succeed. Program Created Succeed.", m_TextureRootPath.c_str());
    return true;
}

bool CSequenceFramePlayer::initTextureAndShaderProgram(const std::string& vVertexShaderPath, const std::string& vFragShaderShaderPath)
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

void CSequenceFramePlayer::updateQuantizationFrame(double vDeltaTime)
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
//        LOGI("SeqTexture: %{public}d, Current Channel: %{public}d" , m_CurrentTexture, m_CurrentChannel);
    }
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
//        LOGI("RainSeqTexture: %{public}d, RainCurrentChannel: %{public}d" , m_CurrentTexture, m_CurrentChannel);
    }
}

void CSequenceFramePlayer::updateInterpolationFrame(double vDeltaTime)
{
    // TODO: 目前仅针对单行单列素材做插值，合并大图插值待添加
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0.0f;
        m_CurrentTexture = m_NextTexture;
        m_CurrentFrame = m_NextFrame;
        if (m_NextFrame == m_ValidFrames - 1)
        {
            if (m_NextTexture == m_TextureCount - 1) m_IsFinished = true;
            m_NextTexture = (m_NextTexture + 1) % m_TextureCount;
        }
        m_NextFrame = (m_NextFrame + 1) % m_ValidFrames;
    }
    m_InterpolationFactor = m_AccumFrameTime / FrameTime;
}

void CSequenceFramePlayer::updateLerpQuantFrame(double vDeltaTime)
{
    // TODO: 目前仅针对单行单列素材做插值，合并大图插值待添加
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
        else if(m_CurrentChannel == m_OneTextureFrames)
        {
            m_CurrentTexture = m_NextTexture;
            m_CurrentChannel = 0;
        }
//        LOGI("Current Channel: %{public}d, CurrentSeqTexture: %{public}d, NextSeqTexture: %{public}d" , m_CurrentChannel, m_CurrentTexture,  m_NextTexture);
    }
    m_InterpolationFactor = m_AccumFrameTime / FrameTime;
}

void CSequenceFramePlayer::updateSeqKTXFrame(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
    }
}

void CSequenceFramePlayer::updateFrameAndUV(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
//    LOGI("DeltaTime: %lf", vDeltaTime);
    if (m_AccumFrameTime >= FrameTime)
    {
//        LOGI("update Frame");
        m_AccumFrameTime = 0.0f;
        if (m_CurrentFrame == m_ValidFrames - 1)
        {
            if (m_CurrentTexture == m_TextureCount - 1) m_IsFinished = true;
            m_CurrentTexture = (m_CurrentTexture + 1) % m_TextureCount;
        }
        m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
    }

    if (m_IsMoving)
    {
        if (m_UseLifeCycle)
        {
            // TODO : update logic
            if (!m_SequenceState._IsAlive)
            {
                m_SequenceState._AlreadyDeadTime += float(vDeltaTime);
                if (m_SequenceState._AlreadyDeadTime > m_SequenceState._PlannedDeadTime)
                {
                    __initSequenceParams();
                    m_SequenceState._IsAlive = true;
                }
            }
            else
            {
                m_SequenceState._AlreadyLivingTime += float(vDeltaTime);
                if (m_SequenceState._AlreadyLivingTime > m_SequenceState._PlannedLivingTime)
                    m_SequenceState._IsAlive = false;
            }
        }
        m_ScreenUVOffset += m_MovingSpeed * float(vDeltaTime);
        float ScreenMaxUV = 1.0f;
        if (m_ScreenUVOffset.x > ScreenMaxUV + m_ScreenUVScale.x || m_ScreenUVOffset.x < -ScreenMaxUV - m_ScreenUVScale.x)
            m_ScreenUVOffset.x = -ScreenMaxUV - m_ScreenUVScale.x;
        if (m_ScreenUVOffset.y > ScreenMaxUV + m_ScreenUVScale.y || m_ScreenUVOffset.y < -ScreenMaxUV - m_ScreenUVScale.y)
            m_ScreenUVOffset.y = -ScreenMaxUV - m_ScreenUVScale.y;
    }
}

void CSequenceFramePlayer::draw(CScreenQuad *vQuad)
{
    if (m_UseLifeCycle && !m_SequenceState._IsAlive)
        return ;

    if (!m_IsLoop && m_IsFinished)
    {
        m_CurrentFrame   = m_ValidFrames - 1;
        m_CurrentTexture = m_TextureCount - 1;
    }
    float RotationAngle   = m_RotationAngle * M_PI / 180.0f;
    int   CurrentFrameRow = m_CurrentFrame / m_SequenceCols;
    int   CurrentFrameCol = m_CurrentFrame % m_SequenceCols;
    float CurrentFrameU0 = static_cast<float>(CurrentFrameCol) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV0 = static_cast<float>(CurrentFrameRow) / static_cast<float>(m_SequenceRows);
    float CurrentFrameU1 = static_cast<float>(CurrentFrameCol + 1) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV1 = static_cast<float>(CurrentFrameRow + 1) / static_cast<float>(m_SequenceRows);
    glm::vec2 TextureUVOffset = glm::vec2(CurrentFrameU0, CurrentFrameV0);
    glm::vec2 TextureUVScale  = glm::vec2(CurrentFrameU1 - CurrentFrameU0, CurrentFrameV1 - CurrentFrameV0);

    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("rotationAngle", RotationAngle);
    m_pSequenceShaderProgram->setUniform("screenUVOffset", m_ScreenUVOffset);
    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale);
    m_pSequenceShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSequenceShaderProgram->setUniform("texUVScale", TextureUVScale);
    m_pSequenceShaderProgram->setUniform("sequenceTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::drawQuantization(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 2);
    glActiveTexture(GL_TEXTURE2);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::drawSeqKTX(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 1);
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::drawMultiChannelKTX(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("indexTexture", 0);
    m_pSequenceShaderProgram->setUniform("channelIndex", m_CurrentChannel);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::drawInterpolation(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("Factor", m_InterpolationFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.0f);
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentChannel);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_NextTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::drawInterpolationWithDisplacement(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("Factor", m_InterpolationFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.01f);
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentChannel);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_SeqTextures[m_NextTexture]->bindTexture();
    vQuad->bindAndDraw();
}

void CSequenceFramePlayer::drawInterpolationWithFiltering(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);
    m_pSequenceShaderProgram->setUniform("Factor", m_InterpolationFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.01f);
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentChannel);
    m_pSequenceShaderProgram->setUniform("TexelSize", glm::vec2( 1.0f / m_SeqSingleTexWidth, 1.0f / m_SeqSingleTexHeight));
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
void CSequenceFramePlayer::__initSequenceParams()
{
    std::random_device Rd;
    std::mt19937 Gen(Rd());
    std::uniform_int_distribution<> IntDistribution(0, 1);
    std::uniform_real_distribution<float> FloatDistribution(0.0, 1.0);

    m_SequenceState._IsAlive = true;

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(30.0f, 40.0f));
    m_SequenceState._PlannedLivingTime = FloatDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(1.0f, 2.0f));
    m_SequenceState._PlannedDeadTime   = FloatDistribution(Gen);
    m_SequenceState._AlreadyDeadTime   = 0;
    m_SequenceState._AlreadyLivingTime = 0;

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(0.2f, 1.0f));
    float ScreenRandomUV = FloatDistribution(Gen);
    m_ScreenUVScale = glm::vec2(ScreenRandomUV, ScreenRandomUV);

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(-0.5f, 0.5f));
    float ScreenRandomOffset = FloatDistribution(Gen);
    // TODO : move from up to down is to be completed later
    float ScreenMaxUV = 1.0f;
    m_ScreenUVOffset = m_MovingSpeed.x > 0 ? glm::vec2(-ScreenMaxUV - ScreenRandomUV, ScreenRandomOffset) : glm::vec2(ScreenMaxUV + ScreenRandomUV, ScreenRandomOffset);

    float MovingDistance = 2.0f + 2 * m_SequenceState._UVScale; // 2.0f is from -1.0 ~ 1.0; * 2 is from left to right
    float Speed = MovingDistance / m_SequenceState._PlannedLivingTime;
    m_MovingSpeed.x = m_MovingSpeed.x > 0 ? Speed : -Speed;
}