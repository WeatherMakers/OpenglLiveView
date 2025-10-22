#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D rainSequenceTexture;
uniform sampler2D backgroundTexture;
uniform int channelIndex;
uniform float SliderColor;

const float rainAlphaThreshold = 0.01;    // 判断当前像素是否为雨所在的区域
const float rainIntensity = 0.5;

void main()
{
    vec4 BackgroundColor = texture(backgroundTexture, vec2(TexCoord.x,1.0f - TexCoord.y));
    float LightMask = 0.0f;


void main() {
    vec4 BackgroundColor = texture(backgroundTexture, TexCoord);
    float LightMask = BackgroundColor.a;

    vec4 RainColor = texture(rainSequenceTexture, TexCoord);
    float FinalRainColor = RainColor.r
                         + RainColor.g * float(channelIndex >= 1)
                         + RainColor.b * float(channelIndex >= 2)
                         + RainColor.a * float(channelIndex >= 3);

    vec4 SrcColor = vec4(vec3(1.0), FinalRainAlpha * rainIntensity + LightMask);

    vec4 DstColor = BackgroundColor;
    DstColor.a = 1.0;

    float BlendAlpha = SrcColor.a + DstColor.a - SrcColor.a * DstColor.a;

    vec3 BlendColor = min(SrcColor.rgb + DstColor.rgb, vec3(1.0, 1.0, 1.0)) * SrcColor.a * DstColor.a;
    BlendColor = BlendColor + SrcColor.rgb * SrcColor.a * (1.0 - DstColor.a) + DstColor.rgb * DstColor.a * (1.0 - SrcColor.a);
    BlendColor = BlendColor / BlendAlpha;
    FragColor = vec4(BlendColor, BlendAlpha);
}