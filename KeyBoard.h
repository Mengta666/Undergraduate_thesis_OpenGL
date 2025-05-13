#pragma once
/* 该头文件仅用于连续移动时 */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glut/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class KeyBoard {
public:
	void setWindow(GLFWwindow* window);

	void setPFU(glm::vec3 cameraPos,glm::vec3 cameraFront,glm::vec3 cameraUp);

public:
	glm::mat4 lookAt();
	void userInput(GLFWwindow* window, float deltaTime);
	void key_rotate(GLFWwindow* window, int key, int scancode, int action, int mode, glm::mat4& rotateMat);

	void mouse_callback(GLFWwindow* window, double xoffset, double yoffset);

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


	inline glm::vec3 getCameraPos()const { return cameraPos;  }
	inline glm::vec3 getCameraFront()const { return cameraFront; }
	inline float getFOV()const { return fov; }
	inline float getYaw()const { return yaw; }

private:
	GLFWwindow* m_window;

	/*相机坐标反馈*/
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 up;
	glm::vec3 right;

	float sensitivity = 0.05f;
	float pitch = 0.0f;
	float yaw = -90.0f;
	glm::vec3 front;

	float fov = 90.0f;


	void updateCameraVectors();
};
