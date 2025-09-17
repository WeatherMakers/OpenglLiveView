#include "SeqPlayerRenderer.h"
#include "ScreenQuad.h"
#include "SequenceFramePlayer.h"
#include "log.h"

using namespace hiveVG;

CSeqPlayerRenderer::CSeqPlayerRenderer() {}

CSeqPlayerRenderer::~CSeqPlayerRenderer()
{
    if (m_pTexturePlayer != nullptr)
    {
        delete m_pTexturePlayer;
        m_pTexturePlayer = nullptr;
    }
}

bool CSeqPlayerRenderer::init()
{
    m_pTexturePlayer = new CSequenceFramePlayer(m_TextureRootPath, m_TextureCount, m_OneTextureFrames, m_FrameSeconds, m_PictureType);
    if (!m_pTexturePlayer->initTextureAndShaderProgram())
    {
        LOGE("Failed to initialize sequence texture and shader program");
        return false;
    }
    m_pScreenQuad = CScreenQuad::getOrCreate();
    if (!m_pScreenQuad)
    {
        LOGE("Failed to get CScreenQuad instance");
        return false;
    }
    LOGI("SeqPlayerRenderer initializes successfully");
    return true;
}

void CSeqPlayerRenderer::draw()
{
    glClearColor(0.345, 0.345, 0.445, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    m_pTexturePlayer->updateSeqKTXFrame(0.016f);
    m_pTexturePlayer->drawSeqKTX(m_pScreenQuad);
}
