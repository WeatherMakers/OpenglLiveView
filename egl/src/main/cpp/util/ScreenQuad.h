#pragma once

#include <mutex>
#include "OpenGLCommon.h"

namespace hiveVG
{
    class CScreenQuad
    {
    public:
        static CScreenQuad& getInstance();
        ~CScreenQuad();
    
        bool init();
        void bindAndDraw();

    private:
        CScreenQuad();
        CScreenQuad(const CScreenQuad&) = delete;
        CScreenQuad& operator=(const CScreenQuad&) = delete;

        bool   m_initialized = false;
        GLuint m_VAOHandle = 0;
        GLuint m_VertexBufferHandle = 0;
        GLuint m_IndexBufferHandle  = 0;
    };
}