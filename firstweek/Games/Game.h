#pragma once

#include "../Window.h"
#include "../Model.h"
#include "../Camera.h"
#include "../SunLight.h"
#include "../OrthoFrustrum.h"
#include "../ShadowMap2D.h"

#include "../PointLight.h"
#include "../ShadowCubeMap.h"
#include "../PerspectiveFrustrum.h"

#include "GameObject.h"
#include "GameLevel.h"

#include "./InstanceSet.h"

#include<vector>



enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

class Game
{
public:
	// Game state
	GameState state;
	GLuint width, height;

	Game(GLuint widthIn, GLuint heightIn);

	// execute
	int execute();

private:
	// first: window ( because graphics must be initialized first)
	Window window;

	// textures
	std::vector<Texture> loadedTextures;

};