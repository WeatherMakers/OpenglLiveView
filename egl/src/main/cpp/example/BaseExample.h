#pragma once

#include "log.h"
#include "napi/native_api.h"
#include "native_common.h"
#include "GLUtil.h"
#include <GLES3/gl3.h>

#define TRIANGLE_TYPE 1
#define VBO_TRIANGLE_TYPE TRIANGLE_TYPE + 1
#define EBO_TRIANGLE_TYPE VBO_TRIANGLE_TYPE + 1
#define VAO_TRIANGLE_TYPE EBO_TRIANGLE_TYPE + 1
#define IMAGE_TYPE VAO_TRIANGLE_TYPE + 1
#define IMAGE_FROM_NATIVE_TYPE IMAGE_TYPE + 1

class BaseExample
{
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
