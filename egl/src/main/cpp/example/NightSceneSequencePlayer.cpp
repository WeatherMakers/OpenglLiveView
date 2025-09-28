#include "NightSceneSequencePlayer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "log.h"

using namespace hiveVG;
CNightSceneSequencePlayer::CNightSceneSequencePlayer(const std::string &vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFramePerSecond, EPictureType::EPictureType vPictureType)
        : CSequenceFramePlayer(vTextureRootPath, vTextureCount, vOneTextureFrames, vFramePerSecond, vPictureType)
{ }

CNightSceneSequencePlayer::~CNightSceneSequencePlayer()
{
    if(m_pBackground)
    {
        delete m_pBackground;
        m_pBackground = nullptr;
    }
}

void CNightSceneSequencePlayer::initBackground(const std::string &vTexturePath, EPictureType::EPictureType vPictureType)
{
    int Width, Height;
    m_pBackground = CTexture2D::loadTexture(vTexturePath, Width, Height, vPictureType);
    if (!m_pBackground)
    {
        LOGE(TAG_KEYWORD::NIGHT_SCENE_SEQUENCE_PLAYER_TAG, "Error loading texture from path [%{public}s].", vTexturePath.c_str());
        return;
    }
    LOGI(TAG_KEYWORD::NIGHT_SCENE_SEQUENCE_PLAYER_TAG, "Successfully loading texture from path [%{public}s].", vTexturePath.c_str());
}

void CNightSceneSequencePlayer::draw(CScreenQuad *vQuad)
{
    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("channelIndex", m_CurrentChannel);
    m_pSequenceShaderProgram->setUniform("rainSequenceTexture", 0);
    m_pSequenceShaderProgram->setUniform("backgroundTexture", 1);

    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_pBackground->bindTexture();
    vQuad->bindAndDraw();
}
