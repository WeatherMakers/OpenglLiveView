#version 300 es
precision mediump float;

in vec2 TexCoordCloud;
in vec2 TexCoordLightning;

uniform float Factor;
uniform float Displacement;
uniform int   CurrentChannel;
uniform vec2  TexelSize;
uniform float FlashProgress;
uniform vec3  FlashColor;
uniform float FlashAlpha;
uniform bool  LightningInFront;
uniform bool  isFinish;
uniform int   ChannelIndex;
uniform float CloudThickness;

uniform sampler2D CurrentTexture;
uniform sampler2D NextTexture;
uniform sampler2D LightningSequenceTexture;

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
    float Transmittance = exp(-CloudThickness);
    vec3 CloudAlbedo = vec3(1.0f) * Transmittance;
    vec4 CloudColor = vec4( CloudAlbedo , MixColor);

    vec3 CloudColorWithoutLight = remap(CloudColor.rgb, 0.0, 1.0, 0.0, 0.65);
    vec4 LightningColor = texture(LightningSequenceTexture, TexCoordLightning);
    float LightningMask = LightningColor[ChannelIndex];

    // 云后效果
    CloudColor.rgb = CloudColorWithoutLight;
    vec4 ColorWhenBehind = CloudColor;

    // 闪电提亮
    float EnableFlash = step(0.0001, FlashProgress); // FlashProgress > 0 => 1.0，否则 0.0
    float UP   = smoothstep(0.0, 0.5, FlashProgress);
    float Down = 1.0 - smoothstep(0.5, 1.0, FlashProgress);
    float FlashIntensity = UP * Down;

    vec4 ColorWhenInFront = vec4(CloudColor.rgb, MixColor + LightningMask);
    FragColor = mix(ColorWhenBehind, ColorWhenInFront, float(LightningInFront) * EnableFlash);

    //当闪电播放在云前的时候 变成全屏
    if ((TexCoordCloud.x < 0.0 || TexCoordCloud.x > 1.0 ||
        TexCoordCloud.y < 0.0 || TexCoordCloud.y > 1.0) && !LightningInFront ){
        FragColor = vec4(0.0);
    }else if((TexCoordCloud.x < 0.0 || TexCoordCloud.x > 1.0 ||
                     TexCoordCloud.y < 0.0 || TexCoordCloud.y > 1.0) && isFinish){
        FragColor = vec4(0.0);
    }
}