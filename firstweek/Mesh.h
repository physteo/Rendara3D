#pragma once

/* stl */
#include<vector>

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
};


class Mesh
{
public:
	Mesh() : m_indices(0) {}
	void fill(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material& material);
	void fill(const std::vector<float>& positions, const std::vector<float>& normals, const std::vector<float>& texCoords,
		const std::vector<unsigned int>& indices, const Material& material);

	// TODO: attributes should be enabled not from number 4 onward, but it should check the last enabled and increase by one
//	void enableInstances()
//	{
//		m_vao.bind();
//		
//		// set attribute pointers for matrix (4 times vec4)
//		glEnableVertexAttribArray(4);
//		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
//		glEnableVertexAttribArray(5);
//		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
//		glEnableVertexAttribArray(6);
//		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
//		glEnableVertexAttribArray(7);
//		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
//
//		glVertexAttribDivisor(4, 1);
//		glVertexAttribDivisor(5, 1);
//		glVertexAttribDivisor(6, 1);
//		glVertexAttribDivisor(7, 1);
//
//		m_vao.unbind();
//	}

	void bindVao() const { m_vao.bind(); }
	void unbindVao() const { m_vao.unbind(); }

	void passMaterialUniforms(Shader& shader) const;
	void passMaterialUniforms(Shader& shader, Material material) const;


	void draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader);
	void draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material);
	void draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader);
	void draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material);

	unsigned int getIndices() const { return m_indices; }

private:
	VertexArray  m_vao;
	Material     m_material;

	unsigned int m_indices;

	void actualDraw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader);

};