#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ErrorHandling.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string GeometrySource;
};
static ShaderProgramSource ParseShader(const std::string& filepath);


class Shader
{
private:
	unsigned int m_id;
	std::string  m_path;

	//std::unordered_map<std::string, size_t> m_textureUnits;
	std::vector<std::string> m_textureUnits;


public:
	Shader() : m_id(-1), m_path("") {};

	void generate(const std::string& path);
	

	void bind()   const { GLCall(glUseProgram(m_id)); }
	void unbind() const { GLCall(glUseProgram(0)); }

	void setUniformValue(const std::string& name, int          value) const;
	void setUniformValue(const std::string& name, double       value) const;
	void setUniformValue(const std::string& name, unsigned int value) const;
		 
	void setUniformValue(const std::string& name, float value) const;
	void setUniformValue(const std::string& name, float v1, float v2) const;
	void setUniformValue(const std::string& name, float v1, float v2, float v3) const;
	void setUniformValue(const std::string& name, float v1, float v2, float v3, float v4) const;

	void setUniformValue(const std::string& name, glm::vec3) const;
	void setUniformMatrix(const std::string& name, const glm::mat4& matrix, bool transpose) const;

	void setTexture(GLenum target, const std::string& uniformName, unsigned int textureID)
	{
		std::vector<std::string>::iterator it = std::find(m_textureUnits.begin(), m_textureUnits.end(), uniformName);
		int textureUnit;
		if (it == m_textureUnits.end())
		{
			// new texture
			textureUnit = m_textureUnits.size();
			setUniformValue(uniformName, textureUnit);
			m_textureUnits.push_back(uniformName);
		}
		else
		{
			// texture unit already exists, dont set the uniform
			textureUnit = it - m_textureUnits.begin();
		}
		
		GLCall(glActiveTexture(GL_TEXTURE0 + textureUnit));
		GLCall(glBindTexture(target, textureID));
	}

private:
	unsigned int compileShader(unsigned int type, const std::string& shader);	
};