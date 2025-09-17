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
    m_pTexturePlayer = new CSingleTexturePlayer(m_TexturePath, EPictureType::ASTC);
    if (!m_pTexturePlayer->initTextureAndShaderProgram())
    {
        LOGE("Failed to initialize texture and shader program");
        return false;
    }
    m_pScreenQuad = CScreenQuad::getOrCreate();
    if (!m_pScreenQuad)
    {
        LOGE("Failed to get CScreenQuad instance");
        return false;
    }
    LOGI("SinglePlayer inits successfully.");
    return true;
}

void CSinglePlayerRenderer::draw()
{
    glClearColor(0.345f, 0.445f, 0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    m_pTexturePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}