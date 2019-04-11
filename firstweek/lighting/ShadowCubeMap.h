#pragma once

#include <vector>

/* opengl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../ErrorHandling.h"

#include "../Texture.h"
#include "../Window.h"
#include "PointLight.h"

// Class used for generating a cube shadow map, for pointlights.
// It creates the framebuffer used for rendering the depth map, and a cube texture used for 
// saving the depth information. It stores their IDs. It also contains the width and height (resolution) of the texture.
// Need to use ".generate()" after instantiation.
class ShadowCubeMap
{
public:

	ShadowCubeMap(float width, float height) : m_width(width), m_height(height) {}

	// create a framebuffer ID, a texture ID used for saving the depth information
	void generate();

	// binds the framebuffer and clears its depth (GL_DEPTH_BUFFER_BIT)
	void clearShadows();

	// Binds the framebuffer and the shader used for computing the shadows, set the viewport to the shadow's size, disables CULL_FACE,
	// passes the poinLight's info (position, transformation matrices, and far_plane) to the shader used for computing the shadows
	void startShadows(const Window& window, Shader& cubeDepthShader, const PointLight& pointLight);

	// unbinds the framebuffer and the shader used for computing the shadows, set the viewport back to the window's size, enables CULL_FACE
	void stopShadows(const Window& window, Shader& shader);

	// For debugging. Draws the texture generated for computing the shadows.
	void drawShadowMap(Window& window, float width, float height, Shader& debugShader);

	// Passes the farPlane and activates the cube texture
	void passUniforms(Shader& shader, const std::string& textureUniformName, const std::string& farPlaneUniformName)
	{
		shader.setUniformValue(farPlaneUniformName, 20.0f);
		shader.setTexture(GL_TEXTURE_CUBE_MAP, textureUniformName, m_textureID);
		//shader.setUniformValue("cubeDepthMap", textureSlot);
		//glActiveTexture(GL_TEXTURE0 + textureSlot);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
	}

	inline float getWidth() const { return m_width; }
	inline float getHeight() const { return m_height; }
	inline unsigned int getTextureID() const { return m_textureID; }
	inline unsigned int getFrameBufferID() const { return m_frameBufferID; }

private:
	int m_width;
	int m_height;
	unsigned int m_frameBufferID;
	unsigned int m_textureID;

private:
	void renderQuad();

};