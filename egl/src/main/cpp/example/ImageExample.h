#pragma once

#include "example/BaseExample.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "ScreenQuad.h"

namespace hiveVG 
{
    class CImageExample : public BaseExample
    {
    public:
        CImageExample();
        virtual ~CImageExample();
        virtual bool init();
        virtual void draw();
        
    private:
        CShaderProgram* m_pShaderProgram = nullptr;
        CTexture2D*     m_pTexture       = nullptr;
        CScreenQuad*    m_pScreenQuad    = nullptr;
    };
}
