#pragma once

#include "BaseRenderer.h"
#include "Common.h"

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
        std::string                m_TexturePath    = "snowScene.astc";
        EPictureType::EPictureType m_TextureType    = EPictureType::ASTC;
        CSingleTexturePlayer*      m_pTexturePlayer = nullptr;
        CScreenQuad*               m_pScreenQuad    = nullptr;
    };
}

