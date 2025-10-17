#include "ImageUtils.h"
#include "FileUtils.h"

using namespace hiveVG;

float CImageUtils::CalculateRGBLumin(float vR, float vG, float vB){
    return vR*0.299f + vG*0.587f + vB*0.114f;
}

float CImageUtils::CalculateImageAverageLumin(const std::vector<unsigned char>& vRGBAData,const unsigned char (&vBackgroundColor)[3],int& oOpaquePixelCount){
    int PixelCount = vRGBAData.size()/4;
    float Luminance = 0.0f;
    auto Blend = [](float vX,float vY,float vA){
        return vX*vA + vY*(1.0f - vA);
    };
    oOpaquePixelCount = 0;
    float BackgroundR = static_cast<float>(vBackgroundColor[0])/255.0f;
    float BackgroundG = static_cast<float>(vBackgroundColor[1])/255.0f;
    float BackgroundB = static_cast<float>(vBackgroundColor[2])/255.0f;
    for(int CurrPixelIndex = 0;CurrPixelIndex<PixelCount;CurrPixelIndex++){
        float R = static_cast<float>(vRGBAData[4*CurrPixelIndex])/255.0f;
        float G = static_cast<float>(vRGBAData[4*CurrPixelIndex + 1])/255.0f;
        float B = static_cast<float>(vRGBAData[4*CurrPixelIndex + 2])/255.0f;
        float A = static_cast<float>(vRGBAData[4*CurrPixelIndex + 3])/255.0f;
        if(A > 0.01f){
            oOpaquePixelCount++;
        }
        R = Blend(R, BackgroundR, A);
        G = Blend(G, BackgroundG, A);
        B = Blend(B, BackgroundB, A);
        Luminance += CalculateRGBLumin(R,G,B);
    }
    return Luminance/PixelCount;
}

