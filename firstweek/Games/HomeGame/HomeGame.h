#pragma once

#include "../../Window.h"
#include "../../Model.h"
#include "../../Camera.h"
#include "../../SunLight.h"
#include "../../OrthoFrustrum.h"
#include "../../ShadowMap2D.h"
			 
#include "../../PointLight.h"
#include "../../ShadowCubeMap.h"
#include "../../PerspectiveFrustrum.h"

#include "../GameObject.h"
#include "./Level.h"

#include "./../InstanceSet.h"

#include<vector>



enum HomeGameState
{
	HOME_GAME_MENU,
	HOME_GAME_AT_HOME,
	HOME_GAME_OUTSIDE
};

class HomeGame
{
public:
	
	// Game state
	HomeGameState state;
	GLuint width, height;

	HomeGame(GLuint widthIn, GLuint heightIn);

	// execute
	int execute();

private:
	// first: window ( because graphics must be initialized first)
	Window window;

	// textures
	std::vector<Texture> loadedTextures;

};