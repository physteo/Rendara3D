#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

unsigned char* load_image(const char* filepath, int* width, int* height, int* nrChannels, bool reversed)
{
	stbi_set_flip_vertically_on_load(reversed);
	return stbi_load(filepath, width, height, nrChannels, 0);
}


void free_image(unsigned char* data)
{
	stbi_image_free(data);
}
