#pragma once

#include "CommonValues.h"

#include <GL/glew.h>

class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);
	~Texture();

	bool LoadTexture();
	// A for alpha channel
	bool LoadTextureA();

	void UseTexture();
	void ClearTexture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};

