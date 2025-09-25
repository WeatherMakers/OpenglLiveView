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
std::vector<std::future<std::pair<std::unique_ptr<unsigned char[]>, size_t>>> CTexture2D::m_AsyncFutures;
std::vector<std::string> CTexture2D::m_AsyncPaths;

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

CTexture2D *CTexture2D::loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight,
                                    EPictureType::EPictureType &vPictureType)
{
    auto future = std::async(std::launch::async, [&vTexturePath]() -> std::pair<std::unique_ptr<unsigned char[]>, size_t> {
        bool IsReadFromAssetManager = true;
        auto pResource = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
        if (!pResource)
        {
            IsReadFromAssetManager = false;
            pResource = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
        }
        if (!pResource) return {nullptr, 0};
    
        size_t AssetSize = CFileUtils::getFileBytes(pResource, IsReadFromAssetManager);
        auto pBuffer = std::make_unique<unsigned char[]>(AssetSize);
        int Flag = CFileUtils::readFile<unsigned char>(pResource, pBuffer.get(), AssetSize, IsReadFromAssetManager);
        CFileUtils::closeFile(pResource, IsReadFromAssetManager);
        if (Flag < 0) return {nullptr, 0};
        return {std::move(pBuffer), AssetSize};
    });
    auto [pBuffer, AssetSize] = future.get();
    if (!pBuffer || AssetSize == 0) 
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to read file: %{public}s", vTexturePath.c_str());
        return nullptr;
    }

    double StartTime = CTimeUtils::getCurrentTime();
    if (vPictureType == EPictureType::PNG || vPictureType == EPictureType::JPG)
    {
        int Channels;
        unsigned char *pImageData = stbi_load_from_memory(pBuffer.get(), static_cast<int>(AssetSize), &voWidth, &voHeight, &Channels, 0);
        if (!pImageData)
        {
            LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to load image from memory: %{public}s", vTexturePath.c_str());
            return nullptr;
        } 
        else
        {
            double EndTime = CTimeUtils::getCurrentTime();
            LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %{public}s from memory to CPU costs time: %{public}f", vTexturePath.c_str(),
                 EndTime - StartTime);
        }

        GLint Format = (Channels == 3) ? GL_RGB : (Channels == 4) ? GL_RGBA : GL_RED;
        
        StartTime = CTimeUtils::getCurrentTime();
        GLuint TextureHandle = 0;
        TextureHandle = __createPngHandle(Format, voWidth, voHeight, pImageData);

        if (TextureHandle == 0)
        {
            LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create texture: %{public}s", vTexturePath.c_str());
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
    else if (EPictureType::EPictureType::ASTC)
    {
        if (AssetSize <= AstcHeaderSize)
        {
            LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ASTC file size: %{public}zu", AssetSize);
            return nullptr;
        }
        const unsigned char *pHeader = pBuffer.get();
        AstcHeaderInfo Info;
        std::lock_guard<std::mutex> Lock(m_CacheMutex);
        auto it = m_AstcCache.find(vTexturePath);
        if (it != m_AstcCache.end()) 
        {
            Info = it->second;
//            LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Using cached ASTC header for %{public}s", vTexturePath.c_str());
        } 
        else 
        {
            if (!parseAstcHeader(reinterpret_cast<const uint8_t *>(pBuffer.get()), AssetSize, Info))
            {
                return nullptr;
            }
            m_AstcCache[vTexturePath] = Info;
            if (m_AstcCache.size() > 128) 
            {
                m_AstcCache.erase(m_AstcCache.begin());
            }
//           LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "ASTC info - Block: %{public}ux%{public}u, Size: %{public}ux%{public}u", Info._BlockX, Info._BlockY, Info._DimX, Info._DimY);
        }
        GLuint TextureHandle = __createAstcHandle(Info, pHeader, AssetSize);

        voWidth  = Info._DimX;
        voHeight = Info._DimY;
        double EndTime = CTimeUtils::getCurrentTime();
//        LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Successfully loaded ASTC texture. Dimensions: %{public}dx%{public}d, Time: %{public}f", voWidth, voHeight, EndTime - StartTime);
        return new CTexture2D(TextureHandle);
    }
}

// 新增：批量加载 128 张 ASTC
std::vector<CTexture2D*> CTexture2D::loadTexturesBatch(const std::vector<std::string>& vTexturePaths, bool IsReadFromAssetManager)
{
    std::vector<CTexture2D*> Textures;
    Textures.reserve(vTexturePaths.size());

    std::vector<std::future<std::pair<std::unique_ptr<unsigned char[]>, size_t>>> Futures;
    Futures.reserve(vTexturePaths.size());

    double StartBatch = CTimeUtils::getCurrentTime();
    for (const auto& path : vTexturePaths) 
    {
        Futures.emplace_back(std::async(std::launch::async, [&path, IsReadFromAssetManager]() -> std::pair<std::unique_ptr<unsigned char[]>, size_t>
        {
            bool IsAsset = IsReadFromAssetManager;
            auto* pResource = CFileUtils::openFile(path.c_str(), IsAsset);
            if (!pResource) return {nullptr, 0};
            size_t size = CFileUtils::getFileBytes(pResource, IsAsset);
            auto pBuffer = std::make_unique<unsigned char[]>(size);
            int flag = CFileUtils::readFile<unsigned char>(pResource, pBuffer.get(), size, IsAsset);
            CFileUtils::closeFile(pResource, IsAsset);
            if (flag < 0) return {nullptr, 0};
            return {std::move(pBuffer), size};
        }));
    }
    // 等待所有 I/O 完成并上传纹理（在渲染线程）
    for (size_t i = 0; i < vTexturePaths.size(); ++i)
    {
        auto [pBuffer, AssetSize] = Futures[i].get();
        if (!pBuffer || AssetSize <= AstcHeaderSize) 
        {
            LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to load ASTC: %s", vTexturePaths[i].c_str());
            Textures.push_back(nullptr);
            continue;
        }
    }

    double EndBatch = CTimeUtils::getCurrentTime();
    LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Batch loaded %{public}zu ASTC textures in %{public}f seconds", vTexturePaths.size(), EndBatch - StartBatch);
    return Textures;
}

void CTexture2D::startLoadingBatch(const std::vector<std::string>& vTexturePaths, bool IsReadFromAssetManager)
{
    // 清理之前的任务状态
    m_AsyncFutures.clear();
    m_AsyncPaths.clear();

    m_AsyncFutures.reserve(vTexturePaths.size());
    m_AsyncPaths = vTexturePaths; // 直接拷贝路径

    LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Starting async batch load for %zu textures.", vTexturePaths.size());

    // 启动所有后台文件读取任务
    for (const auto& path : vTexturePaths) 
    {
        m_AsyncFutures.emplace_back(std::async(std::launch::async, [path, IsReadFromAssetManager]() -> std::pair<std::unique_ptr<unsigned char[]>, size_t>
        {
            // 这个 lambda 的内容和您原来的一样，只负责读文件
            bool IsAsset = IsReadFromAssetManager;
            auto* pResource = CFileUtils::openFile(path.c_str(), IsAsset);
            if (!pResource) return {nullptr, 0};
            
            size_t size = CFileUtils::getFileBytes(pResource, IsAsset);
            auto pBuffer = std::make_unique<unsigned char[]>(size);
            int flag = CFileUtils::readFile<unsigned char>(pResource, pBuffer.get(), size, IsAsset);
            CFileUtils::closeFile(pResource, IsAsset);

            if (flag < 0) return {nullptr, 0};
            return {std::move(pBuffer), size};
        }));
    }
}

bool CTexture2D::finalizeLoadingBatch(std::vector<CTexture2D*>& out_LoadedTextures, int vMaxToFinalize)
{
    // 清空输出列表
    out_LoadedTextures.clear();
    int finalizedCount = 0;

    // 从后往前遍历，这样可以安全地删除已处理的元素
    for (int i = m_AsyncFutures.size() - 1; i >= 0; --i)
    {
        auto& future = m_AsyncFutures[i];

        // 检查 future 是否已就绪，超时设为0表示不等待
        if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            // --- 获取后台线程的成果 ---
            auto [pBuffer, AssetSize] = future.get();
            const std::string& path = m_AsyncPaths[i];

            CTexture2D* newTexture = nullptr;
            if (pBuffer && AssetSize > AstcHeaderSize)
            {
                // --- 在主线程执行 OpenGL 操作 ---
                AstcHeaderInfo Info;
                if (parseAstcHeader(pBuffer.get(), AssetSize, Info)) 
                {
                    GLuint textureHandle = __createAstcHandle(Info, pBuffer.get(), AssetSize);
                    if (textureHandle != 0) 
                    {
                        newTexture = new CTexture2D(textureHandle);
                    }
                }
            }
            
            if (!newTexture)
            {
                LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to finalize ASTC texture: %s", path.c_str());
            }

            out_LoadedTextures.push_back(newTexture);

            // 从任务列表中移除已处理的项
            m_AsyncFutures.erase(m_AsyncFutures.begin() + i);
            m_AsyncPaths.erase(m_AsyncPaths.begin() + i);
            
            finalizedCount++;
            if (vMaxToFinalize > 0 && finalizedCount >= vMaxToFinalize)
            {
                break; // 达到本帧处理上限
            }
        }
    }

    // 如果任务列表不为空，则返回 true
    return !m_AsyncFutures.empty();
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
        LOGW(TAG_KEYWORD::TEXTURE2D_TAG, "Channel Count is invalid, set default format [GL_RGB].");

    double StartTime = CTimeUtils::getCurrentTime();

    GLuint TextureHandle = __createPngHandle(Format, vWidth, vHeight, nullptr);

    if (TextureHandle == 0)
    {
        LOGE(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create empty texture.");
        return nullptr;
    } else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOGI(TAG_KEYWORD::TEXTURE2D_TAG, "Creating empty image costs time: %{public}f", EndTime - StartTime);
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