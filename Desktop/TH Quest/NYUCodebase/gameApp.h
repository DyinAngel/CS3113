#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include "SpriteSheet.h"


enum STATE { MAIN_MENU, MAIN_QUEST, MAIN_DEATH };
enum QUEST {RUNNING_IDLE, MONSTER_ENCOUNTER, BLACKSMITH_ENCOUNTER, SHRINE_ENCOUNTER, ITEMSHOP_ENCOUNTER, BOSS_ENCOUNTER};
class gameApp
{
public:
	gameApp();
	~gameApp();
	void Setup();
	void ProcessInput(float elapsed);

	void Render();
	void Update();
	void UpdateAndRender();
	void RenderMainMenu();
	void RenderQuest();
	void RenderGameOver();


	void GenerateEnemy();
	void GenerateEncounter();
	void doBattle();

	void DrawText(int fontTexture, std::string text, float size, float spacing);
	GLuint LoadTexture(const char *image_path); 
	GLuint LoadTexture24Bit(const char *image_path);

	Matrix modelMatrix;
	Matrix projectionMatrix;
	Matrix viewMatrix;
	SDL_Event event;
	SDL_Window* displayWindow;
	ShaderProgram *program;

	STATE state;
	QUEST stage;

	int heroCurrentHp;
	int heroMaxHp;
	int heroAtk;
	int heroDef;

	bool done;
	bool isDone();


};