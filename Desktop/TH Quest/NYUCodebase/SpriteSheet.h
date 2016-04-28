#pragma once
#include "ShaderProgram.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class SpriteSheet
{
public:
	SpriteSheet();
	SpriteSheet(unsigned int textureID, float u, float v, float width, float height, float size);
	void Draw(ShaderProgram* program);

	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};