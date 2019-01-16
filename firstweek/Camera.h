#pragma once

#include "Window.h"
#include "inputs.h"

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	glm::vec3 m_eye;
	glm::vec3 m_center;
	glm::vec3 m_up;

public:

	Camera() {}
	Camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) : m_eye(eye), m_center(center), m_up(up) {}

	void setEye	  (const glm::vec3& eye	  ) { m_eye = eye; }
	void setCenter(const glm::vec3& center) { m_center = center; }
	void setUp	  (const glm::vec3& up	  ) { m_up = up; };

	const glm::vec3& getEye		() const { return m_eye; }
	const glm::vec3& getCenter	() const { return m_center; }
	const glm::vec3& getUp		() const { return m_up; }


	glm::vec3 getCameraZ() const;
	glm::vec3 getCameraY() const;
	glm::vec3 getCameraX() const;

	glm::mat4 getViewMatrix() const;

	void processCommands(Window& window);
	void processCommands(GLFWwindow *window, float delta);

};
