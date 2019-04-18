#include "Camera.h"

glm::vec3 Camera::getCameraZ() const
{
	return glm::normalize(m_eye - m_center);
}

glm::vec3 Camera::getCameraY() const
{
	return glm::cross(getCameraZ(), getCameraX());
}

glm::vec3 Camera::getCameraX() const
{
	return glm::normalize(glm::cross(m_up, getCameraZ()));  //TODO: sure?
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(m_eye, m_center, m_up);
}


void Camera::processCommands(Window& window)
{
	float cameraSpeed = 12.5f * window.getLastFrameTime();
	GLFWwindow* glfwWin = window.getGLFWwindow();


	if (isKeyPressed(GLFW_KEY_W, glfwWin))
	{
		glm::vec3 toAdd = m_center * cameraSpeed;
		m_eye += toAdd;
	}

	if (isKeyPressed(GLFW_KEY_S, glfwWin))
	{
		glm::vec3 toAdd = m_center * cameraSpeed;
		m_eye += -toAdd;
	}


	if (isKeyPressed(GLFW_KEY_A, glfwWin))
	{
		glm::vec3 toAdd = getCameraX() * cameraSpeed;
		m_eye += -toAdd;
	}


	if (isKeyPressed(GLFW_KEY_D, glfwWin))
	{
		glm::vec3 toAdd = getCameraX() * cameraSpeed;
		m_eye += +toAdd;
	}

}