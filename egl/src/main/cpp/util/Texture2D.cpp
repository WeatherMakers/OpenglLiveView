#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "FileUtils.h"
#include "TimeUtils.h"
#include "log.h"
#include "stb_image.h"

constexpr uint8_t AstcMagic[4] = {0x13, 0xAB, 0xA1, 0x5C};
constexpr size_t AstcHeaderSize = 16;

using namespace hiveVG;

CTexture2D *CTexture2D::loadTexture(const std::string &vTexturePath, EPictureType::EPictureType vPicType)
{
    int Width, Height;
    auto pTexture = loadTexture(vTexturePath, Width, Height, vPicType);
    return pTexture;
}

static inline uint32_t readU24LE(const uint8_t *p)
{
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) | (static_cast<uint32_t>(p[2]) << 16);
}

static inline GLenum getAstcInternalFormat(int vBlockX, int vBlockY)
{
    struct Entry
    {
        int _X, _Y;
        GLenum _Fmt;
    };
    static constexpr Entry kAstcFormats[] = {
        {4, 4, GL_COMPRESSED_RGBA_ASTC_4x4_KHR},     {5, 4, GL_COMPRESSED_RGBA_ASTC_5x4_KHR},
        {5, 5, GL_COMPRESSED_RGBA_ASTC_5x5_KHR},     {6, 6, GL_COMPRESSED_RGBA_ASTC_6x6_KHR},
        {8, 8, GL_COMPRESSED_RGBA_ASTC_8x8_KHR},     {10, 10, GL_COMPRESSED_RGBA_ASTC_10x10_KHR},
        {12, 12, GL_COMPRESSED_RGBA_ASTC_12x12_KHR},
    };
    for (const auto &e : kAstcFormats)
    {
        if (e._X == vBlockX && e._Y == vBlockY)
            return e._Fmt;
    }
    return 0; // 0 表示不支持
}

static inline bool parseAstcHeader(const uint8_t *vData, size_t vSize, AstcHeaderInfo &oOut)
{
    if (vData == nullptr || vSize < AstcHeaderSize)
    {
        LOGE("ASTC file too small or null, size=%{public}zu", vSize);
        return false;
    }
    if (memcmp(vData, AstcMagic, 4) != 0)
    {
        LOGE("Invalid ASTC magic");
        return false;
    }
    oOut._BlockX = vData[4];
    oOut._BlockY = vData[5];
    oOut._DimX = readU24LE(vData + 7);
    oOut._DimY = readU24LE(vData + 10);
    if (oOut._BlockX == 0 || oOut._BlockY == 0 || oOut._DimX == 0 || oOut._DimY == 0)
    {
        LOGE("ASTC header contains zero dimension/block");
        return false;
    }
    return true;
}

CTexture2D *CTexture2D::loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight,
                                    EPictureType::EPictureType &vPictureType)
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
    if (vPictureType == EPictureType::PNG || vPictureType == EPictureType::JPG)
    {
        int Channels;
        unsigned char *pImageData = stbi_load_from_memory(pBuffer.get(), static_cast<int>(AssetSize), &voWidth, &voHeight, &Channels, 0);
        if (!pImageData)
        {
            LOGE("Failed to load image from memory: %{public}s", vTexturePath.c_str());
            return nullptr;
        } 
        else
        {
            double EndTime = CTimeUtils::getCurrentTime();
            LOGI("Loading image %{public}s from memory to CPU costs time: %{public}f", vTexturePath.c_str(),
                 EndTime - StartTime);
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
        TextureHandle = __createPngHandle(Format, voWidth, voHeight, pImageData);

        if (TextureHandle == 0)
        {
            LOGE("Failed to create texture: %{public}s", vTexturePath.c_str());
            return nullptr;
        }
        else
        {
            double EndTime = CTimeUtils::getCurrentTime();
            LOGI("Loading image %{public}s from memory to GPU costs time: %{public}f", vTexturePath.c_str(), EndTime - StartTime);
        }
        stbi_image_free(pImageData);
        return new CTexture2D(TextureHandle);
    } 
    else if (EPictureType::EPictureType::ASTC)
    {
        if (AssetSize <= AstcHeaderSize)
        {
            LOGE("Invalid ASTC file size: %{public}zu", AssetSize);
            return nullptr;
        }
        const unsigned char *pHeader = pBuffer.get();
        AstcHeaderInfo Info;
        if (!parseAstcHeader(reinterpret_cast<const uint8_t *>(pBuffer.get()), AssetSize, Info))
        {
            return nullptr;
        }
        LOGI("ASTC info - Block: %{public}ux%{public}u, Size: %{public}ux%{public}u", Info._BlockX, Info._BlockY, Info._DimX, Info._DimY);

        GLuint TextureHandle = __createAstcHandle(Info, pHeader, AssetSize);

        voWidth = Info._DimX;
        voHeight = Info._DimY;
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI("Successfully loaded ASTC texture. Dimensions: %{public}dx%{public}d, Time: %{public}f", voWidth, voHeight,
             EndTime - StartTime);
        return new CTexture2D(TextureHandle);
    }
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
        LOGW("Channel Count is invalid, set default format [GL_RGB].");

    double StartTime = CTimeUtils::getCurrentTime();

    GLuint TextureHandle = __createPngHandle(Format, vWidth, vHeight, nullptr);

    if (TextureHandle == 0)
    {
        LOGE("Failed to create empty texture.");
        return nullptr;
    } else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI("Creating empty image costs time: %{public}f", EndTime - StartTime);
    }

    return new CTexture2D(TextureHandle);
}

CTexture2D::~CTexture2D()
{
    glDeleteTextures(1, &m_TextureHandle);
    m_TextureHandle = 0;
}

void CTexture2D::bindTexture() const { glBindTexture(GL_TEXTURE_2D, m_TextureHandle); }

CTexture2D::CTexture2D(GLuint vTextureHandle) : m_TextureHandle(vTextureHandle) {}

GLuint CTexture2D::__createPngHandle(GLint vFormat, int vWidth, int vHeight, unsigned char *vImgData)
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

GLuint CTexture2D::__createAstcHandle(AstcHeaderInfo& vHeaderInfo, const unsigned char *vHeaderData, size_t vAssetSize)
{
    if (eglGetCurrentContext() == EGL_NO_CONTEXT)
    {
        LOGE("No valid EGL context");
        return 0;
    }

    GLuint TextureHandle = 0;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum InternalFormat = getAstcInternalFormat(vHeaderInfo._BlockX, vHeaderInfo._BlockY);
    if (InternalFormat == 0)
    {
        LOGE("Unsupported ASTC block size: %{public}ux%{public}u", vHeaderInfo._BlockX, vHeaderInfo._BlockY);
        glDeleteTextures(1, &TextureHandle);
        return 0;
    }
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, vHeaderInfo._DimX, vHeaderInfo._DimY, 0, vAssetSize - AstcHeaderSize,
                           vHeaderData + AstcHeaderSize);

    GLenum Error = glGetError();
    if (Error != GL_NO_ERROR)
    {
        LOGE("Failed to upload ASTC texture, GL error: 0x%{public}x", Error);
        glDeleteTextures(1, &TextureHandle);
        return 0;
    }
    return TextureHandle;
}