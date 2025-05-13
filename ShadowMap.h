#pragma once


#include <stdio.h>

#include <GL/glew.h>


class ShadowMap {
public:
	ShadowMap();

	virtual void creatShadowMap(const unsigned int width, const unsigned int height);

	virtual void bindFramebuffer();

	virtual void unBindFramebuffer();

	virtual void bindTexture(int slot);

	inline GLuint GetShadowWidth() { return shadowWidth; }
	inline GLuint GetShadowHeight() { return shadowHeight; }

	~ShadowMap();

protected:
	GLuint FBO, shadowMap;
	GLuint shadowWidth, shadowHeight;

};