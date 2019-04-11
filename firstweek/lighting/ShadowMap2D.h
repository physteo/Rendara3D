#pragma once

/* opengl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../ErrorHandling.h"

#include "../Texture.h"
#include "../Window.h"
#include "OrthoFrustrum.h"

#include "SunLight.h"

// Class used for generating a 2D shadow map, for directional (sun) lights.
// It creates the framebuffer used for rendering the depth map, and a texture used for 
// saving the depth information. It stores their IDs. It also contains the width and height of the texture.
// Need to use ".generate()" after instantiation.
class ShadowMap2D
{
public:

	ShadowMap2D(OrthoFrustrum frustrum, float width, float height) : m_frustrum(frustrum), m_width(width), m_height(height) {}

	// create a framebuffer ID, a texture ID used for saving the depth information
	void generate();
	 
	// Binds the framebuffer and clears its depth (DEPTH_BUFFER_BIT)
	void clearShadows();

	// Binds the framebuffer and the shader used for computing the shadows, set the viewport to the shadow's size, disables CULL_FACE,
	// passes the lightSpaceMatrix to the shader used for computing the shadows, clears DEPTH_BUFFER_BIT
	void startShadows(const Window& window, Shader& shader, const SunLight* sun);
	
	// unbinds the framebuffer and the shader used for computing the shadows, set the viewport back to the window's size, enables CULL_FACE
	void stopShadows(const Window& window, Shader& shader);

	// passes to the shader the texture sampler as uniform, activates the texture and binds it. The shader is supposed to know which texture
	// unit to
	void passUniforms(Shader& shader,
		const std::string& textureUniformName,
		const std::string& lightSpaceMatrixUniformName,
		const glm::mat4& lightViewMatrix);
	
	// For debugging. Draws the texture generated for computing the shadows.
	void drawShadowMap(Window& window, float width, float height, Shader& debugShader);

	inline float getWidth() const { return m_width; }
	inline float getHeight() const { return m_height; }
	inline unsigned int getTextureID() const { return m_textureID; }
	inline unsigned int getFrameBufferID() const { return m_frameBufferID; }

private:
	OrthoFrustrum m_frustrum;

	int m_width;
	int m_height;
	unsigned int m_frameBufferID;
	unsigned int m_textureID;


private:
	void renderQuad();

};