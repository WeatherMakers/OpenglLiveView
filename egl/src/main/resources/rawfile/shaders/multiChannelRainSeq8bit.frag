#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D indexTexture;
uniform int channelIndex;
uniform float SliderColor;

void main()
{
    vec4 indexColor = texture(indexTexture, TexCoord);
    float alpha = indexColor.r + indexColor.g * float(channelIndex >= 1) + indexColor.b * float(channelIndex >= 2) + indexColor.a * float(channelIndex >= 3);
    float transmittance = exp(SliderColor - 1.f);
    FragColor = vec4(vec3(1.0f), alpha*transmittance);
}