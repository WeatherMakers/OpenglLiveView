#pragma once

#include "example/BaseExample.h"
#include <rawfile/raw_file_manager.h>
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
        virtual void destroy();
        void reset(); // 重置函数，用于重新初始化
        
    private:
        CShaderProgram* m_pShaderProgram;
        CTexture2D*     m_pTexture;
        CScreenQuad*    m_pScreenQuad;
        bool            m_bInitialized; // 初始化状态标志
    };
}
