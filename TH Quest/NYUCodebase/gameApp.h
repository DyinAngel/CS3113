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
enum QUEST {IDLE, RUNNING, MONSTER_ENCOUNTER, BLACKSMITH_ENCOUNTER, SHRINE_ENCOUNTER, ITEMSHOP_ENCOUNTER, BOSS_ENCOUNTER};
enum MONSTER_TYPE {GOBLIN, SKELETON, DRAGON, WRAITH, DEMON, PLANT, BOSS};
class gameApp
{
public:
	gameApp();
	~gameApp();
	void Setup();
	void ProcessInput();

	void Render();
	void Update();
	void UpdateAndRender();
	void RenderMainMenu();
	void RenderQuest();
	void RenderGameOver();

	void DrawText(int fontTexture, std::string text, float size, float spacing);
	GLuint LoadTexture(const char *image_path); 
	GLuint LoadTexture24Bit(const char *image_path);

	Matrix modelMatrix;
	Matrix projectionMatrix;
	Matrix viewMatrix;
	SDL_Event event;
	SDL_Window* displayWindow;
	ShaderProgram *program;

	GLint daynight;
	float daynighttime;

	STATE state;
	QUEST stage;
	MONSTER_TYPE monstertype;

	float bgU= 0.026f;
	float shrineU = .022f;
	float smithU = .022f;
	float itemU = .022f;
	void reset();

	bool done;
	bool isDone();
	
	//game stuff
	void initGameStats();
	void scrollBackground();
	void scrollShrine();
	void scrollSmithy();
	void scrollItemShop();

	Entity bgPict;
	Entity hero;
	Entity enemy;
	Entity shrine;
	Entity itemshop;

	int heroCurrentHp = 1000;
	int heroMaxHp = 1000;
	int heroAtk = 150;
	int heroDef = 50;

	int monsterCurrentHp;
	int monsterMaxHp;
	int monsterAtk;
	bool monsterExists = false;

	int gold = 250;
	int numberOfEncounters = 0;
	int numOfShrinesUsed = 0;

	// for skill
	bool crithit = false;
	bool soulguard = false;
	bool heavanSplit = false;
	bool bladeCall = false;
	bool lightningslash = false;
	std::string curSkillUsed;

	//for cycling through attack types
	int attackTypeCounter = 0; 
	//for drifting
	float drift = 0.03;
	//for item effects
	bool usedSecondChance = false;
	bool usedDivineEdge = false;

	//for attack combos
	bool isPlayerTurn = true;
	int numberofHitsCounter = 1;
	bool playerAnimationFinished = false;
	bool monsterAnimationFinished = false;

	//items - start with a potion
	bool hasItem = true;
	std::string item = "Potion";

	bool hasNotActed = true;

	std::vector<std::string> upcomingEvents;
	std::string knownSkills = " ";
	void dealwithEvents();
	
	bool hasNotPlayedStopAnimation = true;
};