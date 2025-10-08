#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "FileUtils.h"
#include "TimeUtils.h"
#include "log.h"
#include "stb_image.h"
#include <mutex>   // 新增：线程安全

constexpr uint8_t AstcMagic[4] = {0x13, 0xAB, 0xA1, 0x5C};
constexpr size_t AstcHeaderSize = 16;

using namespace hiveVG;

std::map<std::string, AstcHeaderInfo> CTexture2D::m_AstcCache{};
std::mutex                            CTexture2D::m_CacheMutex;

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
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "ASTC file too small or null, size=%{public}zu", vSize);
        return false;
    }
    if (memcmp(vData, AstcMagic, 4) != 0)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ASTC magic");
        return false;
    }
    oOut._BlockX = vData[4];
    oOut._BlockY = vData[5];
    oOut._DimX = readU24LE(vData + 7);
    oOut._DimY = readU24LE(vData + 10);
    if (oOut._BlockX == 0 || oOut._BlockY == 0 || oOut._DimX == 0 || oOut._DimY == 0)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "ASTC header contains zero dimension/block");
        return false;
    }
    return true;
}

bool CTexture2D::__readFileToBuffer(const std::string &vTexturePath, std::unique_ptr<unsigned char[]> &oBuffer, size_t &oSize)
{
    bool IsReadFromAssetManager = true;
    auto pResource = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    if (!pResource)
    {
        IsReadFromAssetManager = false;
        pResource = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    }
    if (!pResource)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to read file: %{public}s", vTexturePath.c_str());
        return false;
    }

    oSize = CFileUtils::getFileBytes(pResource, IsReadFromAssetManager);
    oBuffer = std::make_unique<unsigned char[]>(oSize);
    int Flag = CFileUtils::readFile<unsigned char>(pResource, oBuffer.get(), oSize, IsReadFromAssetManager);
    CFileUtils::closeFile(pResource, IsReadFromAssetManager);
    if (Flag < 0 || oSize == 0)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to read file: %{public}s", vTexturePath.c_str());
        return false;
    }
    return true;
}

CTexture2D *CTexture2D::__loadPngOrJpgFromMemory(const std::string &vTexturePath, const unsigned char *pBuffer, size_t BufferSize, int &voWidth, int &voHeight)
{
    double StartTime = CTimeUtils::getCurrentTime();
    int Channels;
    unsigned char *pImageData = stbi_load_from_memory(pBuffer, static_cast<int>(BufferSize), &voWidth, &voHeight, &Channels, 0);
    if (!pImageData)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to load image from memory: %{public}s", vTexturePath.c_str());
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %{public}s from memory to CPU costs time: %{public}f", vTexturePath.c_str(), EndTime - StartTime);
    }

    GLint Format = (Channels == 3) ? GL_RGB : (Channels == 4) ? GL_RGBA : GL_RED;

    StartTime = CTimeUtils::getCurrentTime();
    GLuint TextureHandle = __createPngHandle(Format, voWidth, voHeight, pImageData);

    if (TextureHandle == 0)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create texture: %{public}s", vTexturePath.c_str());
        stbi_image_free(pImageData);
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %{public}s from memory to GPU costs time: %{public}f", vTexturePath.c_str(), EndTime - StartTime);
    }
    stbi_image_free(pImageData);
    return new CTexture2D(TextureHandle);
}

CTexture2D *CTexture2D::__loadAstcFromMemory(const std::string &vTexturePath, const unsigned char *pBuffer, size_t BufferSize, int &voWidth, int &voHeight)
{
    double StartTime = CTimeUtils::getCurrentTime();
    if (BufferSize <= AstcHeaderSize)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ASTC file size: %{public}zu", BufferSize);
        return nullptr;
    }
    const unsigned char *pHeader = pBuffer;
    AstcHeaderInfo Info;
    std::lock_guard<std::mutex> Lock(m_CacheMutex);
    auto it = m_AstcCache.find(vTexturePath);
    if (it != m_AstcCache.end()) 
    {
        Info = it->second;
    } 
    else 
    {
        if (!parseAstcHeader(reinterpret_cast<const uint8_t *>(pBuffer), BufferSize, Info))
        {
            return nullptr;
        }
        m_AstcCache[vTexturePath] = Info;
        if (m_AstcCache.size() > 128) 
        {
            m_AstcCache.erase(m_AstcCache.begin());
        }
    }
    GLuint TextureHandle = __createAstcHandle(Info, pHeader, BufferSize);

    voWidth  = Info._DimX;
    voHeight = Info._DimY;
    double EndTime = CTimeUtils::getCurrentTime();
    LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %{public}s from memory to GPU costs time: %{public}f", vTexturePath.c_str(), EndTime - StartTime);
    return new CTexture2D(TextureHandle);
}

CTexture2D *CTexture2D::loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight,
                                    EPictureType::EPictureType &vPictureType)
{
    std::unique_ptr<unsigned char[]> pBuffer;
    size_t BufferSize = 0;
    if (!__readFileToBuffer(vTexturePath, pBuffer, BufferSize))
    {
        return nullptr;
    }

    if (vPictureType == EPictureType::PNG || vPictureType == EPictureType::JPG)
    {
        return __loadPngOrJpgFromMemory(vTexturePath, pBuffer.get(), BufferSize, voWidth, voHeight);
    }
    else if (vPictureType == EPictureType::ASTC)
    {
        return __loadAstcFromMemory(vTexturePath, pBuffer.get(), BufferSize, voWidth, voHeight);
    }
    return nullptr;
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
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "No valid EGL context");
        return 0;
    }

    GLuint TextureHandle = 0;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 新增：优化上传对齐
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLenum InternalFormat = getAstcInternalFormat(vHeaderInfo._BlockX, vHeaderInfo._BlockY);
    if (InternalFormat == 0)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Unsupported ASTC block size: %{public}ux%{public}u", vHeaderInfo._BlockX, vHeaderInfo._BlockY);
        glDeleteTextures(1, &TextureHandle);
        return 0;
    }
    glCompressedTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, vHeaderInfo._DimX, vHeaderInfo._DimY, 0, vAssetSize - AstcHeaderSize,
                           vHeaderData + AstcHeaderSize);

    GLenum Error = glGetError();
    if (Error != GL_NO_ERROR)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to upload ASTC texture, GL error: 0x%{public}x", Error);
        glDeleteTextures(1, &TextureHandle);
        return 0;
    }
    return TextureHandle;
}