#pragma once
// GLEW: used to load all opengl functions
#include <GL/glew.h>

// GLFW: is a library that provides some classes/functions to e.g. create 
// a window, create an opegl context, support joysticks/keyboard via callbacks, etc..
#include <GLFW/glfw3.h>

#include "./../ErrorHandling.h"
#include "./../Window.h"
#include "./../Shader.h"
#include "./../buffers/VertexArray.h"
#include "./../Camera.h"
#include "./../Texture.h"

#include <vector>

int chapter1();