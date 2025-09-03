//
// Created on 2024/8/31.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_TRIANGLEEXAMPLE_H
#define OPENGL_TRIANGLEEXAMPLE_H

#include "example/BaseExample.h"
#include "GLES3/gl3.h"
/**
 * 绘制三角形
 */
class TriangleExample: public BaseExample {
    
protected:
    // 顶点
    GLfloat triangleVertices[9] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
public:
    TriangleExample();
    virtual ~TriangleExample();
    virtual bool init();
    
    virtual void draw();
    
    virtual void destroy();

};

#endif //OPENGL_TRIANGLEEXAMPLE_H
