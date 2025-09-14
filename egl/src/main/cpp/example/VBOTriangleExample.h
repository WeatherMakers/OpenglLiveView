#pragma once

#include "example/TriangleExample.h"
#define VBO_COUNT 1

namespace hiveVG
{
    class VBOTriangleExample : public TriangleExample
    {
    private:
        GLuint vboIds[1] = {};
    
    public:
        VBOTriangleExample();
        virtual ~VBOTriangleExample();
        virtual bool init();
        virtual void draw();
    };
}
