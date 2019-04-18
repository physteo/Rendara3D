#include "ShadowMap2D.h"



ShadowMap2D::ShadowMap2D(float near, float far,
	float left, float right,
	float down, float up,
	float textureWidth, float textureHeight)
	: m_width(textureWidth), m_height(textureHeight), m_frameBuffer()
{
	m_frustrum = glm::ortho(left, right, down, up, near, far);

	// create texture for 2D shadow with the correct parameters
	Texture  depthAttachmentTexture{ GL_DEPTH_COMPONENT, (int)textureWidth, (int)textureHeight, "aaa", GL_DEPTH_COMPONENT, GL_FLOAT,NULL };
	depthAttachmentTexture.bind();
	depthAttachmentTexture.set2DTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	depthAttachmentTexture.set2DTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	depthAttachmentTexture.set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	depthAttachmentTexture.set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	depthAttachmentTexture.set2DTextureParameter(GL_TEXTURE_BORDER_COLOR, borderColor);
	depthAttachmentTexture.unbind();
	// attach to framebuffer
	m_frameBuffer.attach2DTexture(GL_DEPTH_ATTACHMENT, std::move(depthAttachmentTexture));

	m_frameBuffer.bind();
	GLCall(glDrawBuffer(GL_NONE));  // TODO: necessary??
	GLCall(glReadBuffer(GL_NONE));	// TODO: necessary??
	m_frameBuffer.unbind();

}


void ShadowMap2D::clearShadows()
{
	m_frameBuffer.bind();
	GLCall(glClear(GL_DEPTH_BUFFER_BIT));
}

void ShadowMap2D::startShadows(const Window& window, Shader& shadowShader, const SunLight* sun)
{
	window.setViewPort(m_width, m_height);
	m_frameBuffer.bind();
	shadowShader.bind();
	shadowShader.setUniformMatrix("lightSpaceMatrix", m_frustrum * sun->getViewMatrix(), false);
	GLCall(glDisable(GL_CULL_FACE));
}

void ShadowMap2D::stopShadows(const Window& window, Shader& shadowShader)
{
	shadowShader.unbind();
	GLCall(glEnable(GL_CULL_FACE));
	m_frameBuffer.unbind();
	window.setViewPort(window.getWidth(), window.getHeight());
}

void ShadowMap2D::passUniforms(Shader& shader,
	const std::string& textureUniformName,
	const std::string& lightSpaceMatrixUniformName,
	const glm::mat4& lightViewMatrix)
{
	shader.setUniformMatrix(lightSpaceMatrixUniformName, m_frustrum * lightViewMatrix, false);
	shader.setTexture(GL_TEXTURE_2D, textureUniformName, m_frameBuffer.getAttachedTextureID(0));
}

void ShadowMap2D::drawShadowMap(Window& window, float width, float height, Shader& debugShader)
{
	window.setViewPort(width, height);
	// render Depth map to quad for visual debugging
	debugShader.bind();
	debugShader.setUniformValue("depthMap", 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_frameBuffer.getAttachedTextureID(0));
	renderQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
	debugShader.unbind();
	window.setViewPort(window.getWidth(), window.getHeight());
}

void ShadowMap2D::renderQuad()
{
	unsigned int quadVAO = 0;
	unsigned int quadVBO;

	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}