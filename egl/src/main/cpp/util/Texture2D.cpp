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
//        ktxTexture2 *pTexture = nullptr;
//        KTX_error_code Result = ktxTexture2_CreateFromMemory(
//            pBuffer.get(),
//            AssetSize,
//            KTX_TEXTURE_CREATE_NO_FLAGS,
//            &pTexture);
//
//        if (Result != KTX_SUCCESS)
//        {
//            LOGE(
//                      "Failed to load KTX2 pTexture from memory. Error code: %{public}d", Result);
//            return nullptr;
//        }
//        if (ktxTexture_NeedsTranscoding(ktxTexture(pTexture)))
//        {
//            Result = ktxTexture2_TranscodeBasis(pTexture, KTX_TTF_ETC2_RGBA, 0);
//            if (Result != KTX_SUCCESS)
//            {
//                LOGE(
//                          "Failed to transcode KTX2 pTexture. Error code: %{public}d", Result);
//                ktxTexture_Destroy(ktxTexture(pTexture));
//                return nullptr;
//            }
//        }
//
//        GLuint TextureHandle = 0;
//        GLenum Target = 0;
//        GLenum GlError = GL_NO_ERROR;
//
//        KTX_error_code GlUploadResult = ktxTexture_GLUpload(
//            reinterpret_cast<ktxTexture *>(pTexture),
//            &TextureHandle,
//            &Target,
//            &GlError);
//
//        glBindTexture(GL_TEXTURE_2D, TextureHandle);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        if (GlUploadResult != KTX_SUCCESS || GlError != GL_NO_ERROR)
//        {
//            LOGE("ktxTexture_GLUpload failed. Error: %{public}d, GL Error: 0x%{public}x", GlUploadResult, GlError);
//            ktxTexture_Destroy(reinterpret_cast<ktxTexture *>(pTexture));
//            return nullptr;
//        }
//
//        voWidth = pTexture->baseWidth;
//        voHeight = pTexture->baseHeight;
//
//        LOGI("Successfully loaded [%{public}s] KTX pTexture. Width: %{public}d, Height: %{public}d, Target: 0x%{public}x",
//                 vTexturePath.c_str(), voWidth, voHeight, Target);
//
//        ktxTexture_Destroy(reinterpret_cast<ktxTexture *>(pTexture));
//        return new CTexture2D(TextureHandle);
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
