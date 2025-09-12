//
// Created on 2024/9/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_EBORECTANGLE_H
#define OPENGL_EBORECTANGLE_H
#include "example/BaseExample.h"
#define VBO_COUNT 1
#define EBO_COUNT 1

namespace hiveVG {

/**
 * 使用EBO绘制矩形
 */
class EBORectangleExample : public BaseExample
{
private:
    GLuint eboIds[1] = {};
    GLuint vboIds[1] = {};
    // 顶点
    float rectangleVertices[12] = {
        0.5f,  0.5f,  0.0f, // 右上角
        0.5f,  -0.5f, 0.0f, // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f,  0.0f  // 左上角
    };
    // 顶点索引
    int indices[6] = {
        // 注意索引从0开始!
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

public:
    EBORectangleExample();
    ~EBORectangleExample();
    virtual bool init();
    virtual void draw();
    virtual void destroy();
};

} // namespace hiveVG

#endif // OPENGL_EBORECTANGLE_H
