#pragma once

#ifndef OPENGLLIVEVIEW_IMAGEUTILS_H
#define OPENGLLIVEVIEW_IMAGEUTILS_H
#include <vector>

namespace hiveVG{
    
    class CImageUtils {
    public:
        static float CalculateRGBLumin(float vR, float vG, float vB);
        static float CalculateImageAverageLumin(const std::vector<unsigned char>& vRGBAData,const unsigned char (&vBackgroundColor)[3],int& oOpaquePixelCount);
     
    };

}


#endif //OPENGLLIVEVIEW_IMAGEUTILS_H
