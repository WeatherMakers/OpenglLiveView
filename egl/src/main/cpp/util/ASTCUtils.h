//
// Created on 2025/10/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGLLIVEVIEW_ASTCCODEC_H
#define OPENGLLIVEVIEW_ASTCCODEC_H
#include <vector>
#include "Texture2D.h"

namespace hiveVG {

    class CASTCUtil {
    public:
        static bool decodeToRGBA(const std::vector<unsigned char>& vASTCData,int vWidth,int vHeight,std::vector<unsigned char>& oRGBAData);
        static bool readASTC(const std::string& vFilePath,std::vector<unsigned char>& oASTCData,int& oWidth,int& oHeight);
        static bool parseAstcHeader(const uint8_t *vData, size_t vSize, AstcHeaderInfo &oOut);
    };


}



#endif //OPENGLLIVEVIEW_ASTCCODEC_H
