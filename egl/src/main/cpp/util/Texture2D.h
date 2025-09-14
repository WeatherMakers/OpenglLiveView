#pragma once

#include <string>
#include "Common.h"
#include "OpenGLCommon.h"

namespace hiveVG
{
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
        static GLuint __createHandle(GLint vFormat, int vWidth, int vHeight, unsigned char *vImgData);

        GLuint m_TextureHandle;
    };
}
