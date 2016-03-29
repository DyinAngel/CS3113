#pragma once
#define LEVEL_HEIGHT 25
#define LEVEL_WIDTH 75
#define TILE_SIZE 0.3f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include <vector>
#include "SpriteSheet.h"
#include <time.h>

class gameApp {
public:
	gameApp();
	~gameApp();

	void Setup();

	void ProcessInput();
	void Render();
	void Update(float elapsed);
	void UpdateAndRender();
	bool isDone();

private:
	ShaderProgram *program;
	SDL_Event event;
	SDL_Window *displayWindow;

	GLuint tileTexture;
	// GLuint textTexture;
	float lastElapsedTime;
	float timeLeftOver;

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];
	std::vector<Entity*> entities;
	Entity *player;

	int xBase;

	bool done;

	/* uneeded functions moved down here
	void DrawText(int fontTexture, std::string text, float size, float spacing);
	*/
};