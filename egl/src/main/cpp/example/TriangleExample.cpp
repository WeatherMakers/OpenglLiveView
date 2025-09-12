#include "TriangleExample.h"

TriangleExample::TriangleExample() {}

TriangleExample::~TriangleExample()
{ 
    destroy(); 
}

bool TriangleExample::init()
{
    program = GLUtil::createProgram(vertexShader, fragmentShader);
    if (program == PROGRAM_ERROR)
    {
        LOGE("链接程序失败");
        return false;
    }
    return true;
}

void TriangleExample::draw()
{
    // 使用程序
    glUseProgram(program);
    // 获取顶点着色器中定义的属性
    GLint positionHandler = glGetAttribLocation(program, "vPosition");
    // 启用顶点数组
    glEnableVertexAttribArray(positionHandler);
    /*
     * 向顶点着色器传递顶点数组
     * 第一个参数是属性变量的下标
     * 第二个参数是顶点坐标的个数，我们在定义顶点坐标的时候，使用了空间坐标系，每个坐标使用x，y，z，所以第二个参数为3
     * 第三个参数是数据的类型
     * 第四个参数是否进行了归一化处理，这里写false
     * 第五个参数是跨度，这里是0，没有跨度
     * 第六个参数是要传递的顶点数据
     */
    glVertexAttribPointer(positionHandler, 3, GL_FLOAT, false, 0, triangleVertices);
    // 获取片元着色器中定义的变量
    GLint colorHandler = glGetUniformLocation(program, "vColor");
    /*
     * 向片元着色器传递颜色
     * 第一个参数是变量的下标
     * 第二个参数是数据的数量，由于将所有的像素都设置成一样的颜色，所以第二个参数是1
     * 第三个参数是颜色
     */
    // 颜色值#7E8FFB
    const GLfloat DRAW_COLOR[] = {126.0f / 255, 143.0f / 255, 251.0f / 255, 1.0f};
    glUniform4fv(colorHandler, 1, DRAW_COLOR);
    // 绘制三角形
    GLsizei count = sizeof(triangleVertices) / sizeof(triangleVertices[0]) / 3;
    /*
     * 绘制三角形
     * 第一个参数是绘制的图形
     * 第二个参数是从哪里开始读取，这里从0开始读取
     * 第三个参数是顶点的数量，三角形有三个顶点，第三个参数就是3
     */
    glDrawArrays(GL_TRIANGLES, 0, count);
    // 释放属性变量
    glDisableVertexAttribArray(positionHandler);
}

void TriangleExample::destroy()
{
    LOGD("释放三角形资源");
    // 暂时没有需要释放的资源
}