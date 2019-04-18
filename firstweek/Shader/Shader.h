#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../utils/ErrorHandling.h"
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
	Shader() : m_id(0), m_path("") {};
	Shader(const std::string& path) : m_id(0), m_path("")
	{
		generate(path);
	};

	//Cannot use the copy constructor/assignment.
	Shader(const Shader &) = delete;
	Shader& operator=(const Shader &) = delete;

	//Can use move constructor/assignment.
	Shader(Shader&& other);
	Shader& operator=(Shader&& other);

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

	void setTexture(GLenum target, const std::string& uniformName, unsigned int textureID);

private:
	void generate(const std::string& path);
	unsigned int compileShader(unsigned int type, const std::string& shader);	

	void release();
	void swapData(Shader& other);
};