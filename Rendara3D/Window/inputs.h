#pragma once
/* opengl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Rendering Engine */
#include "./Window.h"


static bool isKeyPressed(int key, Window& window)
{
	return (glfwGetKey(window.getGLFWwindow(), key) == GLFW_PRESS);
}

static bool isKeyPressed(int key, GLFWwindow* window)
{
	return (glfwGetKey(window, key) == GLFW_PRESS);
}

static void controlVector(Window& window, float multiplier,
	int key_xcoord_up, int key_xcoord_down, 
	int key_ycoord_up, int key_ycoord_down, 
	int key_zcoord_up, int key_zcoord_down, glm::vec3& vectorToModify)
{
	float dt = window.getLastFrameTime();

	if (isKeyPressed(key_xcoord_up, window))
	{
		vectorToModify.x += multiplier * dt;
	}
	if (isKeyPressed(key_xcoord_down, window))
	{
		vectorToModify.x -= multiplier * dt;
	}
	if (isKeyPressed(key_ycoord_up, window))
	{
		vectorToModify.y += multiplier * dt;
	}
	if (isKeyPressed(key_ycoord_down, window))
	{
		vectorToModify.y -= multiplier * dt;
	}
	if (isKeyPressed(key_zcoord_up, window))
	{
		vectorToModify.z += multiplier * dt;
	}
	if (isKeyPressed(key_zcoord_down, window))
	{
		vectorToModify.z -= multiplier * dt;
	}
}