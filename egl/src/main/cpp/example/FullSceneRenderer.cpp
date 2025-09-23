#include "FullSceneRenderer.h"
#include "SequenceFramePlayer.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "log.h"

using namespace hiveVG;

CFullSceneRenderer::CFullSceneRenderer() {}

CFullSceneRenderer::~CFullSceneRenderer()
{
    if (m_pTexturePlayer != nullptr)
    {
        delete m_pTexturePlayer;
        m_pTexturePlayer = nullptr;
    }
}

bool CFullSceneRenderer::init()
{
    m_pBackGroundPlayer = new CSingleTexturePlayer(m_BackgroundTexPath,m_PictureType);
    if(!m_pBackGroundPlayer->initTextureAndShaderProgram())
    {
        LOGE("Failed to initialize single texture and shader program");
        return false;
    }
    
    m_pTexturePlayer = new CSequenceFramePlayer(m_TextureRootPath, m_TextureCount, m_OneTextureFrames, m_FrameSeconds, m_PictureType);
    if (!m_pTexturePlayer->initTextureAndShaderProgram())
    {
        LOGE("Failed to initialize sequence texture and shader program");
        return false;
    }
    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE("Failed to initialize CScreenQuad");
        return false;
    }
    LOGI("FullSceneRenderer initialized successfully");
    return true;
}

void CFullSceneRenderer::draw()
{
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    m_pTexturePlayer->updateSeqKTXFrame(0.016f);
    m_pTexturePlayer->drawSeqKTX(m_pScreenQuad);
    m_pBackGroundPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}

