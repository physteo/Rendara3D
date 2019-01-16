#include "ShadowMap2D.h"

void ShadowMap2D::generate()
{
	// generate framebuffer with which we draw the shadow map
	GLCall(glGenFramebuffers(1, &m_frameBufferID));

	// generate the texture to which we draw the shadow map
	GLCall(glGenTextures(1, &m_textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureID));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	Texture::set2DTextureParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	Texture::set2DTextureParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	Texture::set2DTextureParameter(GL_TEXTURE_BORDER_COLOR, borderColor);
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// attach the texture to the framebuffer 
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textureID, 0));
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

}

void ShadowMap2D::startShadows(const Window& window, Shader& shadowShader, const SunLight* sun)
{
	window.setViewPort(m_width, m_height);
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));
	GLCall(glClear(GL_DEPTH_BUFFER_BIT));
	shadowShader.bind();
	shadowShader.setUniformMatrix("lightSpaceMatrix", m_frustrum.projection * sun->getViewMatrix(), false);
	GLCall(glDisable(GL_CULL_FACE));
}

void ShadowMap2D::stopShadows(const Window& window, Shader& shadowShader)
{
	shadowShader.unbind();
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	window.setViewPort(window.getWidth(), window.getHeight());
}

void ShadowMap2D::passUniforms(Shader& shader, 
								const std::string& textureUniformName,
								const std::string& lightSpaceMatrixUniformName,
								const glm::mat4& lightViewMatrix)
{
	shader.setUniformMatrix(lightSpaceMatrixUniformName, m_frustrum.projection * lightViewMatrix, false);
	shader.setTexture(GL_TEXTURE_2D, textureUniformName, m_textureID);
}

void ShadowMap2D::drawShadowMap(Window& window, float width, float height, Shader& debugShader)
{
	window.setViewPort(width, height);
	// render Depth map to quad for visual debugging
	debugShader.bind();
	debugShader.setUniformValue("depthMap", 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
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