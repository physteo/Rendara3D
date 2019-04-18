#pragma once

/* opengl includes */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* rendering engine includes */
#include "../utils/ErrorHandling.h"
#include "../Window/Window.h"
#include "../buffers/FrameBuffer.h"
#include "../Texture/Texture.h"
#include "SunLight.h"


//! Class for 2D shadow maps.
/*!
	Class used for generating a 2D shadow map, for directional (sun) lights.
	It creates the framebuffer used for rendering the depth map. The depth map is a texture attached to
	the framebuffer. It also contains the width and height of the map.
*/
class ShadowMap2D
{
private:
	glm::mat4     m_frustrum;
	FrameBuffer   m_frameBuffer;
	float m_width;
	float m_height;
public:

	ShadowMap2D(float near = 0.1f, float far = 20.0f,
		         float left = -10.0f, float right = 10.0f,
		         float down = -10.0f, float up = 10.0f,
		         float textureWidth = 1024.0, float textureHeight = 1024.0);

	//!< Binds the framebuffer and clears its depth (DEPTH_BUFFER_BIT)
	void clearShadows();

	//!< Binds the framebuffer and the shader used for computing the shadows, set the viewport to the shadow's size, disables CULL_FACE,
	//!< passes the lightSpaceMatrix to the shader used for computing the shadows, clears DEPTH_BUFFER_BIT
	void startShadows(const Window& window, Shader& shadowShader, const SunLight* sun);
	
	//!< unbinds the framebuffer and the shader used for computing the shadows, set the viewport back to the window's size, enables CULL_FACE
	void stopShadows(const Window& window, Shader& shadowShader);

	//!< Passes to the shader the texture sampler as uniform, using the given textureUniformName.
	void passUniforms(Shader& shader,
		const std::string& textureUniformName,
		const std::string& lightSpaceMatrixUniformName,
		const glm::mat4& lightViewMatrix);

	//!< Used for debugging. Draws the texture generated for computing the shadows, on the screen as a quad.
	void drawShadowMap(Window& window, float width, float height, Shader& debugShader);

	inline float getWidth() const { return m_width; }
	inline float getHeight() const { return m_height; }

private:
	//!< Used for debugging. Draws a quad on the screen that will be filled with the shadow map.
	void renderQuad();
};