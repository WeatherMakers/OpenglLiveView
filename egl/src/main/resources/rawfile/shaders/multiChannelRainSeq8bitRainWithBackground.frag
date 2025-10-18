#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D rainSequenceTexture;
uniform sampler2D backgroundTexture;
uniform int channelIndex;
uniform float SliderColor;

const float rainEdgeThreshold = 0.1;    // 判断当前像素是否为雨所在的区域
const float baseRainAlpha = 0.2;        // 所有雨的基础透明度
const float epsilon = 0.015f;
const float imageBackgroundAlphaThreshold = 0.0045f;
const vec3 imageBackgroundColor = vec3(61.f/255.f);

bool isEqual(vec3 vLeft,vec3 vRight,float vEpsilon){
    vec3 Delta = abs(vLeft - vRight);
    return all(lessThan(Delta,vec3(vEpsilon)));
}

bool isTransparentColor(vec3 vColor){
    vec3 TransparentColor = vec3(imageBackgroundColor);
    if(isEqual(vColor,TransparentColor,epsilon)){
        return true;
    }
    return false;
}

void main() {
    vec4 BackgroundColor = texture(backgroundTexture, vec2(TexCoord.x,TexCoord.y));
    float LightMask = BackgroundColor.a;

    vec4 RainColor = texture(rainSequenceTexture, TexCoord);
    float FinalRainColor = RainColor.r + RainColor.g * float(channelIndex >= 1) + RainColor.b * float(channelIndex >= 2) + RainColor.a * float(channelIndex >= 3);

    vec4 SrcColor = vec4(vec3(1.0f), mix(baseRainAlpha,baseRainAlpha + FinalRainColor,LightMask +  SliderColor));

    vec4 DstColor = BackgroundColor;
    float BlendAlpha = 0.f;
    // 无灯光时，判断颜色是不是背景色
    if(DstColor.a < imageBackgroundAlphaThreshold){
        DstColor.a = isTransparentColor(BackgroundColor.rgb) ? 0.0f : 1.0f;
    }else{
        // 有灯光时，把原始背景色去除，得到原始的灯光颜色
        // 假设是与背景色混合
        // ImgColor.rgb = (Src.rgb * Src.a + Dst.rgb * (1 - Src.a));
        // ImgColor.a = Src.a;
        // Src.rgb = (ImgColor.rgb * Src.a - Dst.rgb * (1 - Src.a)) / Src.a;
        DstColor.rgb =  (DstColor.rgb - imageBackgroundColor * (1.f -  DstColor.a)) / DstColor.a;
    }

    // 结合线性减淡方法的混合方式
    BlendAlpha = SrcColor.a + DstColor.a - SrcColor.a * DstColor.a;
    // vec3 BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;
    // BlendColor += SrcColor.rgb * SrcColor.a * (1.0 - DstColor.a) + DstColor.rgb * DstColor.a * (1.0 - SrcColor.a);
    // BlendColor /= BlendAlpha;
    
    vec3 BlendColor = vec3(1.0f, 1.0f, 1.0f);
    BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;   // base blending color
       // BlendColor *= SliderColor;
    BlendColor = SrcColor.rgb * SrcColor.a  + DstColor.rgb * (1.0 - SrcColor.a);
   /* if(FinalRainColor < rainEdgeThreshold) {
        BlendColor = DstColor.rgb + SrcColor.rgb * SrcColor.a;// * SliderColor;    // 可能还有很小的值的雨，也加上
    }
    else {
        BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;   // base blending color
       // BlendColor *= SliderColor;
        BlendColor += SrcColor.rgb * SrcColor.a  + DstColor.rgb * (1.0 - SrcColor.a);
    }*/

    FragColor = vec4(BlendColor, BlendAlpha);
}