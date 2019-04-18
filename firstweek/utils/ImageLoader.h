#pragma once

unsigned char* load_image(const char* filepath, int* width, int* height, int* nrChannels, bool reversed);

void free_image(unsigned char* data);

