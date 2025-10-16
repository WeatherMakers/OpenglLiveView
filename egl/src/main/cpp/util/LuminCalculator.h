//
// Created on 2025/10/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGLLIVEVIEW_LUMINCALCULATOR_H
#define OPENGLLIVEVIEW_LUMINCALCULATOR_H
#include <string>

namespace hiveVG{
    
    class CLuminCalculator {
    public:
        static float calculateRGBLumin(float vR, float vG, float vB);
        static float calculateImageAverageLumin(const std::vector<unsigned char>& vRGBAData,const unsigned char (&vBackgroundColor)[3]);
    
    };

}


#endif //OPENGLLIVEVIEW_LUMINCALCULATOR_H
