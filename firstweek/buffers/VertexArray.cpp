#include "VertexArray.h"

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
	m_vbo.generate();
	m_vbo.bind();
	m_vbo.setData(&data[0], sizeof(float) * data.size());



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

	m_vbo.unbind();
	unbind();
}

void VertexArray::setIbo(const std::vector<unsigned int>& indices)
{
	bind();
	m_ibo.generate();
	m_ibo.bind();

	m_ibo.setData(&indices[0], indices.size() * sizeof(unsigned int));

	unbind();
	m_ibo.unbind();
}
