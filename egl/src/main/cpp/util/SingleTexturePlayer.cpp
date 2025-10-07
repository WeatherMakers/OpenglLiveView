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

CSingleTexturePlayer::~CSingleTexturePlayer()
{
    for (int i = m_SeqTextures.size() - 1; i >= 0; i--)
    {
        if (m_SeqTextures[i] != nullptr)
        {
            delete m_SeqTextures[i];
            m_SeqTextures[i] = nullptr;
            m_SeqTextures.pop_back();
        }
    }
    if (m_pSingleShaderProgram != nullptr)
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
        LOGE(TAG_KEYWORD::SINGLE_TEXTURE_PLAYER_TAG, "Error loading texture from path [%{public}s].", m_TexturePath.c_str());
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
    LOGI(TAG_KEYWORD::SINGLE_TEXTURE_PLAYER_TAG, "%{public}s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
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
            LOGE(TAG_KEYWORD::SINGLE_TEXTURE_PLAYER_TAG, "Error loading texture from path [%{public}s].", m_TexturePath.c_str());
            return false;
        }
        m_SeqTextures.push_back(pSingleTexture);
    }

    m_pSingleShaderProgram = CShaderProgram::createProgram(vVertexShaderPath, vFragShaderShaderPath);

    assert(m_pSingleShaderProgram != nullptr);
    LOGI(TAG_KEYWORD::SINGLE_TEXTURE_PLAYER_TAG, "%{public}s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
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