//
// Created on 2024/9/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_VBOTRIANGLEEXAMPLE_H
#define OPENGL_VBOTRIANGLEEXAMPLE_H

#include "example/TriangleExample.h"
#define VBO_COUNT 1

namespace hiveVG {

/**
 * 使用VBO绘制三角形
 */
class VBOTriangleExample : public TriangleExample
{

private:
    GLuint vboIds[1] = {};

public:
    VBOTriangleExample();
    virtual ~VBOTriangleExample();
    virtual bool init();

    virtual void draw();

    virtual void destroy();
};

} // namespace hiveVG

#endif // OPENGL_VBOTRIANGLEEXAMPLE_H
