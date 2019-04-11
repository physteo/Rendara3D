#pragma once

/* stl */
#include<vector>

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "./buffers/VertexArray.h"
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
	void fillQuad(int grid_x, int grid_y);

	void bindVao() const { m_vao.bind(); }
	void unbindVao() const { m_vao.unbind(); }

	void passMaterialUniforms(Shader& shader) const;
	void passMaterialUniforms(Shader& shader, Material material) const;


	void draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const;
	void draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const;
	void draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const;
	void draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const;

	void drawQuad(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians,
		int sprite_x, int sprite_y, Shader& shader, const Texture& diffuse);


	unsigned int getIndices() const { return m_indices; }

private:
	VertexArray  m_vao;
	Material     m_material;
	unsigned int m_indices;

	void actualDraw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const;

};


//! Creates and draw a screen-fitting quad. Useful for rendering textures on the screen. (e.g. with HDR)
class ScreenQuad
{
	unsigned int quadVAOi;

public:
	ScreenQuad();
	void draw();
};