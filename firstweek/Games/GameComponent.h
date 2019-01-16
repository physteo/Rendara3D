#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "../Model.h"
#include "../inputs.h"

class GameComponent
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 velocity;
	float scale;

	Model* model;
};


class Player : public GameComponent
{
public:
	void processCommands(Window& window, float delta, float speed)
	{
		float speedMultiplier = speed * delta;

		if (isKeyPressed(GLFW_KEY_UP, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = glm::vec3{-1.0f,0.0f,0.0f} *speedMultiplier;
			this->position += toAdd;
		}

		if (isKeyPressed(GLFW_KEY_DOWN, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = glm::vec3{1.0f,0.0f,0.0f } * speedMultiplier;
			this->position += toAdd;
		}


		if (isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = glm::vec3{ 0.0f,0.0f,1.0f } * speedMultiplier;
			this->position += toAdd;
		}


		if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = glm::vec3{ 0.0f,0.0f,-1.0f } * speedMultiplier;
			this->position += toAdd;
		}

	}
};