#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Transform.h"
#include "../Model.h"
#include "../inputs.h"


class GameObject
{
public:
	Transform transform;
	Model* model;
};


class Brick : public GameObject
{
public:
	bool destructible;
};


class Ball : public GameObject
{
public:
	glm::vec3 velocity;

	float radius;

	// move the ball along its velocity vector
	void move(float dt);

	// check for collisions with bricks
	bool collisionWithRectangle(glm::vec3 center, float w, float h, float d);

	// check for collisions with bricks
	bool collisionWithPlane(glm::vec3 normal, glm::vec3 offset);

	// set the position
	void reset(const glm::vec3& defaultPosition);

};


class Player : public GameObject
{
public:
	void processCommands(Window& window, float delta, float speed)
	{
		float speedMultiplier = speed * delta;

		//if (isKeyPressed(GLFW_KEY_UP, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = glm::vec3{-1.0f,0.0f,0.0f} *speedMultiplier;
		//	this->position += toAdd;
		//}
		//
		//if (isKeyPressed(GLFW_KEY_DOWN, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = glm::vec3{1.0f,0.0f,0.0f } * speedMultiplier;
		//	this->position += toAdd;
		//}


		if (isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = glm::vec3{ 0.0f,0.0f,1.0f } * speedMultiplier;
			this->transform.position += toAdd;
		}


		if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
		{
			glm::vec3 toAdd = glm::vec3{ 0.0f,0.0f,-1.0f } * speedMultiplier;
			this->transform.position += toAdd;
		}

	}
};