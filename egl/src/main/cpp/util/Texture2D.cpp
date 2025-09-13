#include "Texture2D.h"
#include <omp.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Common.h"
#include "log.h"
#include "FileUtils.h"
#include "TimeUtils.h"
//#include "ktx.h"

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
    else if (vPictureType == EPictureType::KTX2)
    {
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
