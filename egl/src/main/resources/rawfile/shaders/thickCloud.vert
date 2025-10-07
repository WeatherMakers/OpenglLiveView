#version 300 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform vec2  cloudUVScale;   // 图像缩放控制（越大越大）
uniform vec2  cloudUVOffset;  // 图像在屏幕上的偏移控制
out vec2 TexCoordCloud;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0); // 直接传入标准化设备坐标 [-1,1]

    // 闪电纹理变换处理（绕中心缩放+旋转+偏移）
    vec2 centerCoord = aTexCoord - vec2(0.5); // 平移到中心
    vec2 safeCloudScale = max(cloudUVScale, vec2(0.0001)); // 防止除以 0
    vec2 scaledCloudCoord = centerCoord / safeCloudScale;


    TexCoordCloud = scaledCloudCoord + vec2(0.5) + cloudUVOffset;
}