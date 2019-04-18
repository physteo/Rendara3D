#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Renderer/Transform.h"
#include "../../Model/Model.h"
#include "../../Window/inputs.h"

#include "../GameObject.h"



class Brick : public GameObject
{
public:
	bool destructible;
};


class Ball : public GameObject
{
public:
	glm::vec3 velocity;
	float     radius;

	// move the ball along its velocity vector
	void move(float dt);

	// check for collisions with bricks
	bool collisionWithRectangle(glm::vec3 center, float w, float h, float d);

	// check for collisions with walls
	bool collisionWithPlane(glm::vec3 normal, glm::vec3 offset);

	// set the position
	void reset(const glm::vec3& defaultPosition);

	void processCommands(Window& window, float delta, float speed)
	{
		float speedMultiplier = speed * delta;

		if (isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		{
			//glm::vec3 toAdd = +glm::vec3{ 0.0f,0.0f,1.0f } *speedMultiplier;
			//this->transform.position += toAdd;
			this->velocity.z = abs(this->velocity.z);
		}


		if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
		{
			//glm::vec3 toAdd = +glm::vec3{ 0.0f,0.0f,1.0f } *speedMultiplier;
			//this->transform.position += toAdd;
			this->velocity.z = -abs(this->velocity.z);
		}

		if (isKeyPressed(GLFW_KEY_UP, window.getGLFWwindow()))
		{
			float sign_z = abs(velocity.z) / (double)velocity.z;
			float sign_x = abs(velocity.x) / (double)velocity.x;
			float modulus = glm::length(this->velocity);
			this->velocity.z = sign_z * 0.7071*modulus;
			this->velocity.x = sign_x * 0.7071*modulus;
		}


		if (isKeyPressed(GLFW_KEY_DOWN, window.getGLFWwindow()))
		{
			//glm::vec3 toAdd = +glm::vec3{ 0.0f,0.0f,1.0f } *speedMultiplier;
			//this->transform.position += toAdd;
			float sign_z = abs(velocity.z) / (double)velocity.z;
			float sign_x = abs(velocity.x) / (double)velocity.x;
			float modulus = glm::length(this->velocity);
			this->velocity.z = sign_z * 0.7071*modulus;
			this->velocity.x = sign_x * 0.7071*modulus;
		}

	}

};


class Player : public GameObject
{
public:
	void processCommands(Window& window, float delta, float t, float speed)
	{
		float speedMultiplier = speed * delta;
		this->transform.position = glm::vec3{ 10.0f,0.0f,1.25f };
		//glm::vec3 toAdd = glm::vec3{ 0.0f,0.0f,1.0f } *speedMultiplier;
		this->transform.position += (5.0f*sin(t) + 5.0f ) * glm::vec3{0.0f,0.0f,1.0f};

		//if (isKeyPressed(GLFW_KEY_UP, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = -glm::vec3{ 1.0f,0.0f,0.0f } *speedMultiplier;
		//	this->transform.position += toAdd;
		//}
		//
		//if (isKeyPressed(GLFW_KEY_DOWN, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = +glm::vec3{ 1.0f,0.0f,0.0f } *speedMultiplier;
		//	this->transform.position += toAdd;
		//}
		//
		//if (isKeyPressed(GLFW_KEY_LEFT, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = +glm::vec3{ 0.0f,0.0f,1.0f } *speedMultiplier;
		//	this->transform.position += toAdd;
		//}
		//
		//
		//if (isKeyPressed(GLFW_KEY_RIGHT, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = -glm::vec3{ 0.0f,0.0f,1.0f } *speedMultiplier;
		//	this->transform.position += toAdd;
		//}

		//if (isKeyPressed(GLFW_KEY_O, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = glm::vec3{ 0.0f,1.0f,1.0f } *speedMultiplier;
		//	this->transform.position += toAdd;
		//}
		//
		//
		//if (isKeyPressed(GLFW_KEY_L, window.getGLFWwindow()))
		//{
		//	glm::vec3 toAdd = -glm::vec3{ 0.0f,0.0f,1.0f } *speedMultiplier;
		//	this->transform.position += toAdd;
		//}

	}
};

