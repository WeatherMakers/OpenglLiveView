#pragma once

#include "OpenGLCommon.h"

class GLUtil 
{
public:
    static GLuint createProgram(const char* vertexShader, const char* fragmentShader);
    static GLuint loadShader(GLenum shaderType, const char *shaderSrc);
};

