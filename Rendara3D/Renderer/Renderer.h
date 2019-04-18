#pragma once

#include "../Renderer/Transform.h"
#include "../Model/Model.h"

// Specifies the information needed for drawing a 3D model on the screen 
struct RenderingSpecification
{
	const Model* model;     // what to draw
	Transform    transform;	// where to draw
	Shader*      shader;	// how to draw
};


class Renderer
{
public:

	virtual void submit(RenderingSpecification renderingSpecification) = 0;     

	// draw using the already provided shaders (when "submit" was used)
	virtual void draw() = 0;
	// draw using the specified shader (not the one provided with "submit")
	virtual void draw(Shader* shader) = 0;
	// clears the internal storage of objects to be drawn 
	virtual void clear() = 0;
};




