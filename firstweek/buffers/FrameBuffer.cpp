
#include "FrameBuffer.h"

FrameBuffer::~FrameBuffer()
{
	for (auto it = textureAttachments.begin(); it != textureAttachments.end(); ++it)
	{
		const GLuint idx = (*it).getID();
		GLCall(glDeleteTextures(1, &idx));
	}

	for (auto it = renderBuffersAttachments.begin(); it != renderBuffersAttachments.end(); ++it)
	{
		GLCall(glDeleteRenderbuffers(1, &(*it)));
	}
}

void FrameBuffer::generate()
{
	GLCall(glGenFramebuffers(1, &m_id)); // TODO: why glCREATEFrameBuffers was causing crashes on a different machine?
	m_currentTarget = GL_FRAMEBUFFER;
}


void FrameBuffer::bind(GLenum target)
{
	m_currentTarget = target;
	GLCall(glBindFramebuffer(target, m_id));
}

void FrameBuffer::unbind()
{
	GLCall(glBindFramebuffer(m_currentTarget, 0));
}

void FrameBuffer::attach2DTexture(GLenum attachment, int width, int height, int nrChannels, Format internalFormat, GLenum dataFormat)
{
	// create texture
	textureAttachments.emplace_back();
	textureAttachments.back().generate(width, height, nrChannels, internalFormat, dataFormat, NULL);
	textureAttachments.back().bind();

	// attach to framebuffer
	bind();
	GLCall(glFramebufferTexture2D(m_currentTarget, attachment, GL_TEXTURE_2D, textureAttachments.back().getID(), 0));
	unbind();

	// unbind texture
	textureAttachments.back().unbind();
}

void FrameBuffer::attachRenderBuffer(GLenum internalFormat, int width, int height)
{
	renderBuffersAttachments.push_back(0);
	GLCall(glGenRenderbuffers(1, &renderBuffersAttachments.back()));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, renderBuffersAttachments.back()));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height));

	// attach texture and renderbuffer to framebuffer
	bind();
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffersAttachments.back()));
	unbind();

	// unbind renderframebuffer
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

bool FrameBuffer::iscomplete()
{
	bind();
	// TODO: allow return value of GLCall so that I can use it here.
	bool result = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	unbind();
	return result;
}