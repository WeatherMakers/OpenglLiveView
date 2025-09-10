#include "ImageExample.h"
#include <GLES3/gl3.h>
#include <string>
#include "napi/native_api.h"
#include <rawfile/raw_file.h>
#include <rawfile/raw_file_manager.h>
#include <vector>
#include "log.h"
#include "util/GLUtil.h"
#include "EglRender.h"
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb/stb_image.h"

CImageExample::CImageExample() : VBO(0), VAO(0), EBO(0), textureID(0) {}
CImageExample::~CImageExample() {}

bool CImageExample::init()
{
    // 1. 创建 VAO / VBO / EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // 顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 2. 创建 shader program（内联着色器，匹配现有quadvert/quadfrag）
    const char* vertexSrc =
        "#version 300 es\n"
        "layout(location = 0) in vec3 aPos;\n"
        "layout(location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "    TexCoord = aTexCoord;\n"
        "}\n";

    const char* fragmentSrc =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D uTexture;\n"
        "void main()\n"
        "{\n"
        "    FragColor = texture(uTexture, TexCoord);\n"
        "}\n";

    program = GLUtil::createProgram(vertexSrc, fragmentSrc);
    if (program == 0) {
        LOGI("Failed to create program");
        return false;
    }

    // 3. 纹理初始化：尝试直接从 rawfile 加载（无需ArkTS传递）
    LOGD("ImageExample init: 创建纹理并尝试从rawfile加载");
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 检查是否为原生文件加载模式
    if (EglRender::getInstance()->currentImageMode == IMAGE_FROM_NATIVE_TYPE) {
        LOGD("ImageExample init: 尝试从原生文件加载 watercolor.png");
        if (!loadFromNativeFile("watercolor.png")) {
            LOGW("从原生文件加载失败，尝试rawfile方式");
            const NativeResourceManager* rm = EglRender::getInstance()->resourceManager;
            if (rm != nullptr) {
                loadFromRawfile("watercolor.png", rm);
            }
        }
    } else {
        // 暂时跳过rawfile加载，避免闪退
        LOGD("ImageExample init: 跳过rawfile加载，等待setImage传入数据");
    }

    // 设置纹理采样器到纹理单元0
    glUseProgram(program);
    GLint texLoc = glGetUniformLocation(program, "uTexture");
    if (texLoc >= 0) {
        glUniform1i(texLoc, 0);
    }
    LOGD("ImageExample init: 完成，program=%d, textureID=%u", program, textureID);
    LOGD("ImageExample init: 纹理已创建但未加载图片，请通过setImage传入数据");

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

void CImageExample::setImageFromMemory(unsigned char* bytes, int length)
{
    LOGD("setImageFromMemory: bytes=%p, length=%d", bytes, length);
    if (bytes == nullptr || length <= 0) {
        LOGW("setImageFromMemory 参数无效");
        return;
    }
    int width = 0, height = 0, nrChannels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load_from_memory(bytes, length, &width, &height, &nrChannels, 0);
    if (!data) {
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

bool CImageExample::loadFromRawfile(const char* filename, const NativeResourceManager* rm)
{
    LOGD("loadFromRawfile: filename=%s, rm=%p", filename ? filename : "(null)", rm);
    if (filename == nullptr || rm == nullptr) {
        return false;
    }
    RawFile* file = OH_ResourceManager_OpenRawFile(rm, filename);
    if (file == nullptr) {
        LOGE("OpenRawFile 失败: %s", filename);
        return false;
    }
    long rawLen = OH_ResourceManager_GetRawFileSize(file);
    LOGD("loadFromRawfile: 文件大小=%ld", rawLen);
    if (rawLen <= 0) {
        OH_ResourceManager_CloseRawFile(file);
        LOGE("GetRawFileSize 无效");
        return false;
    }
    std::vector<unsigned char> buf(static_cast<size_t>(rawLen));
    int readLen = OH_ResourceManager_ReadRawFile(file, buf.data(), buf.size());
    OH_ResourceManager_CloseRawFile(file);
    LOGD("loadFromRawfile: 读取字节=%d", readLen);
    if (readLen != rawLen) {
        LOGE("ReadRawFile 大小不一致");
        return false;
    }
    int width = 0, height = 0, channels = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* pixels = stbi_load_from_memory(buf.data(), rawLen, &width, &height, &channels, 0);
    if (!pixels) {
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

bool CImageExample::loadFromNativeFile(const char* filepath)
{
    LOGD("loadFromNativeFile: 尝试读取文件 %s", filepath);
    
    // 尝试多个可能的路径
    std::string rawfilePath = "resources/rawfile/" + std::string(filepath);
    std::string assetsPath = "assets/images/" + std::string(filepath);
    std::string sandboxPath = "/data/storage/el2/base/haps/entry/files/" + std::string(filepath);
    
    const char* possiblePaths[] = {
        filepath,           // 直接路径
        rawfilePath.c_str(), // rawfile路径
        assetsPath.c_str(),  // assets路径
        sandboxPath.c_str()  // 应用沙箱路径
    };
    
    for (int i = 0; i < 4; i++) {
        LOGD("loadFromNativeFile: 尝试路径 %s", possiblePaths[i]);
        
        int width = 0, height = 0, channels = 0;
        stbi_set_flip_vertically_on_load(1);
        unsigned char* pixels = stbi_load(possiblePaths[i], &width, &height, &channels, 0);
        
        if (pixels != nullptr) {
            LOGD("loadFromNativeFile: 成功读取文件 w=%d h=%d c=%d", width, height, channels);
            
            glBindTexture(GL_TEXTURE_2D, textureID);
            GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
            glGenerateMipmap(GL_TEXTURE_2D);
            LOGD("loadFromNativeFile: 纹理上传完成 format=%d", format);
            
            stbi_image_free(pixels);
            return true;
        } else {
            LOGW("loadFromNativeFile: 路径 %s 读取失败: %s", possiblePaths[i], stbi_failure_reason());
        }
    }
    
    LOGE("loadFromNativeFile: 所有路径都失败");
    return false;
}

void CImageExample::destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &textureID);
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}
