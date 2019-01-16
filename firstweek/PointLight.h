#pragma once

#include "Shader.h"

class PointLight
{
public:
	glm::vec3 eye;

	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

	struct Attenuation
	{
		float constant;
		float linear;
		float quadratic;
	} attenuation;

public:
	PointLight() : eye(0.0f),
		ambientColor(1.0f, 1.0f, 1.0f),
		diffuseColor(1.0f, 1.0f, 1.0f),
		specularColor(1.0f, 1.0f, 1.0f),
		attenuation({ 1.0f, 0.0f, 0.0f }) {}

	PointLight(glm::vec3 in_eye, 
		glm::vec3 in_ambientColor,
		glm::vec3 in_diffuseColor,
		glm::vec3 in_specularColor,
		float attenuationConstant,
		float attenuationLinear,
		float attenuationQuadratic) : eye(in_eye), 
		ambientColor(in_ambientColor),
		diffuseColor(in_diffuseColor),
		specularColor(in_specularColor),
		attenuation({ attenuationConstant, attenuationLinear, attenuationQuadratic }) {}

	void cast(const std::string& uniformName, Shader& shader);

	// Returns the matrix for transforming the coordinates of a vector in the reference frame of the light, with
	// the z axis pointing in the opposite direction of the light's direction.
	glm::mat4 getLightSpaceMatrix(const glm::mat4& frustrum_projection) const
	{
		//return  frustrum_projection * glm::lookAt(eye, center, glm::vec3{ 0.0f,1.0f,0.0f });
	}
};