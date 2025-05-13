#include <iostream>

#include "KeyBoard.h"



void KeyBoard::setWindow(GLFWwindow* window)
{
    m_window = window;
    cameraPos = glm::vec3(0.0, 0.0, 3.0);
    cameraFront = glm::vec3(0.0, 0.0, -1.0);
    cameraUp = glm::vec3(0.0, 1.0, 0.0);
    up = glm::vec3( 0.0f, 0.0f, 0.0f );
    right = glm::vec3(0.0f, 0.0f, 0.0f);
    front = glm::vec3(0.0f, 0.0f, 0.0f);
}

void KeyBoard::setPFU(glm::vec3 Position, glm::vec3 Front, glm::vec3 Up)
{
    cameraPos = Position;
    cameraFront = Front;
    cameraUp = Up;

    updateCameraVectors();
}



glm::mat4 KeyBoard::lookAt()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, up);                             //cameraPos + cameraFront 保证无论怎么移动，摄像机都会注视着目标方向。
}

void KeyBoard::userInput(GLFWwindow* window, float deltaTime)                                                        // 按键检测（相机移动）
{
    float camera_Speed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraFront * camera_Speed;
        std::cout << "已按下 W 键\n";
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraFront * camera_Speed;
        std::cout << "已按下 S 键\n";
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * camera_Speed;
        std::cout << "已按下 D 键\n";
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * camera_Speed;
        std::cout << "已按下 A 键\n";
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        cameraPos += cameraUp * camera_Speed;
        std::cout << "已按下 up 键\n";
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        cameraPos -= cameraUp * camera_Speed;
        std::cout << "已按下 down 键\n";
    }
}

void KeyBoard::key_rotate(GLFWwindow* window, int key, int scancode, int action, int mode, glm::mat4& rot1)                        // 按键检测（物体旋转）
{
    if (action == GLFW_PRESS && key == GLFW_KEY_E)
    {
        rot1 = glm::rotate(rot1, (float)glm::radians(15.0f), glm::vec3(0.0, 0.0, 1.0));
        std::cout << "已按下 E 键\n";
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_Q)
    {
        rot1 = glm::rotate(rot1, (float)glm::radians(-15.0f), glm::vec3(0.0, 0.0, 1.0));
        std::cout << "已按下 Q 键\n";
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_R)
    {
        rot1 = glm::mat4(1.0f);
        std::cout << "已按下 R 键\n";
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
        std::cout << "按下esc健，已退出\n";
    }
}

void KeyBoard::mouse_callback(GLFWwindow* window, double xoffset, double yoffset)               // 见笔记图解
{

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateCameraVectors();
}

void KeyBoard::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 90.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 90.0f)
        fov = 90.0f;
}

void KeyBoard::updateCameraVectors()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));                                        // 见笔记图解
    cameraFront = glm::normalize(front);

    right = glm::normalize(glm::cross(cameraFront, cameraUp));
    up = glm::normalize(glm::cross(right, cameraFront));
}