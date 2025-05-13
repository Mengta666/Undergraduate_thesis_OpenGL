#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class OmnidirectionalShadowMaps
{
public:


	OmnidirectionalShadowMaps();

	virtual void creatOmnidirectionalShadowMaps(const unsigned int width, const unsigned int height);

	virtual void bindOSWFramebuffer();

	virtual void unBindOSWFramebuffer();

	virtual void bindOSWTexture(int slot);

	inline GLuint GetOmnidirextionalShadowWidth() { return shadowWidth; }
	inline GLuint GetOmnidirextionalShadowHeight() { return shadowHeight; }

	std::vector<glm::mat4> creatShadowTransforms(glm::mat4 shadowProj, glm::vec3 lightPos);

	~OmnidirectionalShadowMaps();

protected:
	GLuint FBO, shadowMap;
	GLuint shadowWidth, shadowHeight;

	std::vector<glm::mat4> shadowTransforms;

};
