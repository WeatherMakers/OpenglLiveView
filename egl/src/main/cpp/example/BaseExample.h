#pragma once

#include "native_common.h"
#include <GLES3/gl3.h>

#define IMAGE_FROM_NATIVE_TYPE 1
#define SINGLE_PLAYER_TYPE IMAGE_FROM_NATIVE_TYPE + 1
#define SEQ_PLAYER_TYPE SINGLE_PLAYER_TYPE + 1

namespace hiveVG
{
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
    };
}

