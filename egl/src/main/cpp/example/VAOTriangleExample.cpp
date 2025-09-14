#include "VAOTriangleExample.h"

using namespace hiveVG;

VAOTriangleExample::VAOTriangleExample() {}

VAOTriangleExample::~VAOTriangleExample()
{
    LOGD("释放VBO和VAO");
    glDeleteBuffers(VBO_COUNT, vboIds);
    glDeleteBuffers(VAO_COUNT, vaoIds);
}

bool VAOTriangleExample::init()
{
    program = GLUtil::createProgram(vertexShader, fragmentShader);
    if (program == PROGRAM_ERROR)
    {
        LOGE("链接程序失败");
        return false;
    }
    // 生成vao对象
    glGenVertexArrays(VAO_COUNT, vaoIds);
    // 绑定vao对象
    glBindVertexArray(vaoIds[0]);

    // 生成vbo对象
    glGenBuffers(VBO_COUNT, vboIds);
    // 绑定vbo
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    // 将顶点数据复制到VBO对象
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
    /*
     * 直接从VBO中读取数据，VBO里面的数据会记录到VAO里面，绘制的时候只需要拿到VAO，进而拿到VBO的数据进行绘制
     */
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
    colorHandler = glGetUniformLocation(program, "vColor");

    // 解绑vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 解绑vao对象
    glBindVertexArray(0);
    return true;
}

void VAOTriangleExample::draw()
{
    glUseProgram(program);
    // 绑定vao，减少glBindBuffer、glEnableVertexAttribArray、glVertexAttribPointer这些调用操作
    glBindVertexArray(vaoIds[0]);

    /*
     * 向片元着色器传递颜色
     * 第一个参数是变量的下标
     * 第二个参数是数据的数量，由于将所有的像素都设置成一样的颜色，所以第二个参数是1
     * 第三个参数是颜色
     */
    // 蓝色
    const GLfloat DRAW_COLOR[] = {0, 0, 255, 1.0f};
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
    // 解绑vao
    glBindVertexArray(0);
}