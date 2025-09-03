//
// Created on 2024/8/31.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef OPENGL_GLUTIL_H
#define OPENGL_GLUTIL_H

#include <GLES3/gl3.h>
class GLUtil {
public:
    static GLuint createProgram(const char* vertexShader, const char* fragmentShader);
    static GLuint loadShader(GLenum shaderType, const char *shaderSrc);
};

#endif //OPENGL_GLUTIL_H
