
#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
	generate();
}

FrameBuffer::~FrameBuffer()
{
	release();
}

FrameBuffer::FrameBuffer(FrameBuffer&& other)
{
	swapData(other);
}
FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other)
{
	if (this != &other)
	{
		release();
		swapData(other);
	}
	return *this;
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

void FrameBuffer::attach2DTexture(GLenum attachment, Texture&& texture)
{
	textureAttachments.push_back(std::move(texture));
	textureAttachments.back().bind();

	// attach to framebuffer
	bind();
	GLCall(glFramebufferTexture2D(m_currentTarget, attachment, GL_TEXTURE_2D, textureAttachments.back().getID(), 0));
	unbind();
	textureAttachments.back().unbind();
}

void FrameBuffer::attach2DTexture(GLenum attachment, int width, int height, int nrChannels, Format internalFormat, GLenum dataFormat)
{
	// create texture
	Texture newTexture{ width, height, nrChannels, "nopath-FB", internalFormat, dataFormat, NULL };
	textureAttachments.push_back(std::move(newTexture));
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


void FrameBuffer::release()
{
	// release RenderBuffers
	for (auto it = renderBuffersAttachments.begin(); it != renderBuffersAttachments.end(); ++it)
	{
		GLCall(glDeleteRenderbuffers(1, &(*it)));
	}
	renderBuffersAttachments.clear();

	// textures are released by their destructors
	textureAttachments.clear();

	// release FrameBuffer
	GLCall(glDeleteFramebuffers(1, &m_id));
	m_id = 0;
}

void FrameBuffer::swapData(FrameBuffer& other)
{
	// move
	m_id = other.m_id;
	m_currentTarget = other.m_currentTarget;
	for (auto it = other.textureAttachments.begin(); it != other.textureAttachments.end(); ++it)
	{
		textureAttachments.emplace_back(std::move(*it));
	}
	for (auto it = other.renderBuffersAttachments.begin(); it != other.renderBuffersAttachments.end(); ++it)
	{
		renderBuffersAttachments.push_back(*it);
		// put other to default
		*it = 0;
	}

	other.m_id = 0;
	// other.m_currentTarget can stay the same

}