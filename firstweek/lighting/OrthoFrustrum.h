#pragma once

/* opengl */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../ErrorHandling.h"

/* openGL maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Contains an orthographic projection matrix. The projection matrix is recalculated only when changing
// some parameters of the frustrum.
class OrthoFrustrum
{
public:
	glm::mat4 projection;// = glm::ortho(left, right, down, up, nearPlane, farPlane);

private:
	float near ;
	float far  ;
	float left ;
	float right;
	float down ;
	float up   ;

public:
	OrthoFrustrum(float in_near, float in_far, float in_left, float in_right, float in_down, float in_up) :
	near(in_near), far(in_far), left(in_left), right(in_right), down(in_down), up(in_up)
	{
		computeProjectionMatrix();
	}

	void setNear(float);
	void setFar(float);
	void setLeft(float);
	void setRight(float);
	void setDown(float);
	void setUp(float);

	float getNear()  const { return near; }
	float getFar()   const { return far; }
	float getLeft()  const { return left; }
	float getRight() const { return right; }
	float getDown()  const { return down; }
	float getUp()    const { return up; }

private:
	void computeProjectionMatrix()
	{
		projection = glm::ortho(left, right, down, up, near, far);
	}
};