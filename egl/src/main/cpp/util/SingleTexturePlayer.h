#pragma once

#include <string>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;

    class CSingleTexturePlayer
    {
    public:
        CSingleTexturePlayer(const std::string& vTexturePath, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        CSingleTexturePlayer(const std::string& vTexturePath, int vSequenceRows, int vSequenceCols,int vOneTextureFrames, EPictureType::EPictureType vPictureType, float vFrameSeconds, int vTextureCount);
        ~CSingleTexturePlayer();

        bool initTextureAndShaderProgram();
        bool initTextureAndShaderProgram(std::string& vVertexShaderPath, std::string& vFragShaderShaderPath);
        void updateFrame();
        void drawCompressedFrame(CScreenQuad *vQuad);
        void updateCompressedFrame(EPlayMode::EPlayMode vPlayMode, double vDeltaTime);

    private:
        std::string m_TexturePath;
        int    m_SeqRows            = 0;
        int    m_SeqCols            = 0;
        int    m_CurrentFrame       = 0;
        double m_AccumFrameTime     = 0.0f;
        float  m_FramePerSecond     = 60.0f;
        int    m_OneTextureFrames   = 1;
        int    m_CurrentChannel     = 0;
        int    m_TextureCount       = 1;
        int    m_ValidFrames        = 0;
        int    m_CurrentTexture     = 0;
        int	   m_SeqSingleTexWidth  = 0;
        int	   m_SeqSingleTexHeight = 0;
        EPictureType::EPictureType m_TextureType    = EPictureType::PNG;
        std::vector<CTexture2D*>   m_SeqTextures;
        CShaderProgram*            m_pSingleShaderProgram = nullptr;
    };
}