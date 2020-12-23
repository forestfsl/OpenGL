//
//  main.cpp
//  OpenGLDemo1
//
//  Created by fengsl on 2020/12/23.
//
//
//GLShaderManager.h 移入了GLTool着色管理器(shader Manager)类，没有着色器的话，我们就不能在OpenGL中进行着色
#include "GLShaderManager.h"
//GLTools.h 文件包含了大部分GLTool中类似C语言的独立函数
#include "GLTools.h"
#include <glut/glut.h>



//帮助类:
//定义一个，着色管理器
GLShaderManager shaderManager;
//简单的批次容器，是GLTools的一个简单的容器类
GLBatch triangleBatch;

//blockSize 边长
GLfloat blockSize = 0.1f; //是边长的一半

//正方形的4个点坐标
GLfloat vVerts[] = {
    -blockSize,-blockSize,0.0f,//A
    blockSize,-blockSize,0.0f,//B
    blockSize,blockSize,0.0f,//C
    -blockSize,blockSize,0.0f //D
};


//changeSize 函数：自定义函数，通过glutReshaperFunc(函数名)注册为重塑函数，当屏幕大小发生变化或者第一次创建窗口时，会调用该函数调整窗口大小/视口大小
void changeSize(int w,int h){
    glViewport(0, 0, w, h);//窗口大小改变时接口新的宽度和高度，其中0,0代表窗口中视口的左下角坐标，w，h代表像素
}

//renderScene 函数：自定义函数，通过glutReshaperFunc(函数名)注册为重塑函数，当屏幕发生变化/或者开发者主动渲染会调用此函数，用来实现数据->渲染过程
void renderScene(void){
    //清除一个或一组特定的缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    //设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0f,0.0f,0.0f,1.0f};
    
    //传递到存储着色器，即GLT_SHAPDER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    
    //提交着色器
    triangleBatch.Draw();
    
    //将在后台缓冲区进行渲染，然后再结束时交换到前台
    glutSwapBuffers();
    
}

//setupRC 函数：自定义函数，设置你需要渲染图形的相关顶点数据/颜色数据等数据装备工作
void setupRC(){
    //设置背景颜色
    glClearColor(0.98f, 0.40f, 0.7f, 1);
    //初始化着色管理器
    shaderManager.InitializeStockShaders();
    //设置三角形，其中数组vVert包含所有3个顶点的x、y笛卡尔坐标系对
//    GLfloat vVerts[] = {
//        -0.5f,0.0f,0.0f,
//        0.5f,0.0f,0.0f,
//        0.0f,0.5f,0.0f,
//    };
//
    //批次处理
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}


void SpecialKeys(int key,int x,int y){
    GLfloat stepSize = 0.025f; //移动步长
    //blockX 和 blockY代表了移动顶点D的坐标
    GLfloat blockX = vVerts[0];
    GLfloat blockY = vVerts[10];
    
    printf("v[0] = %f\n",blockX);
    printf("v[10] = %f\n",blockY);
    
    if (key == GLUT_KEY_UP) blockY += stepSize;
    if (key == GLUT_KEY_DOWN) blockY -= stepSize;
    if (key == GLUT_KEY_LEFT) blockX -= stepSize;
    if (key == GLUT_KEY_RIGHT) blockX += stepSize;
    
    
    //边界处理
    //当正方形移动超过最左边的时候
    if (blockX < -1.0f) blockX = -1.0f;
    //当正方形移动到最右边的时候
    //1.0 - blockSize * 2 = 总边长 - 正方形的边长 = 最左边点的位置
    if (blockX > (1.0 - blockSize * 2)) blockX = 1.0f - blockSize * 2;
    
    //当正方形移动到最下面的时候
    //-1.0f - blockSize * 2 = Y(负轴边界)
    if (blockY < -1.0f + blockSize * 2) blockY = -1.0f + blockSize * 2;
    
    //当正方形移动到最上面的时候
    if (blockY > 1.0f) blockY = 1.0f;
    
    printf("blockX = %f\n",blockX);
    printf("blockY = %f\n",blockY);
    
    //更行顶点坐标ABCD
    vVerts[0] = blockX;
    vVerts[1] = blockY - blockSize * 2;
    
    printf("(%f,%f)\n",vVerts[0],vVerts[1]);
    
    vVerts[3] = blockX + blockSize * 2;
    vVerts[4] = blockY - blockSize * 2;
    
    printf("(%f,%f)\n",vVerts[3],vVerts[4]);
    
    vVerts[6] = blockX + blockSize * 2;
    vVerts[7] = blockY;
    
    printf("(%f,%f)\n",vVerts[6],vVerts[7]);
    
    vVerts[9] = blockX;
    vVerts[10] = blockY;
    printf("(%f,%f)\n",vVerts[9],vVerts[10]);
    
    triangleBatch.CopyVertexData3f(vVerts);
    //手动触发重新渲染
    glutPostRedisplay();
    
}

//main 函数：程序入口
int main(int argc,char* argv[])
{
    //设置当前工作目录，针对MAC OS X
    gltSetWorkingDirectory(argv[0]);
    //初始化GLUT库
    glutInit(&argc, argv);
    
    //初始化双缓冲区窗口，其中标致GLUT_DOUBLE,GLUT_RGBA,GLUT_DEPTH,GLUT_STENCIL 分别指双缓冲区窗口，RGBA颜色模式，深度测试，模板缓冲区
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("Triangle");
    //注册回调函数
    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    //注册特殊键位响应
    glutSpecialFunc(SpecialKeys);
    //驱动程序初始化
    GLenum err = glewInit();
    
    if(GLEW_OK != err) {
        
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        
        return 1;
        
    }
    //调用SetupRC
    
    setupRC();
    
    glutMainLoop();
    
    return 0;
}
