#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ErrorHandling.h"

#include <iostream>
#include <exception>
#include <stdexcept>

enum Monitor
{
	G_PRIMARY = 0,
	G_SECONDARY = 1,
	G_NOTSPECIFIED = -1
};

class Window
{
private:
	GLFWwindow*  m_window;
	double		 m_width;
	double		 m_height;
	GLFWmonitor* m_monitor;
	bool		 m_fullscreen;

	float m_lastTime;
	float m_lastFrameTime;

public:
	Window(std::string title, double width, double height, Monitor monitor);

	void setViewPort(float width, float height) const;
	bool isClosed() const;
	void close() const { glfwSetWindowShouldClose(m_window, true); }
	void clearColorBufferBit(float v0, float v1, float v2, float v3) const;
	void swapBuffers() const;
	void pollEvents() const;
	void terminate() const;

	double getWidth() const { return m_width; }
	double getHeight() const { return m_height; }
	const GLFWwindow* getGLFWwindow() const;
	GLFWwindow*		  getGLFWwindow();

	float getLastTime() const;
	float getLastFrameTime() const;
	float getCurrentTime() const;
	void  updateTime();
	void  updateLastFrameTime();
};
