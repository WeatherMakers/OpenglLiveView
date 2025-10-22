#include "CloudSceneRenderer.h"
#include "SequenceFramePlayer.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "log.h"

using namespace hiveVG;

CCloudSceneRenderer::CCloudSceneRenderer() {}

CCloudSceneRenderer::~CCloudSceneRenderer()
{
    if (m_pTexturePlayer != nullptr)
    {
        delete m_pTexturePlayer;
        m_pTexturePlayer = nullptr;
    }
}

bool CCloudSceneRenderer::init()
{
    m_pTexturePlayer = new CSequenceFramePlayer(m_TextureRootPath, m_TextureCount, m_OneTextureFrames, m_FrameSeconds, m_PictureType);
    if (!m_pTexturePlayer->initTextureAndShaderProgram(SeqTexPlayVertASTC, SeqTexPlayFragRainCloudASTC))
    {
        LOGE(TAG_KEYWORD::CLOUD_SCENE_RENDERER_TAG, "Failed to initialize sequence texture and shader program");
        return false;
    }
    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::CLOUD_SCENE_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }
    LOGI(TAG_KEYWORD::CLOUD_SCENE_RENDERER_TAG, "CloudSceneRenderer initialized successfully");
    return true;
}

void CCloudSceneRenderer::draw()
{
    glClearColor(0.6f, 0.7f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    m_pTexturePlayer->updateSeqFrame(0.016f);
    m_pTexturePlayer->drawSeqFrame(m_pScreenQuad);
    m_pScreenQuad->bindAndDraw();
}
