#include "ShaderProgram.h"
#include "Common.h"
#include "log.h"
#include "FileUtils.h"

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
    auto pAsset = CFileUtils::openFile(vShaderPath.c_str());
    assert(pAsset);
    if (!pAsset)
        return false;
    size_t AssetSize = CFileUtils::getFileBytes(pAsset);
    std::unique_ptr<char[]> pBuffer(new char[AssetSize + 1]);
    int Flag = CFileUtils::readFile<char>(pAsset, pBuffer.get(), AssetSize);
    if(Flag < 0)
        return false;
    CFileUtils::closeFile(pAsset);

    pBuffer[AssetSize] = '\0';
    voShaderCode = std::string(pBuffer.get());
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
        LOGE(TAG_KEYWORD::SHADER_PROGRAM_TAG, "Failed to compile shader: %{public}s; Reason: %{public}s", vShaderPath.c_str(), std::string(InfoLog).c_str());
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
        LOGE(TAG_KEYWORD::SHADER_PROGRAM_TAG, "Failed to link shader program: %{public}s", std::string(InfoLog).c_str());
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
        LOGE(TAG_KEYWORD::SHADER_PROGRAM_TAG, "Uniform %{public}s does not exist or is not set correctly", vUniformName.c_str());
    }
    m_UniformIdMap[vUniformName] = UniformId;
    return UniformId;
}
