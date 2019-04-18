#include "ShadowCubeMap.h"


ShadowCubeMap::~ShadowCubeMap()
{
	release();
}

ShadowCubeMap::ShadowCubeMap(ShadowCubeMap&& other)
{
	swapData(other);
}
ShadowCubeMap& ShadowCubeMap::operator=(ShadowCubeMap&& other)
{
	// check for self-assignment.
	if (this != &other)
	{
		release();
		swapData(other);
	}
	return *this;
}

ShadowCubeMap::ShadowCubeMap(float width, float height) : m_width(width), m_height(height)
{
	// TODO: add cube maps to the engine
	// create 3D texture
	GLCall(glGenTextures(1, &m_3DtextureID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_3DtextureID));
	for (size_t i = 0; i < 6; i++)
	{
		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
			NULL));
	}
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	// attach cubemap to FBO
	m_frameBuffer.bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_3DtextureID, 0);
	GLCall(glDrawBuffer(GL_NONE));  // TODO: necessary??
	GLCall(glReadBuffer(GL_NONE));	// TODO: necessary??
	m_frameBuffer.unbind();
}

void ShadowCubeMap::clearShadows()
{
	m_frameBuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT); 
}

void ShadowCubeMap::startShadows(const Window& window, Shader& cubeDepthShader, const PointLight& pointLight)
{
	window.setViewPort(m_width, m_height);
	m_frameBuffer.bind();
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
	m_frameBuffer.unbind();
	window.setViewPort(window.getWidth(), window.getHeight());
}

// Passes the farPlane and activates the cube texture
void ShadowCubeMap::passUniforms(Shader& shader, const std::string& textureUniformName, const std::string& farPlaneUniformName)
{
	shader.setUniformValue(farPlaneUniformName, 20.0f);
	shader.setTexture(GL_TEXTURE_CUBE_MAP, textureUniformName, m_3DtextureID);
	//shader.setUniformValue("cubeDepthMap", textureSlot);
	//glActiveTexture(GL_TEXTURE0 + textureSlot);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
}


void ShadowCubeMap::release()
{
	GLCall(glDeleteTextures(1, &m_3DtextureID));
}

void ShadowCubeMap::swapData(ShadowCubeMap& other)
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_3DtextureID = other.m_3DtextureID;
	m_frameBuffer = std::move(other.m_frameBuffer);

	other.m_width = 0;
	other.m_height = 0;
	other.m_3DtextureID = 0;
}