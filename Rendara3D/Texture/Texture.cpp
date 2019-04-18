#include "Texture.h"

Texture::Texture(const std::string& filepath, Format internalFormat)
{
	generate(filepath, internalFormat);
}

Texture::Texture(int width, int height, int nrChannels, const std::string& filepath, Format internalFormat, const unsigned char* data)
{
	m_filepath = filepath;
	generate(width, height, nrChannels, internalFormat, data);
}

Texture::Texture(int width, int height, int nrChannels, const std::string& filepath, Format internalFormat, GLenum dataFormat, const unsigned char* data)
{
	m_filepath = filepath;
	generate(width, height, nrChannels, internalFormat, dataFormat, data);
}

Texture::Texture(int internalFormat, int width, int height, const std::string& filepath, GLenum format, GLenum dataformat, const unsigned char* data)
{
	m_filepath = filepath;
	GLCall(glGenTextures(1, &m_id));
	bind();
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataformat, data));
	unbind();
}


Texture::Texture(Texture&& other)
{
	swapData(other);
}

Texture& Texture::operator=(Texture&& other)
{
	if (this != &other)
	{
		release();
		swapData(other);
	}
	return *this;
}

Texture::~Texture()
{
	release();
}


void Texture::set2DTextureParameter(GLenum pname, GLint param) 
{
	GLuint previouslyBoundTexture = 0;
	GLCall(glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&previouslyBoundTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_id));
	GLCall(glTexParameteri(GL_TEXTURE_2D, pname, param));
	GLCall(glBindTexture(GL_TEXTURE_2D, previouslyBoundTexture));
}

void Texture::set2DTextureParameter(GLenum pname, GLfloat param) 
{
	GLuint previouslyBoundTexture = 0;
	GLCall(glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&previouslyBoundTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_id));
	GLCall(glTexParameterf(GL_TEXTURE_2D, pname, param));
	GLCall(glBindTexture(GL_TEXTURE_2D, previouslyBoundTexture));
}

void Texture::set2DTextureParameter(GLenum pname, GLfloat* param)
{
	GLuint previouslyBoundTexture = 0;
	GLCall(glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&previouslyBoundTexture));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_id));
	GLCall(glTexParameterfv(GL_TEXTURE_2D, pname, param));
	GLCall(glBindTexture(GL_TEXTURE_2D, previouslyBoundTexture));
}




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
	generate(width, height, nrChannels, internalFormat, GL_UNSIGNED_BYTE, data);
}

void Texture::generate(int width, int height, int nrChannels, Format internalFormat, GLenum dataFormat, const unsigned char* data)
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
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat    , width  , height  , 0, format           , dataFormat, (void*)data));

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	unbind();
}


Material::Material(const Texture* diffuse, const Texture* specular, const Texture* normal, float shininess)
{
	fill(diffuse, specular, normal, shininess);
}

void Material::fill(const Texture* diffuse, const Texture* specular, const Texture* normal, float shininess)
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
	shader.setTexture(GL_TEXTURE_2D, "material.diffuse", m_diffuse->getID());

	/* pass specular */
	shader.setTexture(GL_TEXTURE_2D, "material.specular", m_specular->getID());

	/* pass normal */
	shader.setTexture(GL_TEXTURE_2D, "material.normal", m_normal->getID());

	/* pass shininess */
	shader.setUniformValue("material.shininess", m_shininess);
}



void Texture::release()
{
	GLCall(glDeleteTextures(1, &m_id));
}

void Texture::swapData(Texture& other)
{
	m_id = other.m_id;
	m_filepath = other.m_filepath;
	other.m_id = 0;
	other.m_filepath = "nopath";
}