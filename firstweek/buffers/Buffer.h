#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include "../ErrorHandling.h"

class Buffer
{
private:
	unsigned int m_id;
	unsigned int m_type;
public:
	Buffer(unsigned int type) : m_type(type) {}

	void generate();
	void setData(const void* data, size_t size);
	void bind()   const { GLCall(glBindBuffer(m_type, m_id)); }
	void unbind() const { GLCall(glBindBuffer(m_type, 0)); }
};