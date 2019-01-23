
#include "Window.h"

Window::Window(std::string title, double width, double height) : Window{ title, width, height, Monitor::G_NOTSPECIFIED } {}

Window::Window(std::string title, double width, double height, Monitor monitor)
{
	m_lastTime = getCurrentTime();
	m_lastFrameTime = 0.0f;

	m_width = width;
	m_height = height;
	/* initialize the glfw library */
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW." << std::endl;
		throw - 1; //TODO exception
	}
	/* get primary monitor */
	int numberOfMonitors;
	GLFWmonitor** monitors = glfwGetMonitors(&numberOfMonitors);

	/* specify the version and if core/compatibility mode */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// GLFW_OPENGL_CORE_PROFILE); 

	/* Create a windowed mode window and its OpenGL context */
	if(monitor == Monitor::G_NOTSPECIFIED)
		m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	else
		m_window = glfwCreateWindow(width, height, title.c_str(), monitors[monitor], nullptr);

	if (!m_window)
	{
		std::cerr << "Failed to create GLFWwindow. Terminating GLFW." << std::endl;
		glfwTerminate();
		throw -1;
	}

	/* Make the window's current context */
	glfwMakeContextCurrent(m_window);

	/* Once context is created, I can initialize GLEW*/
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		/* failed to initialize GLEW */
		std::cerr << "Failed to initialize GLEW: terminating the GLFW." << std::endl;
		glfwTerminate();
		throw -1;
	}


	// configure global opengl state
	// -----------------------------
	GLCall(glEnable(GL_DEPTH_TEST));
	//ste//GLCall(glDepthFunc(GL_LESS));
	//ste//GLCall(glEnable(GL_STENCIL_TEST));
	//ste//GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
	//ste//GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

	glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CCW);
	//glEnable(GL_FRAMEBUFFER_SRGB);

	// set the viewport
	setViewPort(m_width, m_height);

	std::cout << "OpenGL: version " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLFW:   version " << glfwGetVersionString() << std::endl;
	std::cout << "GLEW:   version " << glewGetString(GLEW_VERSION) << std::endl;
}

void Window::setViewPort(float width, float height) const
{
	GLCall(glViewport(0, 0, width, height));
}



bool Window::isClosed() const
{
	return glfwWindowShouldClose(m_window);
}

void Window::clearColorBufferBit(float red, float green, float blue, float alpha) const
{
	GLCall(glClearColor(red, green, blue, alpha));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	//ste//GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void Window::swapBuffers() const
{
	glfwSwapBuffers(m_window);
}

void Window::pollEvents() const
{
	glfwPollEvents();
}

void Window::terminate() const
{
	glfwTerminate();
}

const GLFWwindow* Window::getGLFWwindow() const
{ 
	return m_window;
}

GLFWwindow* Window::getGLFWwindow()
{ 
	return m_window;
}

float Window::getLastTime() const
{
	return m_lastTime;
}
float Window::getLastFrameTime() const
{
	return m_lastFrameTime;
}
float Window::getCurrentTime() const
{
	return glfwGetTime();
}
void  Window::updateTime()
{
	m_lastTime = getCurrentTime();
}
void Window::updateLastFrameTime()
{
	m_lastFrameTime = getCurrentTime() - m_lastTime;
}
