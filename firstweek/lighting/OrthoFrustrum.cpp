#include "OrthoFrustrum.h"

void OrthoFrustrum::setNear(float in_near)
{
	near = in_near;
	computeProjectionMatrix();
}
void OrthoFrustrum::setFar(float in_far)
{
	far = in_far;
	computeProjectionMatrix();
}
void OrthoFrustrum::setLeft(float in_left)
{
	left = in_left;
	computeProjectionMatrix();
}
void OrthoFrustrum::setRight(float in_right)
{
	right = in_right;
	computeProjectionMatrix();
}
void OrthoFrustrum::setDown(float in_down)
{
	down = in_down;
	computeProjectionMatrix();
}
void OrthoFrustrum::setUp(float in_up)
{
	up = in_up;
	computeProjectionMatrix();
}
