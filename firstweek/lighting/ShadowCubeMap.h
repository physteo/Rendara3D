#pragma once


/* opengl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../utils/ErrorHandling.h"

/* Rendering Engine */
#include "../Texture/Texture.h"
#include "../Window/Window.h"
#include "PointLight.h"
#include "../buffers/FrameBuffer.h"

/* stl */
#include <vector>


//! Class used for generating a cube shadow map, for pointlights.
/*!
	It creates the framebuffer used for rendering the depth map, and a cube texture used for saving the 
	depth information. The texture is stored as It also contains the width and height (resolution) of the texture.
*/
class ShadowCubeMap
{
public:
	ShadowCubeMap(float width = 1024.f, float height = 1024.f);
	~ShadowCubeMap();

	ShadowCubeMap(ShadowCubeMap& other) = delete;
	ShadowCubeMap& operator=(ShadowCubeMap& other) = delete;

	ShadowCubeMap(ShadowCubeMap&& other);
	ShadowCubeMap& operator=(ShadowCubeMap&& other);


	//!< binds the framebuffer and clears its depth (GL_DEPTH_BUFFER_BIT)
	void clearShadows();
	//!< Binds the framebuffer and the shader used for computing the shadows, set the viewport to the shadow's size, disables CULL_FACE,
	//!< Passes the poinLight's info (position, transformation matrices, and far_plane) to the shader used for computing the shadows
	void startShadows(const Window& window, Shader& cubeDepthShader, const PointLight& pointLight);
	//!< Unbinds the framebuffer and the shader used for computing the shadows, set the viewport back to the window's size, enables CULL_FACE
	void stopShadows(const Window& window, Shader& shader);
	void passUniforms(Shader& shader, const std::string& textureUniformName, const std::string& farPlaneUniformName);


	inline float getWidth() const { return m_width; }
	inline float getHeight() const { return m_height; }

private:
	int m_width;
	int m_height;
	FrameBuffer   m_frameBuffer;
	unsigned int  m_3DtextureID;

	void release();
	void swapData(ShadowCubeMap& other);
};

