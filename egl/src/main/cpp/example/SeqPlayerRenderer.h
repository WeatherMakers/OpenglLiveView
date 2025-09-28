#pragma once

#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG 
{
    class CScreenQuad;
    class CSequenceFramePlayer;

    class CSeqPlayerRenderer : public CBaseRenderer
    {
    public:
        CSeqPlayerRenderer();
        ~CSeqPlayerRenderer() override;
        bool init() override;
        void draw() override;
        
    private:
        std::string     m_TextureRootPath  = "textures/rainScene/rain_splash_4channel_ASTC12x12_Linear";
        int             m_TextureCount     = 64;
        int             m_OneTextureFrames = 4;
        float           m_FrameSeconds     = 60;
        EPictureType::EPictureType m_PictureType    = EPictureType::ASTC;
        CSequenceFramePlayer*      m_pTexturePlayer = nullptr;
        CScreenQuad*               m_pScreenQuad    = nullptr;
    };
}
