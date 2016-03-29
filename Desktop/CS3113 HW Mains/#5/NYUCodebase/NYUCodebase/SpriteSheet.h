#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"

// for sheets with balanced grid layout
class SpriteSheet 
{
public:
	SpriteSheet();
	SpriteSheet(GLuint loadedSheet, int numOfXSprites, int numOfYSprites);

	void Draw(ShaderProgram *program);
	int index;
	GLuint loadedSheet;
	int numOfXSprites, numOfYSprites;
};

//for sheets with unsorted grids
class unsortedSpriteSheet
{
public:
	unsortedSpriteSheet();
	unsortedSpriteSheet(unsigned int textureID, float u, float v, float width, float height, float size);
	void Draw(ShaderProgram* program, Matrix& gameMatrix, const float shiftX, const float shiftY);

	unsigned int textureID;
	float u, v, width, height, size;
};