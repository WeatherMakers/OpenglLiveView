#pragma once

#include <random>
#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CTexture2D;

    class CThickCloudSequencePlayer : public CSequenceFramePlayer
    {
    public:
        CThickCloudSequencePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        CThickCloudSequencePlayer(const std::string& vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType);
        ~CThickCloudSequencePlayer();
    
        void updateQuantizationFrame(double vDeltaTime);
        void draw(CScreenQuad *vQuad);
        void setWindowSize(glm::vec2 vWindowSize){m_WindowSize = vWindowSize;}
    private:
        void __initCloudTextures(const std::string& vCloudPath, int vFrameCount, EPictureType::EPictureType vCloudPicType);

        std::string m_ConfigFileName  = "configs/FullRainSceneConfig.json";

        glm::vec2 m_WindowSize        = glm::vec2 (0,0);
        int     m_CurrentCloudTexture = 0;
        int     m_NextCloudTexture    = 0;
        int     m_CurrentCloudChannel = 0;
        float   m_CloudInterpFactor   = 0.0f;
        double  m_AccumCloudTime      = 0.0;
        int     m_OneCloudTexFrames   = 4;
        float   m_CloudFPS            = 10.0f;
        std::vector<CTexture2D*> m_SeqCloudTextures;
        int     m_CloudSingleTexWidth  = 0;
        int     m_CloudSingleTexHeight = 0;
    };
}