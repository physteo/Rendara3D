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
	glm::vec3 direction;
	glm::vec4 color;
	float tau;
};


