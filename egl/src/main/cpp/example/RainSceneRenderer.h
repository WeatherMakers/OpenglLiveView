#pragma once

#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG 
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    
    class CRainSceneRenderer : public CBaseRenderer
    {
    public:
        CRainSceneRenderer();
        ~CRainSceneRenderer() override;
        bool init() override;
        void draw() override;
        
    private:
        double                m_LastFrameTime     = 0.0f;
        double                m_CurrentTime       = 0.0f;
        std::string           m_TextureRootPath   = "textures/rainScene/rain_ASTC12x12_Linear";
        std::string           m_BackgroundTexPath = "textures/background.astc";
        int                   m_TextureCount      = 128;
        int                   m_OneTextureFrames  = 4;
        float                 m_FrameSeconds      = 60;
        EPictureType::EPictureType m_PictureType  = EPictureType::ASTC;
        CSequenceFramePlayer* m_pTexturePlayer    = nullptr;
        CSingleTexturePlayer* m_pBackGroundPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad       = nullptr;
    };
}
