#pragma once

// GLEW: used to load all opengl functions
#include <GL/glew.h>

// GLFW: is a library that provides some classes/functions to e.g. create 
// a window, create an opegl context, support joysticks/keyboard via callbacks, etc..
#include <GLFW/glfw3.h>

#include "./../ErrorHandling.h"
#include "./../Window.h"
#include "./../Camera.h"
#include "../Model.h"
#include "../SunLight.h"
#include "../ShadowMap2D.h"
#include "../ShadowCubeMap.h"

#include <vector>

/* maths */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>


int chapter_practice();