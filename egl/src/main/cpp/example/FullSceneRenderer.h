#pragma once

#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;
    class CSequenceFramePlayer;
    class CLightningSequencePlayer;
    class CNightSceneSequencePlayer;
    class CJsonReader;

    class CFullSceneRenderer : public CBaseRenderer
    {
    public:
        CFullSceneRenderer();

        ~CFullSceneRenderer() override;
        bool init() override;
        void draw() override;

        void setWindowSize(glm::vec2 vWindowSize) { m_WindowSize = vWindowSize; }
        void setChannel(ERenderChannel vChannel);
        void toggleCloud();
        void toggleLightning();

    private:
        template<typename T>
        void __deleteSafely(T*& vPointer);

        void __initRainSeqPlayer();
        void __initSmallRainDropPlayer();
        void __initBigRainDropPlayer();
        void __initCloudPlayer();
        void __initLightningPlayer();

        std::string                m_ConfigFile           = "configs/RainMultiChannelSeqConfig.json";
        CJsonReader*               m_pConfigReader        = nullptr;
        ERenderChannel             m_RenderChannel        = ERenderChannel::R;
        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        double                     m_AccumFrameTime       = 0.0f;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        glm::vec2                  m_WindowSize           = glm::vec2 (0,0);

        CLightningSequencePlayer*  m_pLightningPlayer     = nullptr;
        CSequenceFramePlayer*      m_pCloudPlayer         = nullptr;
        CSequenceFramePlayer*      m_pSmallRaindropPlayer = nullptr;
        CSequenceFramePlayer*      m_pBigRaindropPlayer   = nullptr;
        CNightSceneSequencePlayer* m_pRainSeqPlayer       = nullptr;

        bool                       m_SmallRainDropInitialized = false;    
        bool                       m_BigRainDropInitialized   = false;    
        bool                       m_LightningInitialized     = false;
        bool                       m_CloudInitialized         = false;
        bool                       m_CloudVisible             = false;
        bool                       m_LightningVisible         = false;
    };

    template<typename T>
    void CFullSceneRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}