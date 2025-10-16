#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;

    class CSequenceFramePlayer
    {
    public:
        CSequenceFramePlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        CSequenceFramePlayer(const std::string &vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType);
        ~CSequenceFramePlayer();

        void setFrameRate(int vFrameRate) { m_FramePerSecond = static_cast<float>(vFrameRate); }
        void setFrameRate(float vFrameRate) { m_FramePerSecond = vFrameRate; }
        void setWindowSize(glm::vec2 vWindowSize) { m_WindowSize = vWindowSize; }

        bool initTextureAndShaderProgram(const std::string &vVertexShaderPath = "", const std::string &vFragShaderShaderPath = "");

        void updateSeqFrame(double vDeltaTime);
        void drawSeqFrame(CScreenQuad *vQuad);

        void updateMultiChannelFrame(double vDeltaTime);
        void updateMultiChannelFrame(double vDeltaTime, ERenderChannel vRenderChannel);
        void drawMultiChannelFrame(CScreenQuad *vQuad);

        void updateCloudLerpMultiChannelFrame(double vDeltaTime);
        void drawCloudLerpMultiChannelFrame(CScreenQuad *vQuad);
        void setRatioUniform();
        void setColor(float vValue) { m_uniformColorValue = vValue; }
    

    protected:
        int    m_SequenceRows       = 1;
        int    m_SequenceCols       = 1;
        int    m_OneTextureFrames   = 1;
        int    m_CurrentChannel     = 0;
        int    m_SequenceWidth      = 0;
        int    m_SequenceHeight     = 0;
        int    m_SeqSingleTexWidth  = 0;
        int    m_SeqSingleTexHeight = 0;
        int    m_ValidFrames;
        float  m_FramePerSecond   = 24.0f;
        bool   m_IsFinished       = false;
        float  m_RotationAngle    = 0.0f;
        int    m_CurrentFrame     = 0;
        int    m_NextFrame        = 0;
        float  m_InterpolationFactor = 0.0f;
        double m_AccumFrameTime      = 0.0f;
        float  m_uniformColorValue = 1.0f;
        std::string m_TextureRootPath;
        int         m_CurrentTexture  = 0;
        int         m_NextTexture     = 0;
        int         m_TextureCount;
        glm::vec2 m_ScreenUVScale  = glm::vec2(0.6f, 0.8f);
        glm::vec2 m_ScreenUVOffset = glm::vec2(0.0f, 0.0f);
        glm::vec2 m_WindowSize     = glm::vec2(0.0f, 0.0f);
        EPictureType::EPictureType m_TextureType = EPictureType::PNG;
        std::vector<CTexture2D*>   m_SeqTextures;
        CShaderProgram*            m_pSequenceShaderProgram = nullptr;
    };
}
