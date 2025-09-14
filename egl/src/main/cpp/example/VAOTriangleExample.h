#pragma once

#include "example/TriangleExample.h"
#define VBO_COUNT 1
#define VAO_COUNT 1

namespace hiveVG 
{
    class VAOTriangleExample: public TriangleExample {
    
    private:
        GLuint vboIds[1] = {};
        GLuint vaoIds[1] = {};
        GLint colorHandler;
    public:
        VAOTriangleExample();
        ~VAOTriangleExample();
        virtual bool init();
        virtual void draw();
    };
}
