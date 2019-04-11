#pragma once

#include <vector>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include "../ErrorHandling.h"
#include "../Texture.h"


//! Class for OpenGL Framebuffers
/*!
	Stores a FrameBuffer with its ID.
	After instantiation, one needs to use \ref FrameBuffer.generate method to initialize the frame buffer.
	2DTextures and RenderBuffers can be attached, and are owned by the FrameBuffer object.
*/
class FrameBuffer
{
public:
	FrameBuffer() {}
	~FrameBuffer();

	// FrameBuffer shall not be copy/assign constructible (it is the only owner of attached textures and renderbuffers)
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	//!< Allocates memory for the framebuffer objects.
	void generate();
	//!< Dinds the target to this framebuffer. Default target is GL_FRAMEBUFFER. It sets an internal state that keeps track of current target.  
	void bind(GLenum target = GL_FRAMEBUFFER);
	//!< Binds the current framebuffer target to zero.
	void unbind();
	//!< creates a texture and owns it, stores its ID.
	void attach2DTexture(GLenum attachment, int width, int height, int nrChannels, Format internalFormat, GLenum dataFormat);
	//!< creates a renderbuffer and owns it, stores its ID.
	void attachRenderBuffer(GLenum internalFormat, int width, int height);
	//!< check if the framebuffer is complete (OpenGL requires at least one color attachment).
	bool iscomplete();
	//!< retrieves the ID of the specified texture.
	inline unsigned int getAttachedTextureID(size_t i) const { return textureAttachments.at(i).getID(); }

private:
	unsigned int m_id;
	GLenum  m_currentTarget;

	std::vector<Texture> textureAttachments;
	std::vector<unsigned int> renderBuffersAttachments;
};
