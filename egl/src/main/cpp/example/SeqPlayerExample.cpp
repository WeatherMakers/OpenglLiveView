#include "SeqPlayerExample.h"
#include "GLUtil.h"
#include "log.h"
#include "render/EglRender.h"
#include <GLES3/gl3.h>

using namespace hiveVG;

CSeqPlayerExample::CSeqPlayerExample() {}

CSeqPlayerExample::~CSeqPlayerExample()
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

bool CSeqPlayerExample::init()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    if (!m_pScreenQuad)
    {
        LOGE("Failed to get CScreenQuad instance");
        return false;
    }
    
    m_pTexturePlayer = new CSequenceFramePlayer(m_TextureRootPath, m_TextureCount, m_OneTextureFrames, m_FrameSeconds, m_PictureType);
    
    if (!m_pTexturePlayer->initTextureAndShaderProgram())
    {
        LOGE("Failed to initialize sequence texture and shader program");
        return false;
    }
    
    LOGI("SeqPlayerExample initialized successfully");
    return true;
}

void CSeqPlayerExample::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    m_pTexturePlayer->updateSeqKTXFrame(0.016f);
    m_pTexturePlayer->drawSeqKTX(m_pScreenQuad);
}
