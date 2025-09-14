#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "OpenGLCommon.h"

namespace hiveVG
{
    class CShaderProgram
    {
    public:
        static CShaderProgram* createProgram(const std::string& vVertFilePath, const std::string& vFragFilePath);
        ~CShaderProgram();

        void useProgram() const;
        void setUniform(const std::string& vName, int vValue);
        void setUniform(const std::string& vName, float vValue);
        void setUniform(const std::string& vName, const glm::vec2& vValue);
        void setUniform(const std::string& vName, const glm::vec3& vValue);
        void setUniform(const std::string& vName, const glm::vec4& vValue);
        void setUniform(const std::string& vName, const glm::mat3& vMat);
        void setUniform(const std::string& vName, const glm::mat4& vMat);
        GLuint getProgramID(){return m_ProgramHandle;}
    private:
        static bool __dumpShaderCodeFromFile(const std::string& vShaderPath, std::string& voShaderCode);
        static bool __compileShader(GLenum vType, const std::string& vShaderPath, const std::string& vShaderCode, GLuint& voShaderHandle);
        static bool __linkProgram(const std::vector<GLuint>& vShaderHandles, GLuint& voProgramHandle);
        CShaderProgram(const std::vector<GLuint>& vShaderHandles, GLuint vProgramHandle);
        GLint  __getOrCreateUniformId(const std::string& vUniformName);

        GLuint                                 m_ProgramHandle;
        std::vector<GLuint>                    m_ShaderHandles;
        std::unordered_map<std::string, GLint> m_UniformIdMap;
    };
}