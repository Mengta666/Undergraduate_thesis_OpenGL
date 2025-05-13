#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glut/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>
#include "Model.h"

#include <iostream>
#include <fstream>
#include <string>

#include "Utils.h"
#include "VertexBuffer.h"                                                               // 顶点缓冲区绑定
#include "IndexBuffer.h"                                                                // 索引缓冲区绑定
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"                                                                    // 纹理
#include "DrawShader.h" 
#include "KeyBoard.h"
#include "ShadowMap.h"
#include "OmnidirectionalShadowMaps.h"



using namespace std;

#define GLSL_VERSION "#version 430"

void frameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

/*初始窗口大小*/
float SCR_WIDTH = 1280;
float SCR_HEIGHT = 720;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

/*透视，观察，模型矩阵*/
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::vec3 mypos = glm::vec3(0.0f);
glm::mat4 rot1 = glm::mat4(1.0f);

/*LOOKAT相机*/
KeyBoard keyboard;
glm::vec3 cameraPos = glm::vec3(0.0, 1.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

/* Light option */
float pointLightPos[3] = { 1.0f, 5.0f, 0.0f };
float directLightPos[3] = { -1.6f, 2.0f, -0.65f };
float pointLightColor[3] = { 1.0f, 1.0f, 1.0f };

/* timespeed */
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

/* mouse_callback */
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool first_mouse = true;
float currentFrame;


int main(void)
{
    GLFWwindow* window;

    float position[] = {
        /* TOP */ /*POSITON*/       /* NORMALS*/               /*TEXCOORD*/
        -0.5f, -0.5f,  0.5f,          0.0f,  0.0f,  1.0f,        0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,          0.0f,  0.0f,  1.0f,        1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,          0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,          0.0f,  0.0f,  1.0f,        1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,          0.0f,  0.0f,  1.0f,        0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,          0.0f,  0.0f,  1.0f,        0.0f, 0.0f,
        /* BOTTON */
        -0.5f, -0.5f, -0.5f,          0.0f,  0.0f, -1.0f,        0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,          0.0f,  0.0f, -1.0f,        1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,          0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,          0.0f,  0.0f, -1.0f,        1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,          0.0f,  0.0f, -1.0f,        0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,          0.0f,  0.0f, -1.0f,        0.0f, 0.0f,

        /* FRONT */
        -0.5f, -0.5f, -0.5f,          0.0f, -1.0f,  0.0f,        0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,          0.0f, -1.0f,  0.0f,        1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,          0.0f, -1.0f,  0.0f,        1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,          0.0f, -1.0f,  0.0f,        1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,          0.0f, -1.0f,  0.0f,        0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,          0.0f, -1.0f,  0.0f,        0.0f, 0.0f,

        /* BACK */
        -0.5f,  0.5f, -0.5f,          0.0f,  1.0f,  0.0f,        0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,          0.0f,  1.0f,  0.0f,        1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,          0.0f,  1.0f,  0.0f,        1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,          0.0f,  1.0f,  0.0f,        1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,          0.0f,  1.0f,  0.0f,        0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,          0.0f,  1.0f,  0.0f,        0.0f, 0.0f,

        /* LEFT */
        0.5f, -0.5f, -0.5f,          1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,          1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,          1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,          1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,          1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,          1.0f,  0.0f,  0.0f,        0.0f, 0.0f,

        /* RIGHT */
       -0.5f, -0.5f, -0.5f,         -1.0f,  0.0f,  0.0f,        0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,         -1.0f,  0.0f,  0.0f,        1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,         -1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,         -1.0f,  0.0f,  0.0f,        1.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,         -1.0f,  0.0f,  0.0f,        0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,         -1.0f,  0.0f,  0.0f,        0.0f, 0.0f,

    };


    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::vec3 pointlightPosition[] =
    {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.7f, -2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f),
    };


    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);                      // 以核心库进行运行，而不是以兼容库，默认为兼容库

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLShader", NULL, NULL);
    glfwSwapInterval(1);                                                                // 类似于垂直同步

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallBack);                             // 屏幕大小重设
    glfwSetScrollCallback(window, scroll_callback);                                             // 滚轮捕捉
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);                                                                           //鼠标位移监测


//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);                                // 隐藏鼠标
    keyboard.setWindow(window);
    keyboard.setPFU(cameraPos, cameraFront, cameraUp);

    /* ImGui */
    //Setup dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //Setup style
    ImGui::StyleColorsDark();
    //Setup the Platform(平台)
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
    /* GUI Variable */
    bool open_PointLight = true;
    bool open_SpotLight = true;
    bool open_DirLight = true;
    bool model_option = false;
    bool light_option = false;
    float light_diffuse[3] = { 0.5f, 0.5f, 0.5f };
    float light_ambient[3] = { 0.2f, 0.2f, 0.2f };
    float light_specular[3] = { 1.0f, 1.0f, 1.0f };
    float Shininess = 32.0f;
    float model_move[3] = { 0.0f, 0.0f, 0.0f };


    if (glewInit() != GLEW_OK)
    {
        std::cout << "glew Init error!" << std::endl;
        return -1;
    }                                                                                           //必须将glew初始化


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                                  // 混合
    glEnable(GL_BLEND);



    /* model */
    Model myBackpack("Object/BackPack/backpack.obj");
    Model sponza("Object/Sponza1/sponza.obj");
    //Model myTree("Object/Trees/Tree.obj");
    Model TheTree("Object/obj/OBJ_AS01_BambusaVulgaris_6.obj");
    Model nanosuit("Object/nanosuit/nanosuit.obj");                                                    // 绘制该模型时需要关闭混合

//    DrawShader dsCube = DrawShader("Shaders/vertexShader.glsl", "Shaders/new_frag.glsl");

    DrawShader dsModel = DrawShader("Shaders/model_vert.glsl", "Shaders/model_frag.glsl");

    DrawShader dsShadow = DrawShader("Shaders/shadow_map_vert.glsl", "Shaders/shadow_map_frag.glsl");               // 阴影绘制

    DrawShader dsOMNShadow = DrawShader("Shaders/OmnidirectionalShadowMapsVert.glsl", "Shaders/OmnidirectionalShadowMapsFrag.glsl", "Shaders/OmnidirectionalShadowMapsGeom.glsl");

    /* light */
    /* Light Bind Buffer */
    VertexBuffer vb(position, sizeof(position));
    vb.bind();
    VertexBufferLayout LayoutLight;
    LayoutLight.Push<float>(3);
    LayoutLight.Push<float>(3);
    LayoutLight.Push<float>(2);
    VertexArray vaoLight;
    vaoLight.addBuffer(vb, LayoutLight);
    vaoLight.bind();
    DrawShader dsLight = DrawShader("Shaders/pointLightVert.glsl", "Shaders/pointLightFrag.glsl");
    dsLight.bind();

    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO, planeVAO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    Texture texture0("textures/container2.png");
    texture0.bind(0);

    // 生成阴影贴图
    // 普通
    ShadowMap shadowMap = ShadowMap();
    shadowMap.creatShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
    // 全向
    OmnidirectionalShadowMaps OMNShadowMap = OmnidirectionalShadowMaps();
    OMNShadowMap.creatOmnidirectionalShadowMaps(SHADOW_WIDTH, SHADOW_HEIGHT);

    // 设置插槽,将阴影纹理绑定在最后两个插槽，有效避免前面的插槽被模型占用。
    dsModel.bind();
    dsModel.setUniform1i("directionalShadowMap", 30);
    dsModel.setUniform1i("depthMap", 31);


    float radius = 2.0f;
    float comX;
    float comZ;

    std::cout << "当前OpenGL以及驱动版本： " << glGetString(GL_VERSION) << std::endl;                                                //获得OpenGL版号

    /* 阴影贴图 */
    float near_plane = 0.1f, far_plane = 75.0f;
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;


    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // 绘制
        Utils::Clear();     //清屏


        comX = -fabs(cos(glfwGetTime()) * radius);
        comZ = -fabs(sin(glfwGetTime()) * radius);

        /* Start new frame for Dear ImGui */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* 计算每帧之间的时间间隔 */
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        keyboard.userInput(window, deltaTime);                                                              // 按键检测

        // 1.绘制全向阴影贴图
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms = OMNShadowMap.creatShadowTransforms(shadowProj, Utils::arrayToVec3(pointLightPos));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        OMNShadowMap.bindOSWFramebuffer();
        dsOMNShadow.bind();
        for (unsigned int i = 0; i < 6; ++i)
            dsOMNShadow.setUniformMat4f(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        dsOMNShadow.setUniform1f("far_plane", far_plane);
        dsOMNShadow.setUniformVec3("lightPos", Utils::arrayToVec3(pointLightPos));
        model = glm::mat4(1.0f);
        dsOMNShadow.setUniformMat4f("model", model);
        //glBindVertexArray(planeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        sponza.Draw(dsOMNShadow);
        model = glm::translate(model, Utils::arrayToVec3(model_move));
        model = glm::scale(model, glm::vec3(0.01f));
        dsOMNShadow.setUniformMat4f("model", model);
        TheTree.Draw(dsOMNShadow);
        OMNShadowMap.unBindOSWFramebuffer();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1. 绘制阴影贴图，以平行光的视角进行绘制，没有输出（片段着色器无内容）
        // --------------------------------------------------------------
        
        lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        lightView = glm::lookAt(Utils::arrayToVec3(directLightPos), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        dsShadow.bind();
        dsShadow.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowMap.bindFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);
        model = glm::mat4(1.0f);
        dsShadow.setUniformMat4f("model", model);
        //glBindVertexArray(planeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        sponza.Draw(dsShadow);
        model = glm::translate(model, Utils::arrayToVec3(model_move));
                model = glm::scale(model, glm::vec3(0.01f));
        dsShadow.setUniformMat4f("model", model);
        TheTree.Draw(dsShadow);
        shadowMap.unBindFramebuffer();

        // 2.重设视角,进行物体绘制
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        projection = glm::perspective(glm::radians(keyboard.getFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = keyboard.lookAt();

        //draw Model
        dsModel.bind();
        dsModel.setUniformMat4f("view", view);
        dsModel.setUniformMat4f("projection", projection);
        model = glm::mat4(1.0f);
        dsModel.setUniformMat4f("model", model);

        cameraPos = keyboard.getCameraPos();
        cameraFront = keyboard.getCameraFront();

        dsModel.setUniformBoolen("open_PointLight", open_PointLight);
        dsModel.setUniformBoolen("open_SpotLight", open_SpotLight);
        dsModel.setUniformBoolen("open_DirLight", open_DirLight);
        /* Model direction Light */
        dsModel.setUniformVec3("lightPos", Utils::arrayToVec3(directLightPos));
        //Model point light Option
        dsModel.setUniformVec3("PointlightPos", Utils::arrayToVec3(pointLightPos));
        dsModel.setUniformVec3("viewPos", cameraPos);

        dsModel.setUniform1f("material.Shininess", Shininess);

        dsModel.setUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);

        shadowMap.bindTexture(30);                                           // 真正的阴影绘制
        OMNShadowMap.bindOSWTexture(31);

        //texture0.bind(0);
        //glBindVertexArray(planeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        sponza.Draw(dsModel);
        model = glm::mat4(1.0f);
        model = glm::translate(model, Utils::arrayToVec3(model_move));
               model = glm::scale(model, glm::vec3(0.01f));
        dsModel.setUniformMat4f("model", model);
        dsModel.setUniform1f("far_plane", far_plane);
        TheTree.Draw(dsModel);

        //Draw light
        dsLight.bind();
        model = glm::mat4(1.0f);
        model = glm::translate(model, Utils::arrayToVec3(pointLightPos));
        model = glm::scale(model, glm::vec3(0.1f));
        dsLight.setUniformMat4f("projection", projection);
        dsLight.setUniformMat4f("view", view);
        dsLight.setUniformMat4f("model", model);
        dsLight.setUniformVec3("lightColor", Utils::arrayToVec3(pointLightColor));
        vaoLight.bind();
        dsLight.display(36);


        /* ImGui */
        ImGui::Begin("Model and Light");
        ImGui::Text("This is Light Option");
        ImGui::Checkbox("Open The PointLight", &open_PointLight);
        ImGui::SameLine();
        ImGui::Checkbox("Open The SpotLight", &open_SpotLight);
        ImGui::Checkbox("Open The DirectLight", &open_DirLight);
        ImGui::SameLine();
        ImGui::Checkbox("Light Option", &light_option);
        ImGui::Text("This is Model Option");
        ImGui::Checkbox("Model Option", &model_option);
        ImGui::End();
        if (model_option)
        {
            ImGui::Begin("Model Option");
            ImGui::SliderFloat("The Model Shininess", &Shininess, 0.01f, 1024.0f);
            ImGui::DragFloat3("The Bamboo Model Move", model_move, 0.001, -20.0f, 20.0f);
            ImGui::End();
        }

        if (light_option)
        {
            ImGui::Begin("Light Option");
            ImGui::Text("The Point Light");
            ImGui::ColorEdit3("The Point Light Color", pointLightColor);
            ImGui::DragFloat3("The Point Light Move", pointLightPos, 0.01f, -20.0f, 20.0f);
            ImGui::Text("The Directional Light");
            ImGui::DragFloat3("The directional Light Move", directLightPos, 0.01f, -20.0f, 20.0f);
            ImGui::End();
        }


        //render to screen;
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

    }
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();                                                                                                    //关闭上下文
    return 0;
}


void frameBufferSizeCallBack(GLFWwindow* window, int width, int height)                                                 // 窗口检测
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    if (SCR_HEIGHT == 0)
    {
        SCR_HEIGHT = 1.0f / 1000000;                                                                                          //防止分母为0
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)                                      // 按键检测
{

    keyboard.key_rotate(window, key, scancode, action, mode, rot1);

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_TRUE)
    {
        keyboard.mouse_callback(window, xoffset, yoffset);
    }

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    keyboard.scroll_callback(window, xoffset, yoffset);
}












