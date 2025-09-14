#include "SinglePlayerExample.h"
#include "GLUtil.h"
#include "log.h"
#include "render/EglRender.h"
#include <GLES3/gl3.h>
#include <string>

using namespace hiveVG;

CSinglePlayerExample::CSinglePlayerExample() {}

CSinglePlayerExample::~CSinglePlayerExample()
{
    if (m_pTexturePlayer != nullptr)
    {
        delete m_pTexturePlayer;
        m_pTexturePlayer = nullptr;
    }
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
}

bool CSinglePlayerExample::init()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    if (!m_pScreenQuad)
    {
        LOGE("Failed to get CScreenQuad instance");
        return false;
    }
    
    m_pTexturePlayer = new CSingleTexturePlayer("snowScene.astc", EPictureType::ASTC);
    if (!m_pTexturePlayer->initTextureAndShaderProgram())
    {
        LOGE("Failed to initialize texture and shader program");
        return false;
    }
    return true;
}

void CSinglePlayerExample::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_pTexturePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}