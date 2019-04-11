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


//!< Used for creating quads, that will use sprite sheets with a grid of (grid_x, grid_y) sprites. This one uses x and z coords.
void Mesh::fillQuad(int grid_x, int grid_y)
{
	std::vector<Vertex> vertices;
	vertices.resize(4);
	vertices.at(0).position = glm::vec3{ -0.5f, 0.0f, 0.5 };
	vertices.at(0).normal = glm::vec3{ 0.0f, 0.0f, 1.0f };
	vertices.at(0).tangent = glm::vec3{ 0.0f, 0.0f, 0.0f };

	vertices.at(1).position = glm::vec3{ 0.5f, 0.0f, 0.5 };
	vertices.at(1).normal = glm::vec3{ 0.0f, 0.0f, 1.0f };
	vertices.at(1).tangent = glm::vec3{ 0.0f, 0.0f, 0.0f };

	vertices.at(2).position = glm::vec3{ 0.5f, 0.0f, -0.5 };
	vertices.at(2).normal = glm::vec3{ 0.0f, 0.0f, 1.0f };
	vertices.at(2).tangent = glm::vec3{ 0.0f, 0.0f, 0.0f };

	vertices.at(3).position = glm::vec3{ -0.5f, 0.0f, -0.5f };
	vertices.at(3).normal = glm::vec3{ 0.0f, 0.0f, 1.0f };
	vertices.at(3).tangent = glm::vec3{ 0.0f, 0.0f, 0.0f };

	vertices.at(0).texCoords = glm::vec3{ 0.0f,                    1.0 / ((float)grid_y),      0.0f };
	vertices.at(1).texCoords = glm::vec3{ 1.0f / ((float)grid_x), 1.0 / ((float)grid_y),      0.0f };
	vertices.at(2).texCoords = glm::vec3{ 1.0f / ((float)grid_x), 0.0f,                        0.0f };
	vertices.at(3).texCoords = glm::vec3{ 0.0f,                    0.0f,                        0.0f };

	std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

	// create placeholder material
	Material material;

	const unsigned char* textureData = { 0 };
	Texture diffuse;
	Texture normal;
	Texture specular;
	diffuse.generate(1, 1, 3, Format::RGB, textureData);
	normal.generate(1, 1, 3, Format::RGB, textureData);
	specular.generate(1, 1, 3, Format::RGB, textureData);
	material.fill(diffuse, specular, normal, 1.0f);

	fill(vertices, indices, material);
}

void Mesh::drawQuad(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, int sprite_x, int sprite_y, Shader& shader, const Texture& spriteSheet)
{
	shader.bind();
	/* pass sprite coordinates */
	shader.setUniformValue("x", sprite_x);
	shader.setUniformValue("y", sprite_y);
	/* pass texture diffuse */
	shader.setTexture(GL_TEXTURE_2D, "material.diffuse", spriteSheet.getID());
	actualDraw(scale, position, radians, shader);
}


void Mesh::actualDraw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const
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

void Mesh::draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const
{
	passMaterialUniforms(shader);
	actualDraw(scale, position, radians, shader);
}


void Mesh::draw(const glm::vec3& scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const
{
	passMaterialUniforms(shader, material);
	actualDraw(scale, position, radians, shader);
}

void Mesh::draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader) const
{
	this->draw(glm::vec3{ scale }, position, radians, shader);
}

void Mesh::draw(float scale, const glm::vec3& position, const glm::vec3& radians, Shader& shader, Material& material) const
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



ScreenQuad::ScreenQuad()
{
	unsigned int quadVBOi;
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// setup plane VAO
	GLCall(glGenVertexArrays(1, &quadVAOi));
	GLCall(glGenBuffers(1, &quadVBOi));
	GLCall(glBindVertexArray(quadVAOi));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, quadVBOi));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
}

void ScreenQuad::draw()
{
	GLCall(glBindVertexArray(quadVAOi));
	GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCall(glBindVertexArray(0));
}