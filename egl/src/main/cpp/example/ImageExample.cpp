#include "ImageExample.h"
#include <GLES3/gl3.h>
#include <string>
#include "GLUtil.h"
#include "log.h"
#include "render/EglRender.h"

using namespace hiveVG;

CImageExample::CImageExample()
    : m_pShaderProgram(nullptr), m_pTexture(nullptr), m_pScreenQuad(nullptr), m_bInitialized(false)
{
}

CImageExample::~CImageExample()
{
    if (m_pShaderProgram != nullptr)
    {
        delete m_pShaderProgram;
        m_pShaderProgram = nullptr;
    }
    if (m_pTexture != nullptr)
    {
        delete m_pTexture;
        m_pTexture = nullptr;
    }
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
}

bool CImageExample::init()
{
    if (m_bInitialized)
    {
        LOGD("CImageExample already initialized, skipping...");
        return true;
    }
    
    m_pScreenQuad = CScreenQuad::getOrCreate();
    if (!m_pScreenQuad)
    {
        LOGE("Failed to get CScreenQuad instance");
        return false;
    }
    
    m_pShaderProgram = CShaderProgram::createProgram("vertex.glsl", "fragment.glsl");
    if (!m_pShaderProgram)
    {
        LOGE("Failed to create shader program from files: vertex.glsl, fragment.glsl");
        return false;
    }
    
    int Width, Height;
    EPictureType::EPictureType PicType = EPictureType::ASTC;
    m_pTexture = CTexture2D::loadTexture("snowScene.astc", Width, Height, PicType);
    if (!m_pTexture)
    {
        LOGE("Failed to load texture: watercolor.png");
        // 清理已创建的资源
        if (m_pShaderProgram)
        {
            delete m_pShaderProgram;
            m_pShaderProgram = nullptr;
        }
        return false;
    }
    
    m_pShaderProgram->useProgram();
    m_pShaderProgram->setUniform("uTexture", 0);
    m_pTexture->bindTexture();

    m_bInitialized = true;
    LOGD("CImageExample init: 完成，使用CScreenQuad替代VAO/VBO");
    return true;
}

void CImageExample::draw()
{
    if (!m_bInitialized)
    {
        LOGE("CImageExample not initialized, cannot draw");
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    m_pShaderProgram->useProgram();
    glActiveTexture(GL_TEXTURE0);
    m_pTexture->bindTexture();
    m_pScreenQuad->bindAndDraw();
}

void CImageExample::destroy()
{
    if (m_pShaderProgram)
    {
        delete m_pShaderProgram;
        m_pShaderProgram = nullptr;
    }
    if (m_pTexture)
    {
        delete m_pTexture;
        m_pTexture = nullptr;
    }
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    m_bInitialized = false;
}

void CImageExample::reset()
{
    LOGD("CImageExample reset: 重新初始化...");
    destroy();
    init();
}
