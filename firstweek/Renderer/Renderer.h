#pragma once

#include "../Renderer/Transform.h"
#include "../Model.h"



class Renderer
{
public:
	virtual void submit(const Model* renderable, // what to draw
		Transform* transform) = 0;   // where to draw
	virtual void draw() = 0;
	virtual void draw(Shader& shader) = 0;

	virtual void clear() = 0;
};




