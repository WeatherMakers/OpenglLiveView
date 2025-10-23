#include "SinglePlayerRenderer.h"
#include "SingleTexturePlayer.h"
#include "ScreenQuad.h"
#include "log.h"

using namespace hiveVG;

CSinglePlayerRenderer::CSinglePlayerRenderer() {}

CSinglePlayerRenderer::~CSinglePlayerRenderer()
{
    if (m_pTexturePlayer != nullptr)
    {
        delete m_pTexturePlayer;
        m_pTexturePlayer = nullptr;
    }
}

bool CSinglePlayerRenderer::init()
{
    m_pTexturePlayer = new CSingleTexturePlayer(m_TexturePath, m_TextureType);
    if (!m_pTexturePlayer->initTextureAndShaderProgram())
    {
        LOGE(TAG_KEYWORD::SINGLE_PLAYER_RENDERER_TAG, "Failed to initialize texture and shader program");
        return false;
    }
    m_pScreenQuad = &CScreenQuad::getInstance();
    if (!m_pScreenQuad->init())
    {
        LOGE(TAG_KEYWORD::SINGLE_PLAYER_RENDERER_TAG, "Failed to initialize CScreenQuad");
        return false;
    }
    LOGI(TAG_KEYWORD::SINGLE_PLAYER_RENDERER_TAG, "SinglePlayer inits successfully.");
    return true;
}

void CSinglePlayerRenderer::draw()
{
    glClearColor(0.345f, 0.345f, 0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    m_pTexturePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}