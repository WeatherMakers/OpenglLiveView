#pragma once

#include <mutex>
#include "OpenGLCommon.h"

namespace hiveVG
{
    class CScreenQuad
    {
    public:
        static CScreenQuad* getOrCreate();
        static void destroy();

        ~CScreenQuad();
        void bindAndDraw() const;

    private:
        CScreenQuad();
        CScreenQuad(const CScreenQuad&) = delete;
        CScreenQuad& operator=(const CScreenQuad&) = delete;

        static CScreenQuad* m_pQuad;
        GLuint m_VAOHandle = 0;
        GLuint m_VertexBufferHandle = 0;
        GLuint m_IndexBufferHandle  = 0;
    };
}
