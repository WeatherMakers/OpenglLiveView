#pragma once

#include "BaseRenderer.h"
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;
    class CSequenceFramePlayer;
    class CThickCloudSequencePlayer;
    class CRainWithBackgroundSeqPlayer;
    class CJsonReader;

    class CFullRainSceneRenderer : public CBaseRenderer
    {
    public:
        CFullRainSceneRenderer();

        ~CFullRainSceneRenderer() override;
        bool init() override;
        void draw() override;

        void setWindowSize(glm::vec2 vWindowSize) { m_WindowSize = vWindowSize; }
        void setChannel(ERenderChannel vChannel);
        void toggleCloud();
 
    private:
        template<typename T>
        void __deleteSafely(T*& vPointer);

        void __initRainSeqPlayer();
        void __initCloudPlayer();
        void __initThickCloudPlayer();

        std::string                m_ConfigFile           = "configs/FullRainSceneConfig.json";
        CJsonReader*               m_pConfigReader        = nullptr;
        ERenderChannel             m_RenderChannel        = ERenderChannel::R;
        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        double                     m_AccumFrameTime       = 0.0f;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        glm::vec2                  m_WindowSize           = glm::vec2 (0,0);

        CThickCloudSequencePlayer*  m_pThickCloudPlayer     = nullptr;
        CSequenceFramePlayer*      m_pCloudPlayer         = nullptr;
        CRainWithBackgroundSeqPlayer*      m_pRainSeqPlayer       = nullptr;
    
        bool                       m_ThickCloudInitialized     = false;
        bool                       m_CloudInitialized         = false;
        bool                       m_CloudVisible             = false;
        bool                       m_ThickCloudVisible         = false;
    };

    template<typename T>
    void CFullRainSceneRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}