#pragma once

// GLEW: used to load all opengl functions
#include <GL/glew.h>

// GLFW: is a library that provides some classes/functions to e.g. create 
// a window, create an opegl context, support joysticks/keyboard via callbacks, etc..
#include <GLFW/glfw3.h>

#include "../../utils/ErrorHandling.h"
#include "../../Window/Window.h"
#include "../../Camera/Camera.h"
//#include "../../Camera/arcball.h"
#include "../../Model/Model.h"
#include "../../lighting/SunLight.h"
#include "../../lighting/ShadowMap2D.h"
#include "../../lighting/ShadowCubeMap.h"
#include "../../buffers/FrameBuffer.h"
#include "../../Renderer/Simple3DRenderer.h"
#include "../../Renderer/InstanceSet.h"
#include "../ParticleSystem.h"

#include <vector>
#include <array>

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#define FLATRAND float(rand())/(RAND_MAX + 1)

int demo_instancing();
