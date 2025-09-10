#pragma once

#include "example/BaseExample.h"

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
    virtual void destroy();
};
