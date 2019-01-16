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


	void draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader);
private:
	VertexArray  m_vao;
	Material     m_material;

	unsigned int m_indices;
};