#pragma once

#include "example/BaseExample.h"
#include "SequenceFramePlayer.h"
#include "SinglePlayerExample.h"
#include "ScreenQuad.h"

namespace hiveVG 
{
    class CSeqPlayerExample : public BaseExample
    {
    public:
        CSeqPlayerExample();
        virtual ~CSeqPlayerExample();
        virtual bool init();
        virtual void draw();
        
    private:
        std::string           m_TextureRootPath = "textures/rain_splash_4channel_ASTC12x12_Linear";
        std::string           m_BackgroundTexPath = "snowScene.astc";
        int                   m_TextureCount    = 64;
        int                   m_OneTextureFrames = 4;
        float                 m_FrameSeconds = 60;
        EPictureType::EPictureType m_PictureType = EPictureType::ASTC;
        CSequenceFramePlayer* m_pTexturePlayer = nullptr;
        CSingleTexturePlayer* m_pBackGroundPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad    = nullptr;
    };
}
