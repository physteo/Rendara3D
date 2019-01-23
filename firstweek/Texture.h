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



//! Class for 2D textures.
/*!
	Generates a texture and stores its ID, and the path to the image file. 
	After instantiation, one needs to use \ref Texture.generate method to initialize
	the texture.
*/
class Texture
{
public:
	Texture() : m_filepath("nopath") {}

	//!<  Allocates memory for the texture and fills it with pixel data of the specified path
	void generate(const std::string& filepath, Format internalFormat); 
	//!< Allocates memory for the texture and fills it with pixel data manually specified 
	void generate(int width, int height, int nrChannels, Format internalFormat, const unsigned char* data); 
	//!< activates thistexture.
	void bind()   const { GLCall(glBindTexture(GL_TEXTURE_2D, m_id)); } 
	//!< deactivates any texture.
	void unbind() const { GLCall(glBindTexture(GL_TEXTURE_2D,    0)); } 
	
	//!< retrieves ID of texture (used when passing a texture to a shader)
	unsigned int getID() const { return m_id; } 
	
	//!< retrieves path of the image
	inline const std::string& getPath() const { return m_filepath; } 

	//!< Set general opengl parameters for 2D textures 
	static void set2DTextureParameter(GLenum pname, GLint param)    { GLCall(glTexParameteri(GL_TEXTURE_2D, pname, param)); }  
	//!< Set general opengl parameters for 2D textures
	static void set2DTextureParameter(GLenum pname, GLfloat param)  { GLCall(glTexParameterf(GL_TEXTURE_2D, pname, param)); }
	//!< Set general opengl parameters for 2D textures
	static void set2DTextureParameter(GLenum pname, GLfloat* param) { GLCall(glTexParameterfv(GL_TEXTURE_2D, pname, param)); } 

	static void setActiveTexture(GLenum texture) { GLCall(glActiveTexture(texture)); }

private:
	std::string  m_filepath;
	unsigned int m_id;
	
};

//! Class representing a Material as a collection of Textures and shininess.
/*!
	Stores three textures for: diffusive, specular, normal(bump) light calculations, and the shininess affecting 
	the specular light. If constructed with default constructor \ref Material.Material, needs to be filled with \ref Material.fill.
*/
class Material
{
private:
	Texture m_diffuse;    //!< Texture for diffusive lighting
	Texture m_specular;	  //!< Texture for specular lighting
	Texture m_normal;	  //!< Texture for normal(bump) lighting
	float   m_shininess;  //!< shininess parameter used for specular lighting.

public:
	Material() : m_diffuse{}, m_specular{}, m_normal(), m_shininess(32.0f) {}
	Material(const Texture& diffuse, const Texture& specular, const Texture& normal, float shininess);
	void fill(const Texture& diffuse, const Texture& specular, const Texture& normal, float shininess);

	//!< Passes to the shader all the needed texture IDs and shininess parameter.
	void passUniforms(Shader& shader) const; 
};