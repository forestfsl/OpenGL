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


///设置角色幁,作为相机(设置观察者视图坐标)
GLFrame viewFrame;
//使用GLFrustum类来设置透视投影(设置图元绘制时的投影方式)
GLFrustum viewFrustum;
//帮助类/容器类
GLTriangleBatch torusBatch;
GLMatrixStack modelViewMatix;
//投影矩阵
GLMatrixStack projectionMatrix;
//变换管道.存储模型视图/投影/模型视图投影矩阵
GLGeometryTransform transformPipeline;
//存储着色器管理工具类
GLShaderManager shaderManager;

//标记：背面剔除，深度测试
int iCull = 0;//是否开启背面剔除
int iDepth = 0;//是否开启深度测试


//自定义函数，通过glutReshaperFunc(函数名)注册为重塑函数，当屏幕发生变化或者第一次创建窗口时，会调用该函数调整窗口大小/视口大小
void changeSize(int w, int h){
    //1.防止h变为0
    if (h == 0)
        h = 1;
    
    //2.设置视图窗口尺寸
    glViewport(0, 0, w, h);
    
    //3.setPerspective 函数的参数是一个从顶点方向看去的视场角度(用角度值表示)
    //设置透视模式，初始化透视矩阵
    viewFrustum.SetPerspective(35.0, floor(w)/floor(h), 1.0f, 100.0f);
    
    //4.把透视矩阵加载到透视矩阵对阵中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    //5.初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);

}

//键位设置，通过不同的键位对其进行设置
void SpecialKeys(int key,int x,int y){
    //1.判断方向
    if (key == GLUT_KEY_UP) {
        //2.根据方向调整观察者位置
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_DOWN) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    }
    if (key == GLUT_KEY_LEFT) {
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    }
    if (key == GLUT_KEY_RIGHT) {
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    }
    //3.重新刷新
    glutPostRedisplay();
}

//自定义函数，通过glutDisplayFunc(函数名)注册为显示渲染函数。当屏幕发生变化或者开发者主动渲染会调用此函数,用来实现数据->渲染过程

void renderScene(){
    //1.清除窗口和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //开启、关闭正背面剔除功能
    if (iCull) {
        //开启正背面剔除
        glEnable(GL_CULL_FACE);
        //指定那个面为正面
        glFrontFace(GL_CCW);
        //剔除哪个面
        glCullFace(GL_BACK);
    }else{
        glDisable(GL_CULL_FACE);
    }
    
    //2.把摄像机矩阵压入模型矩阵中
    modelViewMatix.PushMatrix(viewFrame);
    
    //3.设置绘圆颜色
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    
    //4.使用默认光源着色器
    /*
     通过光源，隐形效果跟体现立体效果
     //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
     //参数2：模型视图矩阵
     //参数3：投影矩阵
     //参数4：基本颜色值
     */
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);
    
    //5.绘制
    torusBatch.Draw();
    
    //6.出栈 绘制完成恢复
    modelViewMatix.PopMatrix();
    
    //7.交换缓存区
    glutSwapBuffers();
}


void setupRC(){
    //1.设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    //2.初始化着色器管理器
    shaderManager.InitializeStockShaders();
    
    //3.将相机向后移动7个单元：肉眼到物体之间的距离
    viewFrame.MoveForward(7.0);
    
    /*
     创建一个甜甜圈
     //参数1：GLTriangleBatch 容器帮助类
     //参数2：外边缘半径
     //参数3：内边缘半径
     //参数4 5，主半径和从半径的细分单元数量
     */
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    //5.点的大小(方便点填充，肉眼观察)
    glPointSize(4.0f);
}


void processMenu(int value){
    switch (value) {
        case 1:
            iDepth = !iDepth;
            break;
        case 2:
            iCull = !iCull;
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Geometry Test Program");
    glutReshapeFunc(changeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(renderScene);
    
    //添加右击菜单栏
    //create the menu
    glutCreateMenu(processMenu);
    glutAddMenuEntry("Toggle depth test", 1);
    glutAddMenuEntry("Toggle cull backface", 2);
    glutAddMenuEntry("Set Fill mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    setupRC();
    
    //runloop运行循环
    glutMainLoop();
    return 0;
}
