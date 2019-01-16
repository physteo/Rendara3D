#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window.h"


static bool isKeyPressed(int key, Window& window)
{
	return (glfwGetKey(window.getGLFWwindow(), key) == GLFW_PRESS);
}

static bool isKeyPressed(int key, GLFWwindow* window)
{
	return (glfwGetKey(window, key) == GLFW_PRESS);
}