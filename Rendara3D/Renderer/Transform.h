#pragma once

#include <glm/glm.hpp>

struct Transform
{
	Transform() : position(0.0f), rotation(0.0f), scale(1.0f) {}
	Transform(const glm::vec3& positionIn,
		const glm::vec3& rotationIn,
		const glm::vec3& scaleIn ) : position(positionIn), rotation(rotationIn), scale(scaleIn) {}

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};