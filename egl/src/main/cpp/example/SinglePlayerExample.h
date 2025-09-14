#pragma once

#include "example/BaseExample.h"
#include "SingleTexturePlayer.h"
#include "ScreenQuad.h"

namespace hiveVG 
{
    class CSinglePlayerExample : public BaseExample
    {
    public:
        CSinglePlayerExample();
        virtual ~CSinglePlayerExample();
        virtual bool init();
        virtual void draw();
        
    private:
        CSingleTexturePlayer* m_pTexturePlayer = nullptr;
        CScreenQuad*          m_pScreenQuad    = nullptr;
    };
}

