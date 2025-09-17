#pragma once

#include "BaseRenderer.h"
#include <string>

namespace hiveVG 
{
    class CSingleTexturePlayer;
    class CScreenQuad;

    class CSinglePlayerRenderer : public CBaseRenderer
    {
    public:
        CSinglePlayerRenderer();
        ~CSinglePlayerRenderer() override;
        bool init() override;
        void draw() override;
        
    private:
        std::string           m_TexturePath    = "snowScene.astc";
        CSingleTexturePlayer* m_pTexturePlayer = nullptr;
        CScreenQuad*          m_pScreenQuad    = nullptr;
    };
}

