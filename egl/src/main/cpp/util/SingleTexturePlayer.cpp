#include "SingleTexturePlayer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "log.h"

using namespace hiveVG;

CSingleTexturePlayer::CSingleTexturePlayer(const std::string& vTexturePath, EPictureType::EPictureType vPictureType)
        : m_TexturePath(vTexturePath), m_TextureType(vPictureType)
{
}

CSingleTexturePlayer::CSingleTexturePlayer(const std::string& vTexturePath, int vSequenceRows, int vSequenceCols, int  vOneTextureFrames, EPictureType::EPictureType vPictureType, float vFramePerSecond,int vTextureCount = 1)
        : m_TexturePath(vTexturePath),m_SeqRows(vSequenceRows),m_SeqCols(vSequenceCols),m_TextureCount(vTextureCount),m_OneTextureFrames(vOneTextureFrames),m_TextureType(vPictureType),m_FramePerSecond(vFramePerSecond)
{
    m_ValidFrames = m_SeqCols * m_SeqRows;
}

CSingleTexturePlayer::~CSingleTexturePlayer()
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
    if (m_pSingleShaderProgram)
    {
        delete m_pSingleShaderProgram;
        m_pSingleShaderProgram = nullptr;
    }
}

bool CSingleTexturePlayer::initTextureAndShaderProgram()
{
    CTexture2D* pSingleTexture = CTexture2D::loadTexture(m_TexturePath, m_TextureType);
    if (!pSingleTexture)
    {
        LOGE( "Error loading texture from path [%{public}s].", m_TexturePath.c_str());
        return false;
    }
    m_SeqTextures.push_back(pSingleTexture);

    if (m_TextureType == EPictureType::PNG)
        m_pSingleShaderProgram = CShaderProgram::createProgram(SingleTexPlayVert, SingleTexPlayFragPNG);
    else if (m_TextureType == EPictureType::JPG)
        m_pSingleShaderProgram = CShaderProgram::createProgram(SingleTexPlayVert, SingleTexPlayFragJPG);
    else if (m_TextureType == EPictureType::ASTC)
        m_pSingleShaderProgram = CShaderProgram::createProgram(SingleTexPlayVert, SingleTexPlayFragASTC);

    assert(m_pSingleShaderProgram != nullptr);
    LOGI("%{public}s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
    return true;
}

bool CSingleTexturePlayer::initTextureAndShaderProgram(std::string& vVertexShaderPath, std::string& vFragShaderShaderPath)
{
    if (!m_TexturePath.empty() && m_TexturePath.back() != '/')
        m_TexturePath += '/';

    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)       PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)  PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::ASTC) PictureSuffix = ".astc";

    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexturePath = m_TexturePath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;

        CTexture2D* pSingleTexture = CTexture2D::loadTexture(TexturePath, m_SeqSingleTexWidth, m_SeqSingleTexHeight, m_TextureType);
        if (!pSingleTexture)
        {
            LOGE("Error loading texture from path [%{public}s].", m_TexturePath.c_str());
            return false;
        }
        m_SeqTextures.push_back(pSingleTexture);
    }

    m_pSingleShaderProgram = CShaderProgram::createProgram(vVertexShaderPath, vFragShaderShaderPath);

    assert(m_pSingleShaderProgram != nullptr);
    LOGI( "%{public}s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
    return true;
}

void CSingleTexturePlayer::updateFrame()
{
    assert(m_pSingleShaderProgram != nullptr);
    m_pSingleShaderProgram->useProgram();
    glActiveTexture(GL_TEXTURE0);
    m_pSingleShaderProgram->setUniform("quadTexture", 0);
    m_SeqTextures[0]->bindTexture();
}

void CSingleTexturePlayer::updateCompressedFrame(EPlayMode::EPlayMode vPlayMode, double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;

        if (vPlayMode == EPlayMode::DEPTH)
        {
            m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
            if (m_CurrentChannel == 0)
            {
                m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
                if (m_CurrentFrame == 0)
                    m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
            }
        }
        else if (vPlayMode == EPlayMode::CHANNEL)
        {
            m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
            if (m_CurrentFrame == 0)
            {
                m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
                if (m_CurrentChannel == 0)
                    m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
            }
        }
        LOGI( "Frame: %{public}d, SeqTexture: %{public}d, Current Channel: %{public}d", m_CurrentFrame, m_CurrentTexture, m_CurrentChannel);
    }
}

void CSingleTexturePlayer::drawCompressedFrame(CScreenQuad *vQuad)
{
    assert(m_pSingleShaderProgram != nullptr);

    int   CurrentFrameRow = m_CurrentFrame / m_SeqCols;
    int   CurrentFrameCol = m_CurrentFrame % m_SeqCols;
    float CurrentFrameU0 = static_cast<float>(CurrentFrameCol) / static_cast<float>(m_SeqCols);
    float CurrentFrameV0 = static_cast<float>(CurrentFrameRow) / static_cast<float>(m_SeqRows);
    float CurrentFrameU1 = static_cast<float>(CurrentFrameCol + 1) / static_cast<float>(m_SeqCols);
    float CurrentFrameV1 = static_cast<float>(CurrentFrameRow + 1) / static_cast<float>(m_SeqRows);
    glm::vec2 TextureUVOffset = glm::vec2(CurrentFrameU0, CurrentFrameV0);
    glm::vec2 TextureUVScale  = glm::vec2(CurrentFrameU1 - CurrentFrameU0, CurrentFrameV1 - CurrentFrameV0);
    m_pSingleShaderProgram->useProgram();
    m_pSingleShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSingleShaderProgram->setUniform("texUVScale",  TextureUVScale);
    m_pSingleShaderProgram->setUniform("indexTexture", 0);
    m_pSingleShaderProgram->setUniform("channelIndex", m_CurrentChannel);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}