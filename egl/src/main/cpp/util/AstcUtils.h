#pragma once

#ifndef OPENGLLIVEVIEW_ASTCUTILS_H
#define OPENGLLIVEVIEW_ASTCUTILS_H
#include <vector>
#include "Texture2D.h"

namespace hiveVG {

    class CAstcUtils {
    private:
        CAstcUtils(){}
        ~CAstcUtils(){}
    public:
        static bool DecodeToRGBA32(const std::vector<unsigned char>& vASTCData,int vWidth,int vHeight,std::vector<unsigned char>& oRGBAData);
        static bool ReadASTC(const std::string& vFilePath,std::vector<unsigned char>& oASTCData,int& oWidth,int& oHeight);
        static bool parseAstcHeader(const uint8_t *vData, size_t vSize, AstcHeaderInfo &oOut);
    };


}



#endif //OPENGLLIVEVIEW_ASTCUTILS_H
