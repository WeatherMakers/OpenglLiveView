#pragma once

#include "ShaderProgram.h"
#include "example/BaseExample.h"

namespace hiveVG
{
    class TriangleExample : public BaseExample
    {
    protected:
        // 顶点
        GLfloat triangleVertices[9] = {
            0.0f, 0.5f, 0.0f, 
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
        };
    
    public:
        TriangleExample();
        virtual ~TriangleExample();
        virtual bool init();
        virtual void draw();
        
    private:
        CShaderProgram* shaderProgram;
    };
}