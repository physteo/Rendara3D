#pragma once

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "../Renderer/Transform.h"

struct Particle
{
	Transform transform;
	glm::vec3 velocity;
	glm::vec4 color;
	float tau;

	glm::vec3 direction;
//	Particle() : transform.position{ 0.0f }, velocity{ 0.0f }, color{ 1.0f }, tau{ 0.0f } {}
//	Particle(const glm::vec3& positionIn, const glm::vec3& velocityIn, const glm::vec3& colorIn, float tauIn) : 
//		position{ positionIn }, velocity{ velocityIn }, color{ colorIn }, tau{ tauIn } {}

};


