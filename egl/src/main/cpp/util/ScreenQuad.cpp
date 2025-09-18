#include "ScreenQuad.h"
#include "log.h"

using namespace hiveVG;

CScreenQuad &CScreenQuad::getInstance()
{
    static CScreenQuad instance;
    return instance;
}

CScreenQuad::~CScreenQuad()
{
    glDeleteBuffers(1, &m_VertexBufferHandle);
    m_VertexBufferHandle = 0;
    glDeleteBuffers(1, &m_IndexBufferHandle);
    m_IndexBufferHandle = 0;
    glDeleteVertexArrays(1, &m_VAOHandle);
    m_VAOHandle = 0;
    LOGI("CScreenQuad destroyed.");
}

void CScreenQuad::bindAndDraw()
{
    if (!m_initialized && !init())
    {
        LOGE("CScreenQuad init failed in bindAndDraw");
        return;
    }
    glBindVertexArray(m_VAOHandle);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

CScreenQuad::CScreenQuad() {}

bool CScreenQuad::init()
{
    if (m_initialized)
        return true;
    if (!eglGetCurrentContext())
    {
        LOGE("No valid OpenGL context in CScreenQuad init");
        return false;
    }
    constexpr float Vertices[] = {-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f,  1.0f, 0.0f,
                                  1.0f,  -1.0, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f};
    const unsigned int Indices[] = {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &m_VAOHandle);
    glBindVertexArray(m_VAOHandle);

    glGenBuffers(1, &m_VertexBufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_IndexBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    m_initialized = true;
    LOGI("CScreenQuad initialized successfully.");
    return true;
}