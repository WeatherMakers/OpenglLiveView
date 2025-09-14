#include "ShaderProgram.h"
#include "Common.h"
#include "log.h"
#include "AppContext.h"
//#include "FileUtils.h"

using namespace hiveVG;

CShaderProgram* CShaderProgram::createProgram(const std::string& vVertFilePath, const std::string& vFragFilePath)
{
    std::string VertCode, FragCode;
    if (!__dumpShaderCodeFromFile(vVertFilePath, VertCode)) return nullptr;
    if (!__dumpShaderCodeFromFile(vFragFilePath, FragCode)) return nullptr;
    GLuint VertHandle, FragHandle;
    if (!__compileShader(GL_VERTEX_SHADER, vVertFilePath, VertCode, VertHandle)) return nullptr;
    if (!__compileShader(GL_FRAGMENT_SHADER, vFragFilePath, FragCode, FragHandle)) return nullptr;
    const std::vector ShaderHandles{ VertHandle, FragHandle };
    GLuint ProgramHandle;
    if (!__linkProgram(ShaderHandles, ProgramHandle))
    {
        for (const auto& ShaderHandle : ShaderHandles)
        {
            glDeleteShader(ShaderHandle);
        }
        return nullptr;
    }
    return new CShaderProgram(ShaderHandles, ProgramHandle);
}

CShaderProgram::~CShaderProgram()
{
    glDeleteProgram(m_ProgramHandle);
    m_ProgramHandle = 0;
    for (const auto& ShaderHandle : m_ShaderHandles)
    {
        glDeleteShader(ShaderHandle);
    }
    m_ShaderHandles.clear();
}

void CShaderProgram::useProgram() const
{
    glUseProgram(m_ProgramHandle);
}

void CShaderProgram::setUniform(const std::string& vName, int vValue)
{
    glUniform1iv(__getOrCreateUniformId(vName), 1, &vValue);
}

void CShaderProgram::setUniform(const std::string& vName, float vValue)
{
    glUniform1fv(__getOrCreateUniformId(vName), 1, &vValue);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::vec2& vValue)
{
    glUniform2fv(__getOrCreateUniformId(vName), 1, &vValue[0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::vec3& vValue)
{
    glUniform3fv(__getOrCreateUniformId(vName), 1, &vValue[0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::vec4& vValue)
{
    glUniform4fv(__getOrCreateUniformId(vName), 1, &vValue[0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::mat3& vMat)
{
    glUniformMatrix3fv(__getOrCreateUniformId(vName), 1, GL_FALSE, &vMat[0][0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::mat4& vMat)
{
    glUniformMatrix4fv(__getOrCreateUniformId(vName), 1, GL_FALSE, &vMat[0][0]);
}

bool CShaderProgram::__dumpShaderCodeFromFile(const std::string& vShaderPath, std::string& voShaderCode)
{
    if (!CAppContext::getResourceManager())
    {
        LOGE("NativeResourceManager is not initialized");
        return false;
    }
    
    RawFile* rawFile = OH_ResourceManager_OpenRawFile(CAppContext::getResourceManager(), vShaderPath.c_str());
    if (!rawFile)
    {
        LOGE("Failed to open shader file: %s", vShaderPath.c_str());
        return false;
    }
    
    long fileSize = OH_ResourceManager_GetRawFileSize(rawFile);
    if (fileSize <= 0)
    {
        LOGE("Invalid file size for shader file: %s", vShaderPath.c_str());
        OH_ResourceManager_CloseRawFile(rawFile);
        return false;
    }
    
    std::unique_ptr<char[]> buffer(new char[fileSize + 1]);
    if (!buffer)
    {
        LOGE("Failed to allocate memory for shader file: %s", vShaderPath.c_str());
        OH_ResourceManager_CloseRawFile(rawFile);
        return false;
    }
    
    long bytesRead = OH_ResourceManager_ReadRawFile(rawFile, buffer.get(), fileSize);
    if (bytesRead != fileSize)
    {
        LOGE("Failed to read complete shader file: %s, expected: %ld, actual: %ld", 
            vShaderPath.c_str(), fileSize, bytesRead);
        OH_ResourceManager_CloseRawFile(rawFile);
        return false;
    }
    
    OH_ResourceManager_CloseRawFile(rawFile);
    
    buffer[fileSize] = '\0';
    voShaderCode = std::string(buffer.get());
    LOGD("Successfully loaded shader file: %{public}s, size: %{public}ld bytes", vShaderPath.c_str(), fileSize);
    return true;
}

bool CShaderProgram::__compileShader(GLenum vType, const std::string& vShaderPath, const std::string& vShaderCode, GLuint& voShaderHandle)
{
    voShaderHandle = glCreateShader(vType);
    if (voShaderHandle == 0) return false;

    const char* ShaderCodeCStr = vShaderCode.c_str();
    glShaderSource(voShaderHandle, 1, &ShaderCodeCStr, nullptr);
    glCompileShader(voShaderHandle);
    GLint CompileStatus = GL_FALSE;
    glGetShaderiv(voShaderHandle, GL_COMPILE_STATUS, &CompileStatus);
    if (CompileStatus == GL_FALSE)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(voShaderHandle, 1024, nullptr, InfoLog);
        LOGE("Failed to compile shader: %{public}s; Reason: %{public}s", vShaderPath.c_str(), InfoLog);
        glDeleteShader(voShaderHandle);
        voShaderHandle = 0;
        return false;
    }
    return true;
}

bool CShaderProgram::__linkProgram(const std::vector<GLuint>& vShaderHandles, GLuint& voProgramHandle)
{
    voProgramHandle = glCreateProgram();
    if (voProgramHandle == 0) return false;

    for (const auto& ShaderHandle : vShaderHandles)
    {
        glAttachShader(voProgramHandle, ShaderHandle);
    }
    glLinkProgram(voProgramHandle);
    GLint LinkStatus = GL_FALSE;
    glGetProgramiv(voProgramHandle, GL_LINK_STATUS, &LinkStatus);
    if (LinkStatus == GL_FALSE)
    {
        GLchar InfoLog[1024];
        glGetProgramInfoLog(voProgramHandle, 1024, nullptr, InfoLog);
        LOGE("Failed to link shader program: %{public}s", InfoLog);
        glDeleteProgram(voProgramHandle);
        voProgramHandle = 0;
        return false;
    }
    return true;
}

CShaderProgram::CShaderProgram(const std::vector<GLuint>& vShaderHandles, GLuint vProgramHandle)
        : m_ShaderHandles(vShaderHandles), m_ProgramHandle(vProgramHandle) {}

GLint CShaderProgram::__getOrCreateUniformId(const std::string& vUniformName)
{
    if (m_UniformIdMap.count(vUniformName) > 0) return m_UniformIdMap[vUniformName];
    const auto UniformId = glGetUniformLocation(m_ProgramHandle, vUniformName.c_str());
    if (UniformId == -1)
    {
        LOGE("Uniform %{public}s does not exist or is not set correctly", vUniformName.c_str());
    }
    m_UniformIdMap[vUniformName] = UniformId;
    return UniformId;
}
