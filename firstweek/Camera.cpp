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


	if (isKeyPressed(GLFW_KEY_W, window))
	{
		glm::vec3 toAdd = m_center * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += toAdd;
	}

	if (isKeyPressed(GLFW_KEY_S, window))
	{
		glm::vec3 toAdd = m_center * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += -toAdd;
	}


	if (isKeyPressed(GLFW_KEY_A, window))
	{
		glm::vec3 toAdd = getCameraX() * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += -toAdd;
	}


	if (isKeyPressed(GLFW_KEY_D, window))
	{
		glm::vec3 toAdd = getCameraX() * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += +toAdd;
	}

}


void Camera::processCommands(GLFWwindow *window, float delta)
{
	float cameraSpeed = 12.5f * delta;//window.getLastFrameTime();


	if (isKeyPressed(GLFW_KEY_W, window))
	{
		glm::vec3 toAdd = m_center * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += toAdd;
	}

	if (isKeyPressed(GLFW_KEY_S, window))
	{
		glm::vec3 toAdd = m_center * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += -toAdd;
	}


	if (isKeyPressed(GLFW_KEY_A, window))
	{
		glm::vec3 toAdd = getCameraX() * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += -toAdd;
	}


	if (isKeyPressed(GLFW_KEY_D, window))
	{
		glm::vec3 toAdd = getCameraX() * cameraSpeed;//getCameraZ() * cameraSpeed;
		m_eye += +toAdd;
	}

}