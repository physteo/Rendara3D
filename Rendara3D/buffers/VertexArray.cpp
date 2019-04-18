#include "VertexArray.h"


VertexArray::VertexArray(const std::vector<std::vector<float> >& attributes, const std::vector<unsigned int>& components, const std::vector<unsigned int>& indices)
{
	generate();
	bind();
	fillData(attributes, components);
	setIbo(indices);
	unbind();
}

VertexArray::~VertexArray()
{
	release();
}

VertexArray::VertexArray(VertexArray&& other)
{
	swapData(other);
}

VertexArray& VertexArray::operator=(VertexArray&& other)
{
	// check for self-assignment.
	if (this != &other)
	{
		release();
		swapData(other);
	}
	return *this;
}

void VertexArray::fillData(const std::vector<std::vector<float>>& attributes, const std::vector<unsigned int>& components)

{
	std::vector<float> data;

	unsigned int numVertices = attributes.at(0).size() / components.at(0);

	/* put data in format VNTVNTVNTVNT */
	for (size_t i = 0; i < numVertices; i++)
	{
		for (size_t j = 0; j < attributes.size(); j++)
		{
			for (size_t k = 0; k < components.at(j); k++)
			{
				data.push_back(attributes.at(j).at(k + i * components.at(j)));
			}
		}
	}

	/* number of floats for each vertex */
	int stride = 0;
	for (size_t i = 0; i < components.size(); i++)
	{
		stride += components.at(i);
	}

	/* put data in the GPU */
	Buffer vbo{ GL_ARRAY_BUFFER, &data[0], sizeof(float) * data.size() };
	m_vbo = std::move(vbo);

	for (size_t i = 0; i < attributes.size(); i++)
	{
		// compute the stride for this attribute
		int offset = 0;
		for (size_t j = 0; j < i; j++)
		{
			offset += components.at(j);
		}

		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, components.at(i), GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float))));
	}

	m_vbo.unbind(); // TODO: necessary?
	unbind();
}

void VertexArray::setIbo(const std::vector<unsigned int>& indices)
{
	bind();

	Buffer ibo{ GL_ELEMENT_ARRAY_BUFFER, &indices[0], indices.size() * sizeof(unsigned int) };
	m_ibo = std::move(ibo);

	unbind();
	m_ibo.unbind(); // Note: need to first unbind the vao.

}


void VertexArray::swapData(VertexArray& other)
{
	m_id = other.m_id;
	other.m_id = 0;
	m_ibo = std::move(other.m_ibo);
	m_vbo = std::move(other.m_vbo);
}

void VertexArray::release()
{
	GLCall(glDeleteVertexArrays(1, &m_id));
}