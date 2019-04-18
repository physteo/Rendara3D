#pragma once

#include "../../Window/Window.h"
#include "../../Model/Model.h"
#include "../../Camera/Camera.h"
#include "../../lighting/SunLight.h"
#include "../../lighting/ShadowMap2D.h"

#include "../../lighting/PointLight.h"
#include "../../lighting/ShadowCubeMap.h"

#include "../../buffers/FrameBuffer.h"

#include "../GameObject.h"
#include "OutBreakLevel.h"

#include "../../Renderer/Renderer.h"

#include<vector>
#include<list>

#include "./../GameState.h"


class OutBreak
{
public:
	
	OutBreak(GLuint widthIn, GLuint heightIn);

	// Executes the outbreak game.
	void execute();

private:
	Window window;
	
	std::vector<Texture> loadedTextures;
	
};