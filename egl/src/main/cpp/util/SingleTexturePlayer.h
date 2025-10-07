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
        ~CSingleTexturePlayer();

        bool initTextureAndShaderProgram();
        bool initTextureAndShaderProgram(std::string& vVertexShaderPath, std::string& vFragShaderShaderPath);
        void updateFrame();

    private:
        std::string m_TexturePath;
        int    m_TextureCount       = 1;
        int	   m_SeqSingleTexWidth  = 0;
        int	   m_SeqSingleTexHeight = 0;
        EPictureType::EPictureType m_TextureType    = EPictureType::PNG;
        std::vector<CTexture2D*>   m_SeqTextures;
        CShaderProgram*            m_pSingleShaderProgram = nullptr;
    };
}