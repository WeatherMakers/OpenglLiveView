#pragma once

#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG 
{
    class CScreenQuad;
    class CSequenceFramePlayer;
    class CJsonReader;

    class CSnowSceneRenderer : public CBaseRenderer
    {
    public:
        CSnowSceneRenderer();
        ~CSnowSceneRenderer() override;
        bool init() override;
        void draw() override;
        
    private:
        void __initSnowBackgroundPlayer();
        void __initBackgroundPlayer();
        void __initSnowForegroundPlayer();

        CSequenceFramePlayer* m_pSnowBackgroundPlayer = nullptr;
        CSequenceFramePlayer* m_pBackgroundPlayer     = nullptr;
        CSequenceFramePlayer* m_pSnowForegroundPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad           = nullptr;
    
        std::string           m_ConfigFile        = "configs/SnowSceneConfig.json";
        CJsonReader*          m_pConfigReader     = nullptr;
        EPictureType::EPictureType m_PictureType  = EPictureType::ASTC;

        ERenderChannel        m_CurrentSnowChannel = ERenderChannel::R;
        double                m_LastFrameTime      = 0.0;
        double                m_CurrentTime        = 0.0;
        double                m_AccumTime          = 0.0;
    };
}
