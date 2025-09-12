#include "ImageExample.h"
#include "GLUtil.h"
#include "app_util.h"
#include "log.h"
#include "render/EglRender.h"
#include <GLES3/gl3.h>
#include <rawfile/raw_file.h>
#include <rawfile/raw_file_manager.h>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace hiveVG;

CImageExample::CImageExample() : VBO(0), VAO(0), EBO(0), textureID(0) {}

CImageExample::~CImageExample()
{
    if (m_pShaderProgram != nullptr)
    {
        delete m_pShaderProgram;
    }
}

// bool CImageExample::init()
//{
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//     glGenBuffers(1, &EBO);
//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
//
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);
//     glBindVertexArray(0);
//    
//     const char *vertexSrc = "#version 300 es\n"
//                             "layout(location = 0) in vec3 aPos;\n"
//                             "layout(location = 1) in vec2 aTexCoord;\n"
//                             "out vec2 TexCoord;\n"
//                             "void main()\n"
//                             "{\n"
//                             "    gl_Position = vec4(aPos, 1.0);\n"
//                             "    TexCoord = aTexCoord;\n"
//                             "}\n";
//
//     const char *fragmentSrc = "#version 300 es\n"
//                               "precision mediump float;\n"
//                               "in vec2 TexCoord;\n"
//                               "out vec4 FragColor;\n"
//                               "uniform sampler2D uTexture;\n"
//                               "void main()\n"
//                               "{\n"
//                               "    FragColor = texture(uTexture, TexCoord);\n"
//                               "}\n";
//
//     program = GLUtil::createProgram(vertexSrc, fragmentSrc);
//     if (program == 0)
//     {
//         LOGI("Failed to create program");
//         return false;
//     }
//    
//     glGenTextures(1, &textureID);
//     glBindTexture(GL_TEXTURE_2D, textureID);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    
//     LOGD("ImageExample init: 尝试从原生文件加载 watercolor.png");
//     if (!loadFromNativeFile("watercolor.png"))
//     {
//         return false;
//     }
//
//     glUseProgram(program);
//     GLint texLoc = glGetUniformLocation(program, "uTexture");
//     if (texLoc >= 0)
//     {
//         glUniform1i(texLoc, 0);
//     }
//     LOGD("ImageExample init: 完成，program=%d, textureID=%u", program, textureID);
//     return true;
// }

bool CImageExample::init()
{

    // 初始化VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    m_pShaderProgram = CShaderProgram::createProgram("vertex.glsl", "fragment.glsl");
    if (!m_pShaderProgram)
    {
        LOGE("Failed to create shader program from files: vertex.glsl, fragment.glsl");
        return false;
    }

    program = m_pShaderProgram->getProgramID();
//    delete shaderProgram; // ShaderProgram会管理shader的清理

    // 设置纹理
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载默认纹理
    LOGD("initWithShaderFile: 尝试从原生文件加载 watercolor.png");
    if (!loadFromNativeFile("watercolor.png"))
    {
        return false;
    }

    glUseProgram(program);
    GLint texLoc = glGetUniformLocation(program, "uTexture");
    if (texLoc >= 0)
    {
        glUniform1i(texLoc, 0);
    }
    LOGD("initWithShaderFile: 完成，program=%d, textureID=%u", program, textureID);
    return true;
}

void CImageExample::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void CImageExample::setImageFromMemory(unsigned char *bytes, int length)
{
    LOGD("setImageFromMemory: bytes=%p, length=%d", bytes, length);
    if (bytes == nullptr || length <= 0)
    {
        LOGW("setImageFromMemory 参数无效");
        return;
    }
    int width = 0, height = 0, nrChannels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load_from_memory(bytes, length, &width, &height, &nrChannels, 0);
    if (!data)
    {
        LOGW("stbi_load_from_memory 失败");
        return;
    }
    LOGD("setImageFromMemory: 解码成功 w=%d h=%d c=%d", width, height, nrChannels);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    LOGD("setImageFromMemory: 纹理上传完成 format=%d", format);
    stbi_image_free(data);
}

bool CImageExample::loadFromRawfile(const char *filename, const NativeResourceManager *rm)
{
    LOGD("loadFromRawfile: filename=%s, rm=%p", filename ? filename : "(null)", rm);
    if (filename == nullptr || rm == nullptr)
    {
        return false;
    }
    RawFile *file = OH_ResourceManager_OpenRawFile(rm, filename);
    if (file == nullptr)
    {
        LOGE("OpenRawFile 失败: %s", filename);
        return false;
    }
    long rawLen = OH_ResourceManager_GetRawFileSize(file);
    LOGD("loadFromRawfile: 文件大小=%ld", rawLen);
    if (rawLen <= 0)
    {
        OH_ResourceManager_CloseRawFile(file);
        LOGE("GetRawFileSize 无效");
        return false;
    }
    std::vector<unsigned char> buf(static_cast<size_t>(rawLen));
    int readLen = OH_ResourceManager_ReadRawFile(file, buf.data(), buf.size());
    OH_ResourceManager_CloseRawFile(file);
    LOGD("loadFromRawfile: 读取字节=%d", readLen);
    if (readLen != rawLen)
    {
        LOGE("ReadRawFile 大小不一致");
        return false;
    }
    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *pixels = stbi_load_from_memory(buf.data(), rawLen, &width, &height, &channels, 0);
    if (!pixels)
    {
        LOGE("stbi_load_from_memory 失败");
        return false;
    }
    LOGD("loadFromRawfile: 解码成功 w=%d h=%d c=%d", width, height, channels);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    LOGD("loadFromRawfile: 纹理上传完成 format=%d", format);
    stbi_image_free(pixels);
    return true;
}

bool CImageExample::loadFromNativeFile(const char *filepath)
{
    LOGD("loadFromNativeFile: 尝试读取文件 %{public}s", filepath);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (!LoadPngFromAssetManager(filepath))
    {
        LOGE("Could not load png texture for colorMap.");
        return false;
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void CImageExample::destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
    if (program)
    {
        glDeleteProgram(program);
        program = 0;
    }
}
