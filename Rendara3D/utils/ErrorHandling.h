#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x)\
{GLclearErrors();\
x;\
ASSERT(GLlogCall());}


bool GLlogCall();// const char* function, const char* file, int line);
void GLclearErrors();
