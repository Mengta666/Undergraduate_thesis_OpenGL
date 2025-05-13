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
    return glm::lookAt(cameraPos, cameraPos + cameraFront, up);                             //cameraPos + cameraFront ��֤������ô�ƶ������������ע����Ŀ�귽��
}

void KeyBoard::userInput(GLFWwindow* window, float deltaTime)                                                        // ������⣨����ƶ���
{
    float camera_Speed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraFront * camera_Speed;
        std::cout << "�Ѱ��� W ��\n";
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraFront * camera_Speed;
        std::cout << "�Ѱ��� S ��\n";
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraUp, cameraFront)) * camera_Speed;
        std::cout << "�Ѱ��� D ��\n";
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraUp, cameraFront)) * camera_Speed;
        std::cout << "�Ѱ��� A ��\n";
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        cameraPos += cameraUp * camera_Speed;
        std::cout << "�Ѱ��� up ��\n";
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        cameraPos -= cameraUp * camera_Speed;
        std::cout << "�Ѱ��� down ��\n";
    }
}

void KeyBoard::key_rotate(GLFWwindow* window, int key, int scancode, int action, int mode, glm::mat4& rot1)                        // ������⣨������ת��
{
    if (action == GLFW_PRESS && key == GLFW_KEY_E)
    {
        rot1 = glm::rotate(rot1, (float)glm::radians(15.0f), glm::vec3(0.0, 0.0, 1.0));
        std::cout << "�Ѱ��� E ��\n";
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_Q)
    {
        rot1 = glm::rotate(rot1, (float)glm::radians(-15.0f), glm::vec3(0.0, 0.0, 1.0));
        std::cout << "�Ѱ��� Q ��\n";
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_R)
    {
        rot1 = glm::mat4(1.0f);
        std::cout << "�Ѱ��� R ��\n";
    }

    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
        std::cout << "����esc�������˳�\n";
    }
}

void KeyBoard::mouse_callback(GLFWwindow* window, double xoffset, double yoffset)               // ���ʼ�ͼ��
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
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));                                        // ���ʼ�ͼ��
    cameraFront = glm::normalize(front);

    right = glm::normalize(glm::cross(cameraFront, cameraUp));
    up = glm::normalize(glm::cross(right, cameraFront));
}