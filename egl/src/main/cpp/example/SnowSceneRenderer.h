#pragma once

#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG 
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CSnowSceneRenderer : public CBaseRenderer
    {
    public:
        CSnowSceneRenderer();
        ~CSnowSceneRenderer() override;
        bool init() override;
        void draw() override;
        
    private:
        std::string           m_TextureRootPath   = "textures/lightning_ASTC12x12_Linear";
        std::string           m_BackgroundTexPath = "snowScene.astc";
        int                   m_TextureCount      = 16;
        int                   m_OneTextureFrames  = 4;
        float                 m_FrameSeconds      = 16;
        EPictureType::EPictureType m_PictureType  = EPictureType::ASTC;
        CSequenceFramePlayer* m_pTexturePlayer    = nullptr;
        CSingleTexturePlayer* m_pBackGroundPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad       = nullptr;
    };
}