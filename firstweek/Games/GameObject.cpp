#include "GameObject.h"

void Ball::move(float dt)
{
	this->transform.position += this->velocity * dt;
}

void Ball::reset(const glm::vec3& defaultPosition)
{
	this->transform.position = defaultPosition;
}

bool Ball::collisionWithRectangle(glm::vec3 rectanglePos, float w, float h, float d)
{

	// Get center point circle first 
	glm::vec3 center = this->transform.position;

	// Calculate AABB info (center, half-extents)
	glm::vec3 aabb_half_extents(w / 2, h / 2, d / 2);
	glm::vec3 aabb_center(
		rectanglePos.x,
		rectanglePos.y,
		rectanglePos.z
	);
	// Get difference vector between both centers
	glm::vec3 difference = center - aabb_center;
	glm::vec3 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	// Add clamped value to AABB_center and we get the value of box closest to circle
	glm::vec3 closest = aabb_center + clamped;
	// Retrieve vector between center circle and closest point AABB and check if length <= radius
	difference = closest - center;
	float length_difference = glm::length(difference);
	if (length_difference < this->radius)
	{
		glm::vec3 minus_normal = difference / length_difference;
		// resolve collision for the ball
		// repositioning
		float offset = 1.0 * (this->radius - length_difference);
		this->transform.position -= minus_normal * offset;
		// change velocity
		float speed = glm::length(this->velocity);
		glm::vec3 norm_velocity = this->velocity / speed;

		glm::vec3 normal = -minus_normal;
		this->velocity = speed * (norm_velocity - 2.0f * (glm::dot(normal, norm_velocity)) * normal);

		return true;
	}
	else
	{
		return false;
	}

}


bool Ball::collisionWithPlane(glm::vec3 normal, glm::vec3 offset)
{
	normal = glm::normalize(normal);

	float signed_distance = glm::dot(this->transform.position - offset, normal);

	if ( signed_distance * signed_distance < radius * radius)
	{
		// closest point
		glm::vec3 closest = this->transform.position - signed_distance * normal;

		glm::vec3 from_center_to_closest = closest - this->transform.position;

		glm::vec3 offset = (from_center_to_closest / glm::length(from_center_to_closest)) * radius 
			- from_center_to_closest;
		//
		//// relocate
		this->transform.position -= offset;

		// change velocity
		float speed = glm::length(this->velocity);
		glm::vec3 norm_velocity = this->velocity / speed;
		this->velocity = speed * (norm_velocity - 2.0f * (glm::dot(normal, norm_velocity)) * normal);
	}
	else
	{
		return false;
	}

}




