#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
	FBO = 0;
	shadowMap = 0;
	shadowWidth = 0;
	shadowHeight = 0;
}

void ShadowMap::creatShadowMap(const unsigned int width, const unsigned int height)							// 形参为 阴影分辨率
{
	shadowWidth = width;
	shadowHeight = height;

	glGenFramebuffers(1, &FBO);

	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT , NULL);			//设置阴影贴图的性质
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 0.0, 0.0, 0.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);                                                 // 检查缓冲区状态

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf_s("FrameBuffer Error: %i \n", status);
		return ;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ShadowMap::bindFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::unBindFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ShadowMap::bindTexture(int slot = 0)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}

ShadowMap::~ShadowMap()
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
