#pragma once

#include <iostream>

#include<GL/glew.h>
#include<GLFW/glfw3.h>


#include "ErrorHandling.h"
#include "ImageLoader.h"
#include "Shader.h"

enum Format
{
	RGB = GL_RGB,
	RGBA = GL_RGBA,
	sRGB = GL_SRGB,
	sRGBA= GL_SRGB_ALPHA
};

class Texture
{
private:
	std::string  m_filepath;
	unsigned int m_id;

public:
	Texture() : m_filepath("nopath") {}
	void generate(const std::string& filepath, Format internalFormat);
	void generate(int width, int height, int nrChannels, Format internalFormat, const unsigned char* data);

	void bind()   const { GLCall(glBindTexture(GL_TEXTURE_2D, m_id)); }
	void unbind() const { GLCall(glBindTexture(GL_TEXTURE_2D,    0)); }
	
	unsigned int getID() const { return m_id; }
	
	inline const std::string& getPath() const { return m_filepath; }
	//inline const std::string* getPath() const { return &m_filepath; }

	static void set2DTextureParameter(GLenum pname, GLint param)    { GLCall(glTexParameteri(GL_TEXTURE_2D, pname, param)); }
	static void set2DTextureParameter(GLenum pname, GLfloat param)  { GLCall(glTexParameterf(GL_TEXTURE_2D, pname, param)); }
	static void set2DTextureParameter(GLenum pname, GLfloat* param) { GLCall(glTexParameterfv(GL_TEXTURE_2D, pname, param)); }

	static void setActiveTexture(GLenum texture) { GLCall(glActiveTexture(texture)); }
	
};

class Material
{
private:
	Texture m_diffuse;
	Texture m_specular;
	Texture m_normal;
	float   m_shininess;

public:
	Material() : m_diffuse{}, m_specular{}, m_normal(), m_shininess(32.0f) {}
	Material(const Texture& diffuse, const Texture& specular, const Texture& normal, float shininess);
	void fill(const Texture& diffuse, const Texture& specular, const Texture& normal, float shininess);

	void passUniforms(Shader& shader);
};