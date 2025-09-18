#pragma once

#include <string>
#include "Common.h"
#include "OpenGLCommon.h"

namespace hiveVG
{
    struct AstcHeaderInfo 
    {
        uint32_t _BlockX = 0;
        uint32_t _BlockY = 0;
        uint32_t _DimX   = 0;
        uint32_t _DimY   = 0;
    };

    class CTexture2D
    {
    public:
        static CTexture2D *loadTexture(const std::string &vTexturePath, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        static CTexture2D *loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType &vPictureType);
        static CTexture2D *createEmptyTexture(int vWidth, int vHeight, int vChannels);
        ~CTexture2D();
        [[nodiscard]] constexpr GLuint getTextureHandle() const { return m_TextureHandle; }
        void bindTexture() const;

    private:
        inline explicit CTexture2D(GLuint vTextureHandle);
        static GLuint __createPngHandle(GLint vFormat, int vWidth, int vHeight, unsigned char *vImgData);
        static GLuint __createAstcHandle(AstcHeaderInfo& vHeaderInfo, const unsigned char *vHeaderData, size_t vAssetSize);

        GLuint m_TextureHandle;
    };
}
