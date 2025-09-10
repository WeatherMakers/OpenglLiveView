#ifndef OPENGL_GLUTIL_H
#define OPENGL_GLUTIL_H

#include <GLES3/gl3.h>

class GLUtil 
{
public:
    static GLuint createProgram(const char* vertexShader, const char* fragmentShader);
    static GLuint loadShader(GLenum shaderType, const char *shaderSrc);
};

#endif //OPENGL_GLUTIL_H
