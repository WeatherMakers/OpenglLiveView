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

    class CFullSceneRenderer : public CBaseRenderer
    {
    public:
        CFullSceneRenderer();

        ~CFullSceneRenderer() override;
        bool init() override;
        void draw() override;

        void setRenderChannel(ERenderChannel vRenderChannel) { m_RenderChannel = vRenderChannel; }
        void setWindowSize(glm::vec2 vWindowSize) { m_WindowSize = vWindowSize; }

    private:
        template<typename T>
        void __deleteSafely(T*& vPointer);

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