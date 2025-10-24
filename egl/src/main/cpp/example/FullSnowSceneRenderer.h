#pragma once

#include "BaseRenderer.h"
#include "Common.h"
#include <glm/glm.hpp>
#include "JsonReader.h"

namespace hiveVG
{
    class CScreenQuad;
    class CSequenceFramePlayer;
    class CThickCloudSequencePlayer;
    class CRainWithBackgroundSeqPlayer;
    class CSingleTexturePlayer;

    class CFullSnowSceneRenderer : public CBaseRenderer
    {
    public:
        CFullSnowSceneRenderer();
        ~CFullSnowSceneRenderer() override;
        bool init() override;
        void draw() override;

        void setWindowSize(glm::vec2 vWindowSize) { m_WindowSize = vWindowSize; }
        void setChannel(ERenderChannel vChannel);
        void toggleSnowBackground();
        void toggleSnowForeground();

    private:
        template<typename T>
        void __deleteSafely(T*& vPointer);

        void __initBackgroundPlayer();
        void __initSnowBackgroundPlayer();
        void __initSnowForegroundPlayer();

        std::string                m_ConfigFile = "configs/FullSnowSceneConfig.json";
        CJsonReader*               m_pConfigReader = nullptr;

        ERenderChannel             m_RenderChannel        = ERenderChannel::R;
        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        double                     m_AccumFrameTime       = 0.0f;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        glm::vec2                  m_WindowSize           = glm::vec2 (0,0);
    
        CSequenceFramePlayer*      m_pSnowBackgroundPlayer = nullptr;
        CSequenceFramePlayer*      m_pSnowForegroundPlayer = nullptr;
        CSingleTexturePlayer*      m_pBackgroundPlayer = nullptr;
    
        bool                       m_SnowBackgroundInitialized = false;
        bool                       m_SnowForegroundInitialized = false;
        bool                       m_SnowBackgroundVisible     = false;
        bool                       m_SnowForegroundVisible     = false;
    };

    template<typename T>
    void CFullSnowSceneRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}
