#version 300 es
precision mediump float;
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    vec2 finalTexCoord = TexCoord;
    finalTexCoord.y = 1.0 - finalTexCoord.y;
    FragColor = texture(uTexture, finalTexCoord);
}