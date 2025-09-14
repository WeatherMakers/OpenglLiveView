//
// Created on 2024/9/1.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "EBORectangleExample.h"

using namespace hiveVG;

EBORectangleExample::EBORectangleExample() {}

EBORectangleExample::~EBORectangleExample() { destroy(); }

bool EBORectangleExample::init()
{
    program = GLUtil::createProgram(vertexShader, fragmentShader);
    if (program == PROGRAM_ERROR)
    {
        LOGE("链接程序失败");
        return false;
    }
    // 使用EBO的前提是绑定VBO
    glGenBuffers(VBO_COUNT, vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    // 将顶点数据复制到VBO对象
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    // 解绑vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(EBO_COUNT, eboIds);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboIds[0]);
    // 将索引数据复制到EBO对象，也就是将数据从CPU拷贝到GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 解绑ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

void EBORectangleExample::draw()
{
    glUseProgram(program);
    // 告知着色器从哪个vbo中读取数据
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    // 绑定ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboIds[0]);
    // 获取顶点着色器中定义的属性
    GLint positionHandler = glGetAttribLocation(program, "vPosition");
    // 启用顶点数组
    glEnableVertexAttribArray(positionHandler);
    /*
     * 顶点数据已经拷贝到GPU，直接从GPU读取顶点数据
     * 第一个参数是属性变量的下标
     * 第二个参数是顶点坐标的个数，我们在定义顶点坐标的时候，使用了空间坐标系，每个坐标使用x，y，z，所以第二个参数为3
     * 第三个参数是数据的类型
     * 第四个参数是否进行了归一化处理，这里写false
     * 第五个参数是跨度，这里是0，没有跨度
     * 第六个参数为0，表示从GPU读取数据
     */
    glVertexAttribPointer(positionHandler, 3, GL_FLOAT, false, 0, 0);
    // 获取片元着色器中定义的变量
    GLint colorHandler = glGetUniformLocation(program, "vColor");
    /*
     * 向片元着色器传递颜色
     * 第一个参数是变量的下标
     * 第二个参数是数据的数量，由于将所有的像素都设置成一样的颜色，所以第二个参数是1
     * 第三个参数是颜色
     */
    // 绿色
    const GLfloat DRAW_COLOR[] = {0, 255, 0, 1.0f};
    glUniform4fv(colorHandler, 1, DRAW_COLOR);
    // 绘制三角形
    GLsizei count = sizeof(indices) / sizeof(indices[0]);
    /*
     * 绘制矩形
     * 第一个参数是绘制的图形
     * 第二个参数是顶点的个数，三角形顶点个数是3，矩形由两个三角形组成，顶点个数是6
     * 第三个数数指定数据类型
     * 第四个参数是从哪里开始读取，这里从0开始读取
     */
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void *)0);
    // 解绑vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBORectangleExample::destroy()
{
    LOGD("释放EBO资源");
    glDeleteBuffers(EBO_COUNT, eboIds);
    glDeleteBuffers(VBO_COUNT, vboIds);
}