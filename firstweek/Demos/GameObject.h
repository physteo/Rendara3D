#pragma once
#include "../Renderer/Transform.h"
#include "../Renderer/Renderer.h"
#include "../Model/Model.h"



class GameObject
{
public:
	GameObject() : transform(), model(nullptr) {}

	unsigned int ID;

	Transform transform;
	Model* model;
	// physics to add

};


