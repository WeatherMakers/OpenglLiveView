#pragma once

#include <map>
#include <future>
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
        static std::vector<CTexture2D*> loadTexturesBatch(const std::vector<std::string>& vTexturePaths, bool IsReadFromAssetManager = true);
        static void startLoadingBatch(const std::vector<std::string>& vTexturePaths, bool IsReadFromAssetManager = true);
        static bool finalizeLoadingBatch(std::vector<CTexture2D*>& out_LoadedTextures, int vMaxToFinalize = 0);
        static CTexture2D *createEmptyTexture(int vWidth, int vHeight, int vChannels);
        ~CTexture2D();
        [[nodiscard]] constexpr GLuint getTextureHandle() const { return m_TextureHandle; }
        void bindTexture() const;

    private:
        inline explicit CTexture2D(GLuint vTextureHandle);
        static GLuint __createPngHandle(GLint vFormat, int vWidth, int vHeight, unsigned char *vImgData);
        static GLuint __createAstcHandle(AstcHeaderInfo& vHeaderInfo, const unsigned char *vHeaderData, size_t vAssetSize);

        GLuint m_TextureHandle;
        static std::map<std::string, AstcHeaderInfo> m_AstcCache;
        static std::mutex                            m_CacheMutex;
        static std::vector<std::future<std::pair<std::unique_ptr<unsigned char[]>, size_t>>> m_AsyncFutures;
        // 用于存储与 future 对应的原始路径
        static std::vector<std::string> m_AsyncPaths;
    
    };
}
