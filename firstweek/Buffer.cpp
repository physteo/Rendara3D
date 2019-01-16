#include "Buffer.h"

void Buffer::generate() {
	GLCall(glGenBuffers(1, &m_id));
}

void Buffer::setData(const void* data, size_t size)
{
	bind();
	GLCall(glBufferData(m_type, size, data, GL_STATIC_DRAW));
}
