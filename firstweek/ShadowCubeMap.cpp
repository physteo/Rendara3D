#include "ShadowCubeMap.h"


void ShadowCubeMap::generate()
{
	// create FBO
	glGenFramebuffers(1, &m_frameBufferID);

	// create cube map
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	for (size_t i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach cubemap to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_textureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ShadowCubeMap::startShadows(const Window& window, Shader& cubeDepthShader, const PointLight& pointLight)
{
	window.setViewPort(m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
	glClear(GL_DEPTH_BUFFER_BIT); // modded
	float aspect = (float)m_width / (float)m_height;

	glm::vec3 lightPosition = pointLight.eye;

	float near = 0.1f;
	float far = 20.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	cubeDepthShader.bind();
	cubeDepthShader.setUniformMatrix("shadowMatrices[0]", shadowTransforms.at(0), false);
	cubeDepthShader.setUniformMatrix("shadowMatrices[1]", shadowTransforms.at(1), false);
	cubeDepthShader.setUniformMatrix("shadowMatrices[2]", shadowTransforms.at(2), false);
	cubeDepthShader.setUniformMatrix("shadowMatrices[3]", shadowTransforms.at(3), false);
	cubeDepthShader.setUniformMatrix("shadowMatrices[4]", shadowTransforms.at(4), false);
	cubeDepthShader.setUniformMatrix("shadowMatrices[5]", shadowTransforms.at(5), false);

	cubeDepthShader.setUniformValue("lightPos", lightPosition);
	cubeDepthShader.setUniformValue("far_plane", far);
	glDisable(GL_CULL_FACE);
}

void ShadowCubeMap::stopShadows(const Window& window, Shader& shader)
{
	glEnable(GL_CULL_FACE);
	shader.unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	window.setViewPort(window.getWidth(), window.getHeight());
}