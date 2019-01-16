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

#include "GameComponent.h"
#include "GameLevel.h"

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
	GLboolean keys[1024];
	GLuint width, height;

	Game(GLuint widthIn, GLuint heightIn);
	//~Game();

	// initialization
	void init();
	
	// game loop
	//void render();
	//void update(float dt);
	//void processInput(float dt);

	// execute
	int execute();

private:
	// first: window ( because graphics must be initialized first)
	Window window;

	// textures
	std::vector<Texture> loadedTextures;

	// models
	Model cube;

};