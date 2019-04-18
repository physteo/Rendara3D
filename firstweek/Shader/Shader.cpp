#include "Shader.h"

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	bool geometryShaderExists = false;

	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
	};
	std::string line = "";
	std::stringstream ss[3];
	ss[0].str(std::string());
	ss[1].str(std::string());
	ss[2].str(std::string());

	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
			else if (line.find("geometry") != std::string::npos)
			{
				geometryShaderExists = true;
				type = ShaderType::GEOMETRY;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	
	ShaderProgramSource source = { ss[0].str(), ss[1].str(), geometryShaderExists ? ss[2].str() : "" };
	return source;
}

Shader::Shader(Shader&& other)
{
	swapData(other);
}
Shader& Shader::operator=(Shader&& other)
{
	if (this != &other)
	{
		release();
		swapData(other);
	}
	return *this;
}

void Shader::generate(const std::string& path)
{
	m_path = path;
	ShaderProgramSource source = ParseShader(m_path);

	std::string vertexShader = source.VertexSource;
	std::string fragmentShader = source.FragmentSource;
	std::string geometryShader = source.GeometrySource;

	/* create and compile vertex, fragment and geometry (if present) shaders */
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
	unsigned int gs = -1;
	if (geometryShader != "") {
		gs = compileShader(GL_GEOMETRY_SHADER, geometryShader);
	}

	/* create a program and attach the shaders */
	GLCall(m_id = glCreateProgram());

	GLCall(glAttachShader(m_id, vs));
	GLCall(glAttachShader(m_id, fs));
	if (geometryShader != "") {
		GLCall(glAttachShader(m_id, gs));

	}

	/* link the program */
	GLCall(glLinkProgram(m_id));

	/* validate the program */
	//GLCall(glValidateProgram(m_id)); //TODO: needed?

	/* now the shaders' memory can be freed */
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
	if (geometryShader != "") {
		GLCall(glDeleteShader(gs));

	}

	/* check the status of linking and print log if linking failed */
	GLint linked;
	GLCall(glGetProgramiv(m_id, GL_LINK_STATUS, &linked));
	if (linked == GL_FALSE)
	{
		// retrieve the length of the log
		GLint logLength;
		GLCall(glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &logLength));

		// retrieve the log
		char* log = (char*)alloca(logLength);
		GLCall(glGetProgramInfoLog(m_id, logLength, nullptr, log));

		// print the log
		std::cout << "Failure at linking program. Log: \n" << log << std::endl;
		throw - 1;
	}
}

void Shader::setTexture(GLenum target, const std::string& uniformName, unsigned int textureID)
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

unsigned int Shader::compileShader(unsigned int type, const std::string & shader)
{
	unsigned int s;
	const char* src = shader.c_str();
	GLCall(s = glCreateShader(type));
	GLint length = strlen(src);
	GLCall(glShaderSource(s, 1, &src, &length));
	GLCall(glCompileShader(s));

	GLint result;
	GLCall(glGetShaderiv(s, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{

		GLCall(glGetShaderiv(s, GL_INFO_LOG_LENGTH, &result));

		char* log = (char*)alloca(result);
		GLCall(glGetShaderInfoLog(s, result, nullptr, log));

		const char* typeToLog = "unspecified";
		switch (type) {
		case GL_VERTEX_SHADER: typeToLog = "vertex"; break;
		case GL_FRAGMENT_SHADER: typeToLog = "fragment"; break;
		case GL_GEOMETRY_SHADER: typeToLog = "geometry"; break;
		}

		std::cerr << "Failure at compiling " << typeToLog << " shader. Log: \n" << log << std::endl;
		return 0;
	}
	return s;
}

void Shader::release()
{
	GLCall(glDeleteProgram(m_id));
	m_id = 0;
	m_path = "";
	m_textureUnits.clear();
}

void Shader::swapData(Shader& other)
{
	m_id = other.m_id;
	m_path = other.m_path;
	//m_textureUnits.clear(); should already be cleared during "release" in the move assignment
	for (auto it = other.m_textureUnits.begin(); it != other.m_textureUnits.end(); ++it)
	{
		m_textureUnits.push_back(*it);
	}

	other.m_id = 0;
	other.m_path = "";
	other.m_textureUnits.clear();
}

void Shader::setUniformValue(const std::string& name, int          value) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniform1i(location, value));
}
void Shader::setUniformValue(const std::string& name, double       value) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniform1d(location, value));
}
void Shader::setUniformValue(const std::string& name, unsigned int value) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniform1ui(location, value));
}

void Shader::setUniformValue(const std::string& name, float        value) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniform1f(location, value));
}

void Shader::setUniformValue(const std::string & name, float v1, float v2) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniform2f(location, v1, v2));
}

void Shader::setUniformValue(const std::string & name, float v1, float v2, float v3) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniform3f(location, v1, v2, v3));
}

void Shader::setUniformValue(const std::string & name, float v1, float v2, float v3, float v4) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniform4f(location, v1, v2, v3, v4));
}

void Shader::setUniformValue(const std::string & name, glm::vec3 values) const
{
	setUniformValue(name, values.x, values.y, values.z);
}

void Shader::setUniformMatrix(const std::string& name, const glm::mat4& matrix, bool transpose) const
{
	int location;
	GLCall(location = glGetUniformLocation(m_id, name.c_str()));
	GLCall(glUniformMatrix4fv(location, 1, transpose, glm::value_ptr(matrix) ));
}
