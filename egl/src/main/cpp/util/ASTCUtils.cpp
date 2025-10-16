#include "ASTCUtils.h"
#include <astc-codec/astc-codec.h>
#include "FileUtils.h"
#include "Texture2D.h"
using namespace hiveVG;


bool CASTCUtil::decodeToRGBA(const std::vector<unsigned char>& vASTCData,int vWidth,int vHeight,std::vector<unsigned char>& oRGBAData){
    
    oRGBAData.resize(vWidth * vHeight * 4);
    constexpr size_t AstcHeaderSize = 16;
    bool Success = astc_codec::ASTCDecompressToRGBA(vASTCData.data() + AstcHeaderSize, vASTCData.size() - AstcHeaderSize,
                                                    vWidth, vHeight, astc_codec::FootprintType::k12x12,
        oRGBAData.data(), oRGBAData.size(), /* stride */ vWidth * 4);
    return Success;
}

bool CASTCUtil::readASTC(const std::string &vFilePath, std::vector<unsigned char> &oASTCData, int &oWidth, int &oHeight){
    size_t Size;
    std::unique_ptr<unsigned char[]> pBuffer;
    if(!CFileUtils::readFileToBuffer(vFilePath, pBuffer, Size)){
        return false;
    }
    AstcHeaderInfo HeaderInfo;
    if (!parseAstcHeader(pBuffer.get(), Size, HeaderInfo)){
        return false;
    }
    oWidth = HeaderInfo._DimX;
    oHeight = HeaderInfo._DimY;
    oASTCData = std::vector<unsigned char>(pBuffer.get(),pBuffer.get() + Size);
    return true;
}

static inline uint32_t readU24LE(const uint8_t *p)
{
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) | (static_cast<uint32_t>(p[2]) << 16);
}

bool CASTCUtil::parseAstcHeader(const uint8_t *vData, size_t vSize, AstcHeaderInfo &oOut)
{
    constexpr uint8_t AstcMagic[4] = {0x13, 0xAB, 0xA1, 0x5C};
    constexpr size_t AstcHeaderSize = 16;
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
    
