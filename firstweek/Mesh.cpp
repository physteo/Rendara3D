#include "Mesh.h"


void Mesh::fill(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material& material)
{
	m_vao.generate();
	m_vao.bind();
	
	// put each attribute into separate vector of floats
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<float> tangents;

	positions.reserve(3 * vertices.size());
	normals.reserve(3 * vertices.size());
	texCoords.reserve(2 * vertices.size());
	tangents.reserve(3 * vertices.size());

	for (size_t i = 0; i < vertices.size(); i++)
	{
		positions.push_back(vertices.at(i).position.x);
		positions.push_back(vertices.at(i).position.y);
		positions.push_back(vertices.at(i).position.z);

		normals.push_back(vertices.at(i).normal.x);
		normals.push_back(vertices.at(i).normal.y);
		normals.push_back(vertices.at(i).normal.z);

		texCoords.push_back(vertices.at(i).texCoords.x);
		texCoords.push_back(vertices.at(i).texCoords.y);

		tangents.push_back(vertices.at(i).tangent.x);
		tangents.push_back(vertices.at(i).tangent.y);
		tangents.push_back(vertices.at(i).tangent.z);

	}

	m_vao.fillData({ positions, normals, texCoords, tangents }, { 3,3,2,3 });
	m_vao.setIbo(indices);
	// set number of indices
	m_indices = indices.size();
	m_vao.unbind();

	// shallow copy
	m_material = material;

}

void Mesh::fill(const std::vector<float>& positions, const std::vector<float>& normals, const std::vector<float>& texCoords,
	const std::vector<unsigned int>& indices, const Material& material)
{
	m_vao.generate();
	m_vao.bind();


	m_vao.fillData({ positions, normals, texCoords }, { 3,3,2 });
	m_vao.setIbo(indices);
	// set number of indices
	m_indices = indices.size();
	m_vao.unbind();

	// shallow copy
	m_material = material;
}

void Mesh::actualDraw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader)
{
	shader.bind();

	glm::mat4 model{ 1.0 };
	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(radians.z), glm::vec3{ 0.0f,0.0f,1.0f });
	model = glm::rotate(model, glm::radians(radians.y), glm::vec3{ 0.0f,1.0f,0.0f });
	model = glm::rotate(model, glm::radians(radians.x), glm::vec3{ 1.0f,0.0f,0.0f });
	model = glm::scale(model, scale);
	glm::mat3 normalMatrix = glm::inverse(glm::transpose(model));

	shader.setUniformMatrix("model", model, false);
	shader.setUniformMatrix("normalMat", normalMatrix, false);


	m_vao.bind();
	// draw call
	GLCall(glDrawElements(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0));
	m_vao.unbind();
	shader.unbind();
}

void Mesh::draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader)
{
	passMaterialUniforms(shader);
	actualDraw(scale, position, radians, shader);
}


void Mesh::draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material)
{
	passMaterialUniforms(shader, material);
	actualDraw(scale, position, radians, shader);
}

void Mesh::draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader)
{
	this->draw(glm::vec3{ scale }, position, radians, shader);
}

void Mesh::draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material)
{
	this->draw(glm::vec3{ scale }, position, radians, shader, material);
}


void Mesh::passMaterialUniforms(Shader& shader) const
{
	passMaterialUniforms(shader, this->m_material);
}

void Mesh::passMaterialUniforms(Shader& shader, Material material) const
{
	shader.bind();
	material.passUniforms(shader);
}