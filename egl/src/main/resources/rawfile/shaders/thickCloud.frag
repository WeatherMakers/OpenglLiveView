#version 300 es
precision mediump float;

in vec2 TexCoordCloud;

uniform float Factor;
uniform float Displacement;
uniform int   CurrentChannel;
uniform vec2  TexelSize;

uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;


out vec4 FragColor;

float filteredChannelSpace3x3(sampler2D vTex, vec2 vUV, int vChannelIndex)
{
    float Sum = 0.0;
    float Count = 0.0;
    float FilterSpacingFactor = 3.0;

    for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy)
        {
            vec2 SampleUV = vUV + vec2(float(dx), float(dy)) * TexelSize * FilterSpacingFactor;
            vec4 SampleColor = texture(vTex, SampleUV);
            Sum += SampleColor[vChannelIndex];
            Count += 1.0;
        }
    return Sum / Count;
}

vec3 remap(vec3 vVector, float vOldMin, float vOldMax, float vNewMin, float vNewMax)
{
    return (vVector - vOldMin) / (vOldMax - vOldMin) * (vNewMax - vNewMin) + vNewMin;
}

float remap(float vData, float vOldMin, float vOldMax, float vNewMin, float vNewMax)
{
    return (vData - vOldMin) / (vOldMax - vOldMin) * (vNewMax - vNewMin) + vNewMin;
}

void main()
{
    float CurrentOffset = TexCoordCloud.x - Displacement * Factor;
    vec2  CurrentUV     = vec2(CurrentOffset, TexCoordCloud.y);
    float NextOffset    = TexCoordCloud.x + Displacement * (1.0 - Factor);
    vec2  NextUV        = vec2(NextOffset,   TexCoordCloud.y);
    int   NextChannel   = (CurrentChannel + 1) % 4;
    float CurrentSpaceFilterColor = filteredChannelSpace3x3(CurrentTexture, CurrentUV, CurrentChannel);
    float NextSpaceFilterColor    = filteredChannelSpace3x3(NextTexture,    NextUV,    NextChannel);
    float MixColor = mix(CurrentSpaceFilterColor, NextSpaceFilterColor, Factor);

    vec4 CloudColor = vec4(1.0, 1.0, 1.0, MixColor);

    vec3 CloudColorWithoutLight = remap(CloudColor.rgb, 0.0, 1.0, 0.0, 0.65);
    FragColor =  vec4(CloudColorWithoutLight,MixColor);
}