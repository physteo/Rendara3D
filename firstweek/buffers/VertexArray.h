#pragma once

#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include <assert.h>
#include <vector>
#include "Buffer.h"

class VertexArray
{
private:
	unsigned int m_id;
	Buffer m_vbo;
	Buffer m_ibo;

public:
	VertexArray() : m_id(-1), m_vbo{ GL_ARRAY_BUFFER }, m_ibo{ GL_ELEMENT_ARRAY_BUFFER } {}
	void generate() { GLCall(glGenVertexArrays(1, &m_id)); }
	void bind()   const { GLCall(glBindVertexArray(m_id)); }
	void unbind() const { GLCall(glBindVertexArray(0)); }
	void fillData(const std::vector<std::vector<float> >& attributes, const std::vector<unsigned int>& components);

	void setIbo(const std::vector<unsigned int>& indices);

};


#if 0
class VAO
{
private:
	unsigned int m_id;
	std::vector<Buffer> m_vbos;
	Buffer m_ibo;

public:
	VAO() : m_vbos(), m_ibo{ GL_ELEMENT_ARRAY_BUFFER } {}
	void generate() { glGenVertexArrays(1, &m_id); }
	void bind() { glBindVertexArray(m_id); }
	void unbind() { glBindVertexArray(0); }
	void addBuffer(const std::vector<float>& data, int numComponents)
	{
		bind();

		m_vbos.push_back(Buffer{ GL_ARRAY_BUFFER });
		Buffer& vbo = m_vbos.back();
		vbo.generate();
		vbo.bind();
		vbo.setData(&data[0], sizeof(float) * data.size());
		GLCall(glEnableVertexAttribArray(m_vbos.size() - 1));
		GLCall(glVertexAttribPointer(m_vbos.size() - 1, numComponents, GL_FLOAT, GL_FALSE, 0, 0));

		vbo.unbind();
		unbind();
	}

	void setIbo(const std::vector<unsigned int>& indices)
	{
		bind();
		m_ibo.generate();
		m_ibo.bind();

		m_ibo.setData(&indices[0], indices.size() * sizeof(unsigned int));

		unbind();
		m_ibo.unbind();
	}


};

#endif