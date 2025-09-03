//
// Created on 2024/8/31.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_BASEEXAMPLE_H
#define OPENGL_BASEEXAMPLE_H

#include <GLES3/gl3.h>
#include "native_common.h"
#include "util/GLUtil.h"
#include "log.h"
#include "napi/native_api.h"

#define TRIANGLE_TYPE 1
#define VBO_TRIANGLE_TYPE TRIANGLE_TYPE + 1
#define EBO_TRIANGLE_TYPE VBO_TRIANGLE_TYPE + 1
#define VAO_TRIANGLE_TYPE EBO_TRIANGLE_TYPE + 1

class BaseExample {
public:
    GLuint program;
protected:
    // 默认的顶点着色器
    const char *vertexShader = "attribute vec4 vPosition;\n"
                            "void main() {              \n"
                            "   gl_Position = vPosition;\n"
                            "}";
    // 默认的片元着色器
    const char *fragmentShader = "precision mediump float;\n"
                            "uniform vec4 vColor;      \n"
                            "void main() {             \n"
                            "   gl_FragColor = vColor; \n"
                            "}";
    
public:
    BaseExample();
    virtual ~BaseExample();
    virtual bool init() = 0;
    
    virtual void draw() = 0;
    
    virtual void destroy() = 0;
};

#endif //OPENGL_BASEEXAMPLE_H
