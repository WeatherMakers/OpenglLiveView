#include "LuminCalculator.h"
#include "FileUtils.h"

using namespace hiveVG;

float CLuminCalculator::calculateRGBLumin(float vR, float vG, float vB){
    return vR*0.299f + vG*0.587f + vB*0.114f;
}

float CLuminCalculator::calculateImageAverageLumin(const std::vector<unsigned char>& vRGBAData,const unsigned char (&vBackgroundColor)[3]){
    int PixelCount = vRGBAData.size()/4;
    float Luminance = 0.0f;
    auto Blend = [](float vX,float vY,float vA){
        return vX*vA + vY*(1.0f - vA);
    };
    float BackgroundR = static_cast<float>(vBackgroundColor[0])/255.0f;
    float BackgroundG = static_cast<float>(vBackgroundColor[1])/255.0f;
    float BackgroundB = static_cast<float>(vBackgroundColor[2])/255.0f;
    for(int CurrPixelIndex = 0;CurrPixelIndex<PixelCount;CurrPixelIndex += 4){
        float A = static_cast<float>(vRGBAData[CurrPixelIndex + 3])/255.0f;
        float R = static_cast<float>(vRGBAData[CurrPixelIndex])/255.0f;
        float G = static_cast<float>(vRGBAData[CurrPixelIndex + 1])/255.0f;
        float B = static_cast<float>(vRGBAData[CurrPixelIndex + 2])/255.0f;
        R = Blend(R, BackgroundR, A);
        G = Blend(G, BackgroundG, A);
        B = Blend(B, BackgroundB, A);
        Luminance += calculateRGBLumin(R,G,B);
    }
    return Luminance/PixelCount;
}
