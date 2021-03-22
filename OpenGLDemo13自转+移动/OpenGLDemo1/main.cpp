
#include "GLShaderManager.h"
#include "GLFrustum.h"
#include "GLBatch.h"
#include "GLMatrixStack.h"
#include "GLGeometryTransform.h"
#include "StopWatch.h"

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager shaderManager;  //着色器管理器
GLMatrixStack modelViewMatrix;  //模型视图矩阵
GLMatrixStack projectionMatrix; //投影矩阵
GLFrustum viewFrustum;         //视景体
GLGeometryTransform transformPipeline;  //几何图形变化管道

GLTriangleBatch torusBatch; //打球
GLTriangleBatch sphereBatch;    //小球
GLBatch floorBatch; //地板

//角色幁，照相机角色幁
GLFrame cameraFrame;

//添加附加随机球
#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

void SetupRC()
{
    //1.初始化
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    shaderManager.InitializeStockShaders();
    
    //2.开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    //3.设置地板顶点数据
    floorBatch.Begin(GL_LINES, 324);
    for (GLfloat x = - 20.0; x <= 20.0f; x += 0.5) {
        floorBatch.Vertex3f(x, -0.55f, 20.0f);
        floorBatch.Vertex3f(x, -0.55f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.55f, x);
        floorBatch.Vertex3f(-20.0f, -0.55f, x);
    }
    
    floorBatch.End();
    
    //4.设置大球模型
    gltMakeSphere(torusBatch, 0.4f, 40, 80);
    
    //5.设置小球模型
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);
    
    //6.随机位置放置小球球
    for (int i = 0; i < NUM_SPHERES; i++) {
        
        //y轴不变，X,Z产生随机值
        GLfloat x = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200 ) * 0.1f);
        
        //在y方向，将球体设置为0.0的位置，这使得它们看起来是飘浮在眼睛的高度
        //对spheres数组中的每一个顶点，设置顶点数据
        spheres[i].SetOrigin(x, 0.0f, z);
    }
}

//进行调用以绘制场景
void RenderScene(void)
{
    //1.颜色值(地板，大球，小球)
    static GLfloat vFloorColor[] = {0.0f,1.0f,0.0f,1.0f};
    static GLfloat vTorusColor[] = {1.0f,0.0f,0.0f,1.0f};
    static GLfloat vSphereColor[] = {0.0f,0.0f,1.0f,1.0f};
    
    //2.基于时间动画
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    //3.清除颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //4.加入观察者
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.PushMatrix(mCamera);
    
    //5.绘制地面
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vFloorColor);
    
    floorBatch.Draw();
    
    //6.获取光源位置
    M3DVector4f vLightPos = {0.0f,10.0f,5.0f,1.0f};
    
    //7.使得大球位置平移3.0 想屏幕里面
    modelViewMatrix.Translate(0.0f, 0.0f, -3.0f);
    //8.压栈
    modelViewMatrix.PushMatrix();
    //9.大球自转
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    
    //10.指定合适的着色器
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vTorusColor);
    torusBatch.Draw();
    
    //11.绘制完毕则pop
    modelViewMatrix.PopMatrix();
    
    //12.画小球
    for (int i = 0; i < NUM_SPHERES; i++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vLightPos,vSphereColor);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    
    //13.让一个小篮球围绕大球公众自转
    modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT,transformPipeline.GetModelViewProjectionMatrix(),vSphereColor);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    //13.执行缓存区交换
    glutSwapBuffers();
    //14.
    glutPostRedisplay();
}

//屏幕更改大小或已初始化
void ChangeSize(int nWidth, int nHeight){
    //1.设置视口大小
    glViewport(0, 0, nWidth, nHeight);
    //2.创建投影矩阵
    viewFrustum.SetPerspective(35.0f, float(nWidth)/float(nHeight), 1.0f, 100.0f);
    //并将其加载到投影矩阵堆栈上
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    //3.设置变换管道以使用两个矩阵堆栈(变换矩阵modelViewMatrix，投影矩阵projectionMatrix)
    //初始化GLGeometryTransform的实例transformPipeline，通过将它的内部指针设置为模型视图矩阵堆栈和投影矩阵堆栈实例，来完成初始化
    //当然这个操作也可以在SetupRC函数中完成，但是在窗口大小改变或者窗口创建时设置它们并没有坏处，而且这样可以一次性完成矩阵和管线的设置
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL SphereWorld");
    
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);

    
    SetupRC();
    
    glutMainLoop();
    return 0;
}
