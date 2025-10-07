#pragma once

#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG 
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CCloudSceneRenderer : public CBaseRenderer
    {
    public:
        CCloudSceneRenderer();
        ~CCloudSceneRenderer() override;
        bool init() override;
        void draw() override;
        
    private:
        std::string           m_TextureRootPath   = "textures/rainScene/cloud_ASTC12x12_Linear";
        std::string           m_BackgroundTexPath = "textures/background.astc";
        int                   m_TextureCount      = 34;
        int                   m_OneTextureFrames  = 4;
        float                 m_FrameSeconds      = 15;
        EPictureType::EPictureType m_PictureType  = EPictureType::ASTC;
        CSequenceFramePlayer* m_pTexturePlayer    = nullptr;
        CSingleTexturePlayer* m_pBackGroundPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad       = nullptr;
    };
}
