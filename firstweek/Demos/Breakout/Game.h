#pragma once

#include "../../Window.h"
#include "../../Model.h"
#include "../../Camera.h"
#include "../../lighting/SunLight.h"
#include "../../lighting/OrthoFrustrum.h"
#include "../../lighting/ShadowMap2D.h"

#include "../../lighting/PointLight.h"
#include "../../lighting/ShadowCubeMap.h"
#include "../../lighting/PerspectiveFrustrum.h"

#include "../../buffers/FrameBuffer.h"

#include "../GameObject.h"
#include "GameLevel.h"

#include "../../Renderer/Renderer.h"

#include<vector>
#include<list>


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