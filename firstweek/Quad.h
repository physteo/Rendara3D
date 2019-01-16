#pragma once

#include <iostream>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include "ErrorHandling.h"

class Quad
{
	unsigned int quadVAOi;

public:
	Quad()
	{
		unsigned int quadVBOi;
		float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		GLCall(glGenVertexArrays(1, &quadVAOi));
		GLCall(glGenBuffers(1, &quadVBOi));
		GLCall(glBindVertexArray(quadVAOi));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, quadVBOi));
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	}

	void draw()
	{
		GLCall(glBindVertexArray(quadVAOi));
		GLCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
		GLCall(glBindVertexArray(0));
	}

};