#version 300 es
precision mediump float;
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D quadTexture;

void main()
{
    vec2 FinalTexCoord = TexCoord;
    FinalTexCoord.y = 1.0 - FinalTexCoord.y;
    vec4 QuadColor = texture(quadTexture, FinalTexCoord);
    FragColor = QuadColor;
}