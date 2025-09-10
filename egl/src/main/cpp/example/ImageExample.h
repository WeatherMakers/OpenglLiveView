#pragma once

#include "example/BaseExample.h"
#include <rawfile/raw_file.h>
#include <rawfile/raw_file_manager.h>

class CImageExample : public BaseExample
{
protected:
    // 顶点（X, Y, Z, U, V）
    GLfloat quadVertices[20] = {
        // 位置        // 纹理坐标
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // 左上
    };

    GLuint quadIndices[6] = {
        0, 1, 2, // 第一个三角形
        0, 2, 3  // 第二个三角形
    };

    GLuint VBO, VAO, EBO;
    GLuint textureID;

public:
    CImageExample();
    virtual ~CImageExample();
    virtual bool init();
    virtual void draw();
    virtual void destroy();
    void setImageFromMemory(unsigned char* bytes, int length);
    bool loadFromRawfile(const char* filename, const NativeResourceManager* rm);
    bool loadFromNativeFile(const char* filepath);
};
