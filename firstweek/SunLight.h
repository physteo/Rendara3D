#pragma once

/* opengl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ErrorHandling.h"

/* openGL maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* stl */
#include <iostream>

#include "Shader.h"
#include "OrthoFrustrum.h"

// Directional light. It contains three colors for ambient, diffuse, specular lighting.
// The direction is specified as the difference between "center" and "eye". The position
// of the light (eye) is used only when casting shadows.
class SunLight
{
public:
	glm::vec3 eye;
	glm::vec3 center;
	glm::vec3 ambientColor;
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;

public:
	SunLight() : eye(0.0f), center(0.0f, -1.0f, 0.0f), 
		ambientColor(1.0f,1.0f,1.0f), 
		diffuseColor(1.0f,1.0f,1.0f), 
		specularColor(1.0f,1.0f,1.0f) {}

	SunLight(glm::vec3 in_eye, glm::vec3 in_center,	
		glm::vec3 in_ambientColor,	
		glm::vec3 in_diffuseColor,
		glm::vec3 in_specularColor) : eye(in_eye), center(in_center),
		ambientColor(in_ambientColor),
		diffuseColor(in_diffuseColor),
		specularColor(in_specularColor) {}

	// Binds the shader, sends the uniforms as uniformName+"diffuse", uniformName+"ambient",
	// uniformName+"specular". At the end, unbinds the shader.
	void cast(const std::string& uniformName, Shader& shader);

	// Returns the matrix for transforming the coordinates of a vector in the reference frame of the light, with
	// the z axis pointing in the opposite direction of the light's direction.
	glm::mat4 getViewMatrix() const
	{
		return  glm::lookAt(eye, center, glm::vec3{ 0.0f,1.0f,0.0f });
	}

};