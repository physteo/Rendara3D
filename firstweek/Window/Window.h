#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../utils/ErrorHandling.h"

#include <iostream>
#include <exception>
#include <stdexcept>

#define MAX_KEYS 1024
#define MAX_BUTTONS 32

//! Specify the monitor to use. To be used when creating the Window.
enum Monitor
{
	G_PRIMARY = 0,
	G_SECONDARY = 1,
	G_NOTSPECIFIED = -1
};

//! Class for the window on which things will be rendered.
/*!
	Creates a window for rendering stuff on the screen. Must be created before anything else. 
*/
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

	bool m_Keys[MAX_KEYS];
	bool m_MouseButtons[MAX_BUTTONS];
	double mx, my;

public:
	/*!
	Width and Height must be specified in pixels.
	*/
	Window(std::string title, double width /* in pixels */, double height);
	/*!
	Width and Height must be specified in pixels. The Monitor can be either G_PRIMARY or G_SECONDARY or G_NOTSPECIFIED. G_NOTSPECIFIED will take the default primary
	monitor.
	*/
	Window(std::string title, double width /* in pixels */, double height, Monitor monitor);


	//!< Checks whether the window was closed.
	bool isClosed() const;
	//!< Closes the window.
	void close() const { glfwSetWindowShouldClose(m_window, true); }
	void setViewPort(float width, float height) const;
	//!< Set the "background color" of the currently binded buffer. Use numbers between 0 and 1 (not 0-255!).
	void clearColorBufferBit(float red, float blue, float green, float alpha) const;
	void swapBuffers() const;
	void pollEvents() const;
	void terminate() const;

	void setWidth(double width)   { m_width  = width; }
	void setHeight(double height) { m_height = height; }

	double getWidth() const { return m_width; }
	double getHeight() const { return m_height; }
	const GLFWwindow* getGLFWwindow() const;
	GLFWwindow*		  getGLFWwindow();

	//!< Gets the latest stored time (stored when using updateTime
	float getLastTime() const;
	//!< Gets the time it took to process the previous frame (in seconds). 
	float getLastFrameTime() const;
	//!< Gets the real time, measured from the start of the program (in seconds) 
	float getCurrentTime() const;
	//!< Updates the stored time (in seconds) with the real time.
	void  updateTime();
	//!< Update the time it took to process this frame (in seconds). To be used at the end of each frame.
	void  updateLastFrameTime();
};
