#pragma once

#include "AsycSequenceFramePlayer.h"
#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG 
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    class CAsycSequenceFramePlayer;
    
    class CRainSceneRenderer : public CBaseRenderer
    {
    public:
        CRainSceneRenderer();
        ~CRainSceneRenderer() override;
        bool init() override;
        void draw() override;
        
    private:
        std::string           m_TextureRootPath   = "textures/rain_splash_4channel_ASTC12x12_Linear";
        std::string           m_BackgroundTexPath = "snowScene.astc";
        int                   m_TextureCount      = 128;
        int                   m_OneTextureFrames  = 4;
        float                 m_FrameSeconds      = 60;
        EPictureType::EPictureType m_PictureType  = EPictureType::ASTC;
        CSequenceFramePlayer* m_pTexturePlayer    = nullptr;
        CAsycSequenceFramePlayer* m_pAsycTexturePlayer = nullptr;
        CSingleTexturePlayer* m_pBackGroundPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad       = nullptr;
    };
}
