#include "OmnidirectionalShadowMaps.h"

OmnidirectionalShadowMaps::OmnidirectionalShadowMaps()
{
	FBO = 0;
	shadowMap = 0;
	shadowWidth = 0;
	shadowHeight = 0;
}

void OmnidirectionalShadowMaps::creatOmnidirectionalShadowMaps(const unsigned int width, const unsigned int height)
{
	shadowWidth = width;
	shadowHeight = height;

	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmnidirectionalShadowMaps::bindOSWFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OmnidirectionalShadowMaps::unBindOSWFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmnidirectionalShadowMaps::bindOSWTexture(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
}

// ÁùÃæµÄÒõÓ°ÌùÍ¼±ä»»¾ØÕó
std::vector<glm::mat4> OmnidirectionalShadowMaps::creatShadowTransforms(glm::mat4 shadowProj, glm::vec3 lightPos)
{
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	return shadowTransforms;
}				

OmnidirectionalShadowMaps::~OmnidirectionalShadowMaps()
{
	if (FBO)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if (shadowMap)
	{
		glDeleteTextures(1, &shadowMap);
	}
}
