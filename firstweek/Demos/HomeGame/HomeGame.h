#pragma once

#include "Level.h"

#include "../../Window.h"
#include "../../Model.h"
#include "../../Camera.h"
#include "../../lighting/SunLight.h"
#include "../../lighting/OrthoFrustrum.h"
#include "../../lighting/ShadowMap2D.h"
			 
#include "../../lighting/PointLight.h"
#include "../../lighting/ShadowCubeMap.h"
#include "../../lighting/PerspectiveFrustrum.h"

#include "../GameObject.h"
#include "./FirstLevel.h"

#include "./../../Renderer/InstanceSet.h"


#include<vector>

namespace homelife {



	enum HomeGameState
	{
		HOME_GAME_MENU,
		HOME_GAME_PLAYING,
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

}