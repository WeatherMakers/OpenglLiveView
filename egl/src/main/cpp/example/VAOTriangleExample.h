//
// Created on 2024/9/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_VAOTRIANGLEEXAMPLE_H
#define OPENGL_VAOTRIANGLEEXAMPLE_H

#include "example/TriangleExample.h"
#define VBO_COUNT 1
#define VAO_COUNT 1

class VAOTriangleExample: public TriangleExample {

private:
    GLuint vboIds[1] = {};
    GLuint vaoIds[1] = {};
    GLint colorHandler;
public:
    VAOTriangleExample();
    ~VAOTriangleExample();
    virtual bool init();
    virtual void draw();
    virtual void destroy();
};

#endif //OPENGL_VAOTRIANGLEEXAMPLE_H
