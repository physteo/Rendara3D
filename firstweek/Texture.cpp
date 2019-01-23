#include "Texture.h"

void Texture::generate(const std::string& filepath, Format internalFormat)
{
	m_filepath = filepath;
	int width, height, nrChannels;
	unsigned char* data = load_image(filepath.c_str(), &width, &height, &nrChannels, true);
	if (data)
	{
		generate(width, height, nrChannels, internalFormat, data);
		free_image(data);
	}
	else
	{
		std::cerr << "[Graphics Engine Error]: Failed to load texture from path: " << filepath  << std::endl;
		ASSERT(false);
	}
}

void Texture::generate(int width, int height, int nrChannels, Format internalFormat, const unsigned char* data)
{
	Format format;
	if (nrChannels == 4)
		format = Format::RGBA;
	else if (nrChannels == 3)
		format = Format::RGB;
	else
	{
		std::cerr << "[Graphics Engine Error]: Texture's number of channels not recognized." << std::endl;
		assert(false);
	}

	// choose format based on the number of channels
	GLCall(glGenTextures(1, &m_id));
	bind();
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, (void*)data));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	unbind();
}

Material::Material(const Texture& diffuse, const Texture& specular, const Texture& normal, float shininess)
{
	fill(diffuse, specular, normal, shininess);
}

void Material::fill(const Texture& diffuse, const Texture& specular, const Texture& normal, float shininess)
{
	m_diffuse   = diffuse;
	m_specular  = specular;
	m_shininess = shininess;
	m_normal    = normal;
}


void Material::passUniforms(Shader& shader) const
{
	shader.bind();

	/* pass diffuse */
	shader.setTexture(GL_TEXTURE_2D, "material.diffuse", m_diffuse.getID());

	/* pass specular */
	shader.setTexture(GL_TEXTURE_2D, "material.specular", m_specular.getID());

	/* pass normal */
	shader.setTexture(GL_TEXTURE_2D, "material.normal", m_normal.getID());

	/* pass shininess */
	shader.setUniformValue("material.shininess", m_shininess);
}