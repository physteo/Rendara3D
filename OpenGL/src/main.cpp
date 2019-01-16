
#include<iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <string>
#include <sstream>

#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x)\
	{GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__));}

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL error] (" << error << "):" << 
			function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void window_resize_callback(GLFWwindow* window, int width, int height)
{
	GLCall(glViewport(0, 0, width, height));
}

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::string line;
	std::stringstream ss[2];
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
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source )
{
	unsigned int id;
	const char* src;
	GLCall(id = glCreateShader(type));
	GLCall(src = source.c_str());
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, nullptr, message));
		std::cout << "Failed to compile "<< 
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << 
		" shader." << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	// Provide OpenGL with shader source code and compile the shaders
	unsigned int program, vs, fs;
	GLCall(program = glCreateProgram());
	GLCall(vs = CompileShader(GL_VERTEX_SHADER, vertexShader));
	GLCall(fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));
	// Link the two shaders together into a "program"
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// delete intermediate shaders
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	// GIve a shaderID back to the caller so that it can be binded
	return program;
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* specify the version and if core/compatibility mode */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// sinchronize to our VSync
	glfwSwapInterval(1);

	glViewport(0, 0, 640, 480);
	glfwSetFramebufferSizeCallback(window, window_resize_callback);

	/* initialize GLEW */
	GLenum err = glewInit();
	if (err != GLEW_OK)
		std::cout << "NOPE" << std::endl;
	
	/* Print current version of OpenGL */
	const GLubyte* version; GLCall(version = glGetString(GL_VERSION));
	std::cout << version << std::endl;

	float positions[] = {
		-0.5f, -0.5f,
		+0.5f, -0.5f,
		+0.5f, +0.5f,
		-0.5f, +0.5f
	};

	unsigned int indices[] = {
		0, 1, 2, 2, 3, 0
	};

	unsigned int bufferID;
	GLCall(glGenBuffers(1, &bufferID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW));

	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// this command actually bind the buffer to the vertex array and specifies its layout.
	// now I dont need to bind the buffer AND specify the layout before each draw call,
	// but I just need to bind the vao before each draw call 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// unbind vao and then buffer
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// index buffer object
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));
	// Unbind
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);




	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);

	GLCall(glUseProgram(shader));
	// after creating the program, each uniform has an index assigned 
	// we have to retrieve it to assign it a value. We could also set it
	// to whatever we want but it is only OpenGL 4.3 above.
	int location;
	GLCall(location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	GLCall(glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f));
	glUseProgram(0);






	float r = 0.0f;
	float increment = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{

		/* Render here */
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		// bind what we need
		glUseProgram(shader);
		//glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		//GLCall(glEnableVertexAttribArray(0));
		//GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);


		GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		
		if (r > 1.0 || r < 0.0)
			increment *= -1;
		r += increment;


		// unbind
		glUseProgram(0);
		glBindVertexArray(0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		/* Swap front and back buffers */
		GLCall(glfwSwapBuffers(window));

		/* Poll for and process events */
		GLCall(glfwPollEvents());
	}

	GLCall(glDeleteProgram(shader));

	glfwTerminate();
	return 0;

}