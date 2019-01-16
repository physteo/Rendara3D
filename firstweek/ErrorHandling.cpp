#include "ErrorHandling.h"

bool GLlogCall()//const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL error] (" << error << "):" <<
			std::endl;
			//function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void GLclearErrors()
{
	while (glGetError() != GL_NO_ERROR);
}