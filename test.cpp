#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>
#include "Model.h"
#include "Animator.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Utils.h"
#include "VertexBuffer.h"                                                               // ���㻺������
#include "IndexBuffer.h"                                                                // ������������
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Texture.h"                                                                    // ����
#include "DrawShader.h" 
#include "KeyBoard.h"
#include "ShadowMap.h"
#include "OmnidirectionalShadowMaps.h"
#include "SkyBox.h"
#include "Particle.h"


using namespace std;

#define GLSL_VERSION "#version 430"

void frameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setShaderUniform(DrawShader& shader);

/*��ʼ���ڴ�С*/
float SCR_WIDTH = 1280;
float SCR_HEIGHT = 720;

const unsigned int particleNumber = 40000;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

/*͸�ӣ��۲죬ģ�;���*/
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::vec3 mypos = glm::vec3(0.0f);
glm::mat4 rot1 = glm::mat4(1.0f);

/*LOOKAT���*/
KeyBoard keyboard;
glm::vec3 cameraPos = glm::vec3(0.0, 1.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

/* Light option */
float spotLightPos[3] = { 15.0f, 2.5f, -0.06f };
float spotLightDir[3] = { -5.32f, -0.2f, 0.07f };
float spotLightColor[3] = { 1.0f, 1.0f, 1.0f };
float directLightPos[3] = { 0.5f, 25.0f, 0.3f };
float pointLightPos[3] = { 0.0f, 5.0f, 0.0f };
float pointLightColor[3] = { 1.0f, 1.0f, 1.0f };

/* timespeed */
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

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
        glm::vec3(14.0f,  8.5f,  6.0f),
        glm::vec3(14.0f,  8.5f,  -6.0f),
        glm::vec3(-15.0f,  8.5f,  -6.0f),
        glm::vec3(-15.0f,  8.5f,  6.0f),
    };

    // ��պ�·��
    std::vector<std::string> faces
    {
        "SkyBox/new1/right.png",
        "SkyBox/new1/left.png",
        "SkyBox/new1/top.png",
        "SkyBox/new1/bottom.png",
        "SkyBox/new1/front.png",
        "SkyBox/new1/back.png",
    };


    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);                      // �Ժ��Ŀ�������У��������Լ��ݿ⣬Ĭ��Ϊ���ݿ�

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL 3D Scene", NULL, NULL);
    glfwSwapInterval(1);                                                                // �����ڴ�ֱͬ��

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallBack);                             // ��Ļ��С����
    glfwSetScrollCallback(window, scroll_callback);                                             // ���ֲ�׽
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);                                             //���λ�Ƽ��


//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);                                // �������
    keyboard.setWindow(window);
    keyboard.setPFU(cameraPos, cameraFront, cameraUp);

    /* ImGui */
    //Setup dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //Setup style
    ImGui::StyleColorsDark();
    //Setup the Platform(ƽ̨)
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
    /* GUI Variable */
    bool open_PointLight = true;
    bool open_SpotLight = false;
    bool open_DirLight = true;
    bool model_option = false;
    bool light_option = false;
    bool gamma_correction = false;
    bool open_particle = false;
    float Shininess = 32.0f;
    float model_move[3] = { 0.0f, 0.0f, 0.0f };



    if (glewInit() != GLEW_OK)
    {
        std::cout << "glew Init error!" << std::endl;
        return -1;
    }                                                                                           //���뽫glew��ʼ��


    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                                  // ���
    glEnable(GL_BLEND);



    /* model OBJ */
    Model myBackpack("Object/BackPack/backpack.obj");
    Model sponza("Object/Sponza1/sponza.obj");
    //Model myTree("Object/Trees/Tree.obj");
    Model TheTree("Object/obj/OBJ_AS01_BambusaVulgaris_4.obj");
//    Model Animation("Object/Wraith/Wraith.obj");
    Model nanosuit("Object/nanosuit/nanosuit.obj");                                                    // ���Ƹ�ģ��ʱ��Ҫ�رջ��

    Model_Animation m_Animation("Object/2/Northern_Soul_Dance.fbx");
    Animation walkAnimation(&m_Animation, "Object/2/Northern_Soul_Dance.fbx");
    //Model_Animation m_Animation("Object/2/Cross_Jumps_Rotation.fbx");
    //Animation walkAnimation(&m_Animation, "Object/2/Cross_Jumps_Rotation.fbx");
    Animator animator(&walkAnimation);

    // ģ����ɫ��
    DrawShader dsModel = DrawShader("Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl");
    // ƽ�й���Ӱ
    DrawShader dsShadow = DrawShader("Shaders/shadow_map_vert.glsl", "Shaders/shadow_map_frag.glsl"); 
    // �۹���Ӱ
    DrawShader dsSpotShadow = DrawShader("Shaders/shadow_map_vert.glsl", "Shaders/shadow_map_frag.glsl");
    // ���Դ��Ӱ
    DrawShader dsOMNShadow = DrawShader("Shaders/OmnidirectionalShadowMapsVert.glsl", "Shaders/OmnidirectionalShadowMapsFrag.glsl", "Shaders/OmnidirectionalShadowMapsGeom.glsl");
    // ��պ�
    DrawShader dsSkyBox = DrawShader("Shaders/SkyBox_vert.glsl", "Shaders/SkyBox_frag.glsl");
    // ��������
    DrawShader dsAnimation = DrawShader("Shaders/AnimaionVert.glsl", "Shaders/AnimaionFrag.glsl");
    // ����Ч��
    DrawShader dsParticle = DrawShader("Shaders/ParticleVert.glsl", "Shaders/ParticleFrag.glsl");

    /* Particle */
    Particle particle = Particle(particleNumber, Utils::arrayToVec3(pointLightPos));

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


    // ������Ӱ��ͼ
    // ��ͨ
    ShadowMap spotShadowMap = ShadowMap();
    spotShadowMap.creatShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
    ShadowMap shadowMap = ShadowMap();
    shadowMap.creatShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
    // ȫ��
    OmnidirectionalShadowMaps OMNShadowMap = OmnidirectionalShadowMaps();
    OMNShadowMap.creatOmnidirectionalShadowMaps(SHADOW_WIDTH, SHADOW_HEIGHT);
    // ���ڶ�����ƽ�й���Ӱ��ͼ
    ShadowMap animationShadowMap = ShadowMap();
    animationShadowMap.creatShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);

    SkyBox skyBox(faces, true);

    // ���ò��,����Ӱ����������������ۣ���Ч����ǰ��Ĳ�۱�ģ��ռ�á�
    dsSkyBox.bind();
    dsSkyBox.setUniform1i("skyBoxTexture", 28);
    dsModel.bind();
    dsModel.setUniform1i("spotShadowMap", 29);
    dsModel.setUniform1i("directionalShadowMap", 30);
    dsModel.setUniform1i("depthMap", 31);
    dsAnimation.bind();
    dsAnimation.setUniform1i("directionalShadowMap", 30);
    dsAnimation.setUniform1i("depthMap", 31);


    float radius = 2.0f;
    float comX;
    float comZ;

    std::cout << "��ǰOpenGL�Լ������汾�� " << glGetString(GL_VERSION) << std::endl;                                                //���OpenGL���

    /* ��Ӱ��ͼ���� */
    float near_plane = 0.1f, far_plane = 100.0f;
    glm::mat4 lightProjection, lightView;
    glm::mat4 directionLightSpaceMatrix, spotLightSpaceMatrix;

    currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    animator.UpdateAnimation(deltaTime);
    auto transforms = animator.GetFinalBoneMatrices();

    while (!glfwWindowShouldClose(window))
    {
        /* �����괰�ڱ仯 */
        glfwPollEvents();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // ����
        Utils::Clear();     //����


        comX = -fabs(cos(glfwGetTime()) * radius);
        comZ = -fabs(sin(glfwGetTime()) * radius);

        projection = glm::perspective(glm::radians(keyboard.getFOV()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = keyboard.lookAt();

        /* Start new frame for ImGui */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* ����ÿ֮֡���ʱ���� */
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        keyboard.userInput(window, deltaTime);                                                              // �������

        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            animator.UpdateAnimation(deltaTime);                                                  // ���¶�������һ֡�˶�����
            transforms = animator.GetFinalBoneMatrices();
        }

        // 1.1����ȫ����Ӱ��ͼ
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms = OMNShadowMap.creatShadowTransforms(shadowProj, Utils::arrayToVec3(pointLightPos));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        OMNShadowMap.bindOSWFramebuffer();
        dsOMNShadow.bind();
        for (unsigned int i = 0; i < 6; ++i)
            dsOMNShadow.setUniformMat4f(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        dsOMNShadow.setUniform1f("far_plane", far_plane);
        dsOMNShadow.setUniformVec3("lightPos", Utils::arrayToVec3(pointLightPos));

        dsOMNShadow.setUniform1i("AnimationShadow", false);
        model = glm::mat4(1.0f);
        dsOMNShadow.setUniformMat4f("model", model);
        sponza.Draw(dsOMNShadow);
        model = glm::translate(model, Utils::arrayToVec3(model_move));
        model = glm::scale(model, glm::vec3(0.01f));
        dsOMNShadow.setUniformMat4f("model", model);
        TheTree.Draw(dsOMNShadow);

        dsOMNShadow.setUniform1i("AnimationShadow", true);
        for (int i = 0; i < transforms.size(); ++i)
            dsOMNShadow.setUniformMat4f("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 0.1f, 1.0f));
        model = glm::scale(model, glm::vec3(.008f, .008f, .008f));
        dsOMNShadow.setUniformMat4f("model", model);
        m_Animation.Draw(dsOMNShadow);

        OMNShadowMap.unBindOSWFramebuffer();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1.2 ������Ӱ��ͼ����ƽ�й���ӽǽ��л��ƣ�û�������Ƭ����ɫ�������ݣ�
        // --------------------------------------------------------------
        glCullFace(GL_FRONT);
        lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);
        lightView = glm::lookAt(Utils::arrayToVec3(directLightPos), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        directionLightSpaceMatrix = lightProjection * lightView;

        dsShadow.bind();
        dsShadow.setUniformMat4f("lightSpaceMatrix", directionLightSpaceMatrix);
        dsShadow.setUniform1i("AnimationShadow", false);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        shadowMap.bindFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);
        model = glm::mat4(1.0f);
        dsShadow.setUniformMat4f("model", model);
        sponza.Draw(dsShadow);
        model = glm::translate(model, Utils::arrayToVec3(model_move));
        model = glm::scale(model, glm::vec3(0.01f));
        dsShadow.setUniformMat4f("model", model);
        TheTree.Draw(dsShadow);

        dsShadow.setUniform1i("AnimationShadow", true);
        for (int i = 0; i < transforms.size(); ++i)
            dsShadow.setUniformMat4f("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 0.1f, 1.0f));
        model = glm::scale(model, glm::vec3(.008f, .008f, .008f));
        dsShadow.setUniformMat4f("model", model);
        m_Animation.Draw(dsShadow);

        shadowMap.unBindFramebuffer();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 1.3 ������Ӱ��ͼ���Ծ۹�Ƶ��ӽǽ��л���
        // --------------------------------------------------------------
        lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
        lightView = glm::lookAt(Utils::arrayToVec3(spotLightPos), Utils::arrayToVec3(spotLightDir), glm::vec3(0.0, 1.0, 0.0));
        spotLightSpaceMatrix = lightProjection * lightView;

        dsSpotShadow.bind();
        dsSpotShadow.setUniformMat4f("lightSpaceMatrix", spotLightSpaceMatrix);
        dsSpotShadow.setUniform1i("AnimationShadow", false);                                    //δ���ö����ľ۹����Ӱ

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        spotShadowMap.bindFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);
        model = glm::mat4(1.0f);
        dsSpotShadow.setUniformMat4f("model", model);
        sponza.Draw(dsSpotShadow);
        model = glm::translate(model, Utils::arrayToVec3(model_move));
        model = glm::scale(model, glm::vec3(0.01f));
        dsSpotShadow.setUniformMat4f("model", model);
        TheTree.Draw(dsSpotShadow);

        dsSpotShadow.setUniform1i("AnimationShadow", true);
        for (int i = 0; i < transforms.size(); ++i)
            dsSpotShadow.setUniformMat4f("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 0.1f, 1.0f));
        model = glm::scale(model, glm::vec3(.008f, .008f, .008f));
        dsSpotShadow.setUniformMat4f("model", model);
        m_Animation.Draw(dsSpotShadow);

        spotShadowMap.unBindFramebuffer();
        glCullFace(GL_BACK);

        // 2.�����ӽ�,�����������
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


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
        dsModel.setUniformBoolen("gamma_correction", gamma_correction);

        setShaderUniform(dsModel);

        dsModel.setUniform1f("material.Shininess", Shininess);
        dsModel.setUniform1f("far_plane", far_plane);
        dsModel.setUniform1f("near_plane", near_plane);

        dsModel.setUniformMat4f("directionLightSpaceMatrix", directionLightSpaceMatrix);
        dsModel.setUniformMat4f("spotLightSpaceMatrix", spotLightSpaceMatrix);

        spotShadowMap.bindTexture(29);
        shadowMap.bindTexture(30);                                           // ��������Ӱ����
        OMNShadowMap.bindOSWTexture(31);

        sponza.Draw(dsModel);
        model = glm::mat4(1.0f);
        model = glm::translate(model, Utils::arrayToVec3(model_move));
        model = glm::scale(model, glm::vec3(0.01f));
        dsModel.setUniformMat4f("model", model);
        
        TheTree.Draw(dsModel);

        /* ���ƹ������� */
        glDisable(GL_BLEND);
        dsAnimation.bind();
        dsAnimation.setUniformMat4f("projection", projection);
        dsAnimation.setUniformMat4f("view", view);
        for (int i = 0; i < transforms.size(); ++i)
            dsAnimation.setUniformMat4f("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        dsAnimation.setUniformBoolen("open_PointLight", open_PointLight);
        dsAnimation.setUniformBoolen("open_SpotLight", open_SpotLight);
        dsAnimation.setUniformBoolen("open_DirLight", open_DirLight);
        dsAnimation.setUniformBoolen("gamma_correction", gamma_correction);

        setShaderUniform(dsAnimation);
        
        dsAnimation.setUniform1f("material.Shininess", Shininess);
        dsAnimation.setUniformMat4f("directionLightSpaceMatrix", directionLightSpaceMatrix);

        //animationShadowMap.bindTexture(30);
        //OMNShadowMap.bindOSWTexture(31);  

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 0.1f, 1.0f)); 
        model = glm::scale(model, glm::vec3(.008f, .008f, .008f));
        dsAnimation.setUniformMat4f("model", model);
        dsAnimation.setUniform1f("far_plane", far_plane);
        m_Animation.Draw(dsAnimation);

        glEnable(GL_BLEND);

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


        // Draw SkyBox;
        dsSkyBox.bind();
        skyBox.bind(28);
        dsSkyBox.setUniformMat4f("view", glm::mat4(glm::mat3(view)));       // ֻҪ�����ת���ŵĲ��֣���Ҫƽ�ƣ�������ƶ�ʱ��Ӱ����պ�λ��
        dsSkyBox.setUniformMat4f("projection", projection);
        dsSkyBox.display(36);



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
        ImGui::Text("Other Option");
        ImGui::Checkbox("Open the particle", &open_particle);
        ImGui::SameLine();
        ImGui::Checkbox("Gamma Correction", &gamma_correction);
        ImGui::Text("Other Info");
        ImGui::Text("Camera Position: X: %f, Y: %f, Z: %f", keyboard.getCameraPos().x, keyboard.getCameraPos().y, keyboard.getCameraPos().z);
        ImGui::Text("Application average %.1f FPS", io.Framerate);
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
            ImGui::Text("The Spot Light");
            ImGui::DragFloat3("The Spot Light Pos Move", spotLightPos, 0.01f, -20.0f, 20.0f);
            ImGui::DragFloat3("The Spot Light Dir Move", spotLightDir, 0.01f, -20.0f, 20.0f);
            ImGui::ColorEdit3("The Spot Light Color", spotLightColor);
            ImGui::End();
        }

        if (open_particle)
        {
            dsParticle.bind();
            dsParticle.setUniformMat4f("projection", projection);
            dsParticle.setUniformMat4f("view", view);
            model = glm::mat4(1.0f);
            model = glm::rotate(model, deltaTime * 50.0f, Utils::arrayToVec3(pointLightPos));
            dsParticle.setUniformMat4f("model", model);
            particle.setDelteTime(deltaTime);
            particle.Update(deltaTime, particleNumber * deltaTime);
            particle.Draw(dsParticle);
        }

        //render to screen;
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);                            // ������Ļ��������δ��ʾ����Ļ�ϵĻ�����

    }
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();                                                                                                    //�ر�������
    return 0;
}


void frameBufferSizeCallBack(GLFWwindow* window, int width, int height)                                                 // ���ڼ��
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    if (SCR_HEIGHT == 0)
    {
        SCR_HEIGHT = 1.0f / 1000000;                                                                                          //��ֹ��ĸΪ0
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)                                      // �������
{

    keyboard.key_rotate(window, key, scancode, action, mode, rot1);

}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);        // ǿ������ת����������ȫ���
    float ypos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // ��ȡǰһ�κͱ��ε����ƫ����

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


// ����ĺ���

void setShaderUniform(DrawShader& shader)
{
    /* Model direction Light */
    shader.setUniformVec3("dirLight.direction", Utils::arrayToVec3(directLightPos));
    shader.setUniformVec3("dirLight.ambient", glm::vec3(0.2f));
    shader.setUniformVec3("dirLight.diffuse", glm::vec3(0.5f));
    shader.setUniformVec3("dirLight.specular", glm::vec3(0.8f));

    //Model point light Option
    shader.setUniformVec3("pointLight.position", Utils::arrayToVec3(pointLightPos));
    shader.setUniformVec3("pointLight.color", Utils::arrayToVec3(pointLightColor));
    shader.setUniformVec3("pointLight.ambient", glm::vec3(0.2f));
    shader.setUniformVec3("pointLight.diffuse", glm::vec3(0.5f));
    shader.setUniformVec3("pointLight.specular", glm::vec3(0.8f));
    shader.setUniformVec3("viewPos", cameraPos);
    shader.setUniform1f("pointLight.constant", 1.0f);
    shader.setUniform1f("pointLight.linear", 0.09f);
    shader.setUniform1f("pointLight.quadratic", 0.032f);

    // SPOT LIGHT
    shader.setUniformVec3("spotLight.position", Utils::arrayToVec3(spotLightPos));
    shader.setUniformVec3("spotLight.direction", Utils::arrayToVec3(spotLightDir));
    shader.setUniformVec3("spotLight.color", Utils::arrayToVec3(spotLightColor));
    shader.setUniformVec3("spotLight.ambient", glm::vec3(0.2f));
    shader.setUniformVec3("spotLight.diffuse", glm::vec3(0.5f));
    shader.setUniformVec3("spotLight.specular", glm::vec3(0.8f));
    shader.setUniform1f("spotLight.constant", 1.0f);
    shader.setUniform1f("spotLight.linear", 0.09f);
    shader.setUniform1f("spotLight.quadratic", 0.032f);
    shader.setUniform1f("spotLight.cutoff", glm::cos(glm::radians(13.0f)));
    shader.setUniform1f("spotLight.outerCutoff", glm::cos(glm::radians(16.0f)));
}

