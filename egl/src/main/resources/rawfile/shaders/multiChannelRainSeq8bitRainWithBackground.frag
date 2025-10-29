#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D rainSequenceTexture;
uniform sampler2D backgroundTexture;
uniform int channelIndex;
uniform float SliderColor;
uniform vec3 backgroundColor;

const float rainAlphaThreshold = 0.01;    // 判断当前像素是否为雨所在的区域
const float rainIntensity = 0.5;

void main() {
    vec4 BackgroundColor = texture(backgroundTexture, TexCoord);
    float LightMask = 0.f;

    vec4 RainColor = texture(rainSequenceTexture, TexCoord);
    float FinalRainAlpha = RainColor.r + RainColor.g * float(channelIndex >= 1) + RainColor.b * float(channelIndex >= 2) + RainColor.a * float(channelIndex >= 3);

    vec4 SrcColor = vec4(vec3(1.0), FinalRainAlpha * rainIntensity + LightMask);

    vec4 DstColor = vec4(backgroundColor,0.f);
    DstColor.a = 1.0f;

    // 结合线性减淡方法的混合方式
    float BlendAlpha = SrcColor.a + DstColor.a - SrcColor.a * DstColor.a;
    // vec3 BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;
    // BlendColor += SrcColor.rgb * SrcColor.a * (1.0 - DstColor.a) + DstColor.rgb * DstColor.a * (1.0 - SrcColor.a);
    // BlendColor /= BlendAlpha;

    vec3 BlendColor = vec3(1.0f, 1.0f, 1.0f);
    if(FinalRainAlpha < rainAlphaThreshold) {
        BlendColor = DstColor.rgb;
    }
    else {
        BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;   // base blending color
        BlendColor *= SliderColor;
        BlendColor += SrcColor.rgb * SrcColor.a * SliderColor + DstColor.rgb * (1.0 - SrcColor.a);
    }

    FragColor = vec4(BlendColor, BlendAlpha);
}