#include "GLUtil.h"
#include "log.h"
#include "native_common.h"

GLuint GLUtil::createProgram(const char* vertexShader, const char* fragmentShader) {
    GLuint vertex = loadShader(GL_VERTEX_SHADER, vertexShader);
    if (vertex == PROGRAM_ERROR) {
        LOGE("编译顶点着色器失败");
        return PROGRAM_ERROR;
    }
    GLuint fragment = loadShader(GL_FRAGMENT_SHADER, fragmentShader);
    if (fragment == PROGRAM_ERROR) {
        LOGE("编译片元着色器失败");
        return PROGRAM_ERROR;
    }
    GLuint program = glCreateProgram();
    if (program == PROGRAM_ERROR) {
        LOGE("创建程序失败");
        return PROGRAM_ERROR;
    }
    // 绑定着色器
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    // 链接程序
    glLinkProgram(program);
    GLint link;
    glGetProgramiv(program, GL_LINK_STATUS, &link);
    if (PROGRAM_ERROR != link) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        // 链接程序成功
        return program;
    }
    // 链接程序失败
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteProgram(program);
    return PROGRAM_ERROR;
}

GLuint GLUtil::loadShader(GLenum shaderType, const char *shaderSrc) {
    // 创建着色器
    GLuint shader = glCreateShader(shaderType);
    // 绑定做得起源码
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    // 编译着色器
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (0 != compiled) {
        return shader;
    }
    glDeleteShader(shader);
    return PROGRAM_ERROR;
}