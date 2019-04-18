#pragma once

#include <iostream>

#include<GL/glew.h>
#include<GLFW/glfw3.h>


#include "../utils/ErrorHandling.h"
#include "../utils/ImageLoader.h"
#include "../Shader/Shader.h"

enum Format
{
	RGB = GL_RGB,
	RGBA = GL_RGBA,
	sRGB = GL_SRGB,
	sRGBA = GL_SRGB_ALPHA,
	RGBA16=GL_RGBA16
};



//! Class for 2D textures.
/*!
	Generates a texture and stores its ID, and the path to the image file. 
	After instantiation, one needs to use \ref Texture.generate method to initialize
	the texture. The default path of the texture is "nopath", which means the texture
	is empty.
*/
class Texture
{
public:
	Texture() : m_id(0), m_filepath("nopath") {}
	//!<  Create the texture and fills it with pixel data of the specified path
	Texture(const std::string& filepath, Format internalFormat);
	//!< Creates the texture and fills it with pixel data manually specified 
	Texture(int width, int height, int nrChannels, const std::string& filepath, Format internalFormat, const unsigned char* data);
	//!< Creates the texture and fills it with pixel data manually specified, and specified data format
	Texture(int width, int height, int nrChannels, const std::string& filepath, Format internalFormat, GLenum dataFormat, const unsigned char* data);	
	//!< Creates the texture and fills it with pixel data manually specified, and specified data format
	Texture(int internalFormat, int width, int height, const std::string& filepath, GLenum format, GLenum dataformat, const unsigned char* data);


	
	//!< Can't copy textures
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	//!< Can move textures
	Texture(Texture&& other);
	Texture& operator=(Texture&& other);

	~Texture();

	//!< activates this texture.
	void bind()   const { GLCall(glBindTexture(GL_TEXTURE_2D, m_id)); } 
	//!< deactivates any texture.
	void unbind() const { GLCall(glBindTexture(GL_TEXTURE_2D,    0)); } 
	
	//!< retrieves ID of texture (used when passing a texture to a shader)
	unsigned int getID() const { return m_id; } 
	
	//!< retrieves path of the image
	inline const std::string& getPath() const { return m_filepath; } 

	//!< Set general opengl parameters for 2D textures 
	void set2DTextureParameter(GLenum pname, GLint param);
	//!< Set general opengl parameters for 2D textures
	void set2DTextureParameter(GLenum pname, GLfloat param);
	//!< Set general opengl parameters for 2D textures
	void set2DTextureParameter(GLenum pname, GLfloat* param);

	static void setActiveTexture(GLenum texture) { GLCall(glActiveTexture(texture)); }

private:
	unsigned int m_id;
	std::string  m_filepath;
	
	void generate(const std::string& filepath, Format internalFormat); 
	void generate(int width, int height, int nrChannels, Format internalFormat, const unsigned char* data); 
	void generate(int width, int height, int nrChannels, Format internalFormat, GLenum dataFormat, const unsigned char* data);

	void release();
	void swapData(Texture& other);
};

//! Class representing a Material as a collection of Textures and shininess.
/*!
	Stores pointer to textures for: diffusive, specular, normal(bump) light calculations, and the shininess affecting 
	the specular light. If constructed with default constructor \ref Material.Material, needs to be filled with \ref Material.fill.
*/
class Material
{
private:
	const Texture* m_diffuse;    //!< Texture for diffusive lighting
	const Texture* m_specular;	  //!< Texture for specular lighting
	const Texture* m_normal;	  //!< Texture for normal(bump) lighting
	float   m_shininess;  //!< shininess parameter used for specular lighting.

public:
	Material() : m_diffuse{}, m_specular{}, m_normal(), m_shininess(32.0f) {}
	Material(const Texture* diffuse, const Texture* specular, const Texture* normal, float shininess);
	void fill(const Texture* diffuse, const Texture* specular, const Texture* normal, float shininess);

	//!< Passes to the shader all the needed texture IDs and shininess parameter.
	void passUniforms(Shader& shader) const; 
};