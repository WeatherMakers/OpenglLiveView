#include "Texture2D.h"
#include <GLES2/gl2ext.h>
#include <omp.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Common.h"
#include "log.h"
#include "FileUtils.h"
#include "TimeUtils.h"
//#include "ktx.h"
#define ASTCHeaderMinSize 16
using namespace hiveVG;

CTexture2D *CTexture2D::loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType &vPictureType)
{
    std::unique_ptr<unsigned char[]> pBuffer;
    size_t AssetSize;
    bool IsReadFromAssetManager = true;
    auto pResource = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    if (!pResource)
    {
        IsReadFromAssetManager = false;
        pResource = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    }

    AssetSize = CFileUtils::getFileBytes(pResource, IsReadFromAssetManager);
    pBuffer = std::make_unique<unsigned char[]>(AssetSize);
    int Flag = CFileUtils::readFile<unsigned char>(pResource, pBuffer.get(), AssetSize, IsReadFromAssetManager);
    CFileUtils::closeFile(pResource, IsReadFromAssetManager);
    if (Flag < 0)
        return nullptr;

    double StartTime = CTimeUtils::getCurrentTime();
    int Channels;
    unsigned char *pImageData = nullptr;
    if (vPictureType == EPictureType::PNG || vPictureType == EPictureType::JPG)
    {
        pImageData = stbi_load_from_memory(pBuffer.get(), static_cast<int>(AssetSize), &voWidth, &voHeight, &Channels, 0);
    }
    else if(EPictureType::EPictureType::ASTC)
    {
        if (AssetSize <= ASTCHeaderMinSize) 
        {
            LOGE("Invalid ASTC file size: %{public}zu", AssetSize);
            return nullptr;
        }
        const unsigned char* pHeader = pBuffer.get();
        if (pHeader[0] != 0x13 || pHeader[1] != 0xAB || pHeader[2] != 0xA1 || pHeader[3] != 0x5C) {
            LOGE("Invalid ASTC file magic number");
            return nullptr;
        }
        
        unsigned int BlockX = pHeader[4];
        unsigned int BlockY = pHeader[5];
        unsigned int DimX = pHeader[7] | (pHeader[8] << 8) | (pHeader[9] << 16) ;
        unsigned int DimY = pHeader[10] | (pHeader[11] << 8) | (pHeader[12] << 16);
        LOGI("ASTC texture info - Block: %{public}dx%{public}d, Dimensions: %{public}dx%{public}d", BlockX, BlockY, DimX,DimY);
        
        GLuint TextureHandle;
        glGenTextures(1, &TextureHandle);
        glBindTexture(GL_TEXTURE_2D, TextureHandle);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        GLenum InternalFormat = 0;
        if (BlockX == 4 && BlockY == 4)        InternalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        else if (BlockX == 5 && BlockY == 4)   InternalFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
        else if (BlockX == 5 && BlockY == 5)   InternalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
        else if (BlockX == 6 && BlockY == 6)   InternalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
        else if (BlockX == 8 && BlockY == 8)   InternalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
        else if (BlockX == 10 && BlockY == 10) InternalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
        else if (BlockX == 12 && BlockY == 12) InternalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
        else 
        {
            LOGE("Unsupported ASTC block size: %{public}dx%{public}d", BlockX, BlockY);
            glDeleteTextures(1, &TextureHandle);
            return nullptr;
        }
        
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, 
                             DimX, DimY, 0, 
                             AssetSize - 16, 
                             pBuffer.get() + 16);
        
        GLenum Error = glGetError();
        if (Error != GL_NO_ERROR) {
            LOGE("Failed to upload ASTC texture, GL error: 0x%{public}x", Error);
            glDeleteTextures(1, &TextureHandle);
            return nullptr;
        }
        
        voWidth = DimX;
        voHeight = DimY;
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI("Successfully loaded ASTC texture. Dimensions: %{public}dx%{public}d, Time: %{public}f", DimX, DimY, EndTime - StartTime);
        return new CTexture2D(TextureHandle);
    }

    if (!pImageData)
    {
        LOGE( "Failed to load image from memory: %{public}s", vTexturePath.c_str());
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI( "Loading image %{public}s from memory to CPU costs time: %{public}f", vTexturePath.c_str(), EndTime - StartTime);
    }

    GLint Format = GL_RGB;
    if (Channels == 3)
        Format = GL_RGB;
    else if (Channels == 4)
        Format = GL_RGBA;
    else if (Channels == 1)
        Format = GL_RED;

    StartTime = CTimeUtils::getCurrentTime();

    GLuint TextureHandle = 0;
    TextureHandle = __createHandle(Format, voWidth, voHeight, pImageData);

    if (TextureHandle == 0)
    {
        LOGE( "Failed to create texture: %{public}s", vTexturePath.c_str());
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI( "Loading image %{public}s from memory to GPU costs time: %{public}f", vTexturePath.c_str(), EndTime - StartTime);
    }
    stbi_image_free(pImageData);
    return new CTexture2D(TextureHandle);
}

CTexture2D *CTexture2D::createEmptyTexture(int vWidth, int vHeight, int vChannels)
{
    GLint Format = GL_RGB;
    if (vChannels == 3)
        Format = GL_RGB;
    else if (vChannels == 4)
        Format = GL_RGBA;
    else if (vChannels == 1)
        Format = GL_RED;
    else
        LOGW( "Channel Count is invalid, set default format [GL_RGB].");

    double StartTime = CTimeUtils::getCurrentTime();

    GLuint TextureHandle = __createHandle(Format, vWidth, vHeight, nullptr);

    if (TextureHandle == 0)
    {
        LOGE( "Failed to create empty texture.");
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI( "Creating empty image costs time: %{public}f", EndTime - StartTime);
    }

    return new CTexture2D(TextureHandle);
}

CTexture2D::~CTexture2D()
{
    glDeleteTextures(1, &m_TextureHandle);
    m_TextureHandle = 0;
}

void CTexture2D::bindTexture() const
{
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
}

CTexture2D::CTexture2D(GLuint vTextureHandle) : m_TextureHandle(vTextureHandle) {}

GLuint CTexture2D::__createHandle(GLint vFormat, int vWidth, int vHeight, unsigned char *vImgData)
{
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, vFormat, vWidth, vHeight, 0, vFormat, GL_UNSIGNED_BYTE, vImgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    bool IsValid = (glIsTexture(TextureHandle) == GL_TRUE);
    if (!IsValid)
        return 0;
    else
        return TextureHandle;
}
