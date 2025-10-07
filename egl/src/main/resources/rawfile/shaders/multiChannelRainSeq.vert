#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 TexCoordCloud;
uniform vec2  cloudUVScale;   // 图像缩放控制（越大越大）
uniform vec2  cloudUVOffset;  // 图像在屏幕上的偏移控制
void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
    vec2 centerCoord = aTexCoord - vec2(0.5); // 平移到中心
    vec2 safeCloudScale = max(cloudUVScale, vec2(0.0001)); // 防止除以 0
    vec2 scaledCloudCoord = centerCoord / safeCloudScale;
    TexCoordCloud = scaledCloudCoord + vec2(0.5) + cloudUVOffset;

}