#pragma once
#include "../Renderer/Transform.h"
#include "../Renderer/Renderer.h"
#include "../Model.h"



class GameObject
{
public:
	GameObject() : transform(), model(nullptr), renderer(nullptr) {}

	unsigned int ID;

	Transform transform;
	Model* model;
	// physics to add

	Renderer* renderer;
};


