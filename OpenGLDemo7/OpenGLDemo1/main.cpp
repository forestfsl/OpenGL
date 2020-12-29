/*
 课程名称：OpenGL 视觉班--第二次课
 案例名称：OpenGL demo1
 实现功能：
    点击屏幕，将固定位置上的顶点数据以6种不同形态展示！
 */


#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


void renderScene(void){
    //设置清屏颜色为蓝色
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //1.现在裁剪成小红色分区
    //1.1设置裁剪区颜色为红色
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    //1.2设置裁剪尺寸
    glScissor(100, 100, 600, 400);
    //1.3开启裁剪测试
    glEnable(GL_SCISSOR_TEST);
    //1.4开启清屏，执行裁剪
    glClear(GL_COLOR_BUFFER_BIT);
    
    //2.裁剪一个绿色的小矩形
    //2.1设置清屏颜色为绿色
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
    //2.2设置裁剪尺寸
    glScissor(200, 200, 400, 200);
    //2.3开始清屏执行裁剪
    glClear(GL_COLOR_BUFFER_BIT);
    //关闭裁剪测试
    glDisable(GL_SCISSOR_TEST);
    
    //强制执行缓存区
    glutSwapBuffers();
}


void ChangeSize(int w, int h)
{
    //保证高度不能为0
    if(h == 0)
        h = 1;
    
    // 将视口设置为窗口尺寸
    glViewport(0, 0, w, h);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800,600);
    glutCreateWindow("OpenGL Scissor");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(renderScene);
    glutMainLoop();
    return 0;
}
