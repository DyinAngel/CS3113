#include "gameApp.h"
#include <ctime>
#include <SDL_mixer.h>
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

//constants
float MAX_LEFT = -3.1f;
float MAX_TOP = .8f;
float MAX_RIGHT = 2.0f;
float MAX_BOT = -2.4f;
//music --------------------------------------------------------------------------------------------------------------------------------------------------------------
Mix_Chunk *swordhit;
Mix_Chunk *monsterhit;
Mix_Chunk *interact;
Mix_Music *titlemewsic;
Mix_Music *stagemewsic;
Mix_Music *gameovermewsic;
// current sprites--------------------------------------------------------------------------------------------------------------------------------------------------------------
GLuint fontSprites; 
GLuint shrineImg; 
GLuint blacksmithImg;
GLuint itemshopImg;
GLuint damageEffectH;
GLuint damageEffectM;
// hero animation frame sprites--------------------------------------------------------------------------------------------------------------------------------------------------------------
SpriteSheet attack1[11];
SpriteSheet attack2[13];
SpriteSheet attack3[10];
SpriteSheet attack4[10];
SpriteSheet block[2];
SpriteSheet crit[13];
SpriteSheet criteffect[13];
SpriteSheet death[2];
SpriteSheet encounter[7];
SpriteSheet flashstepEff[11];
SpriteSheet flashstep[13];
SpriteSheet idle[8];
SpriteSheet dash[7];
SpriteSheet useitem[11];
SpriteSheet takedamage[2];
// monster sprites --------------------------------------------------------------------------------------------------------------------------------------------------------------
SpriteSheet balrog[11];
SpriteSheet balrogdamage;
SpriteSheet goblin[10];
SpriteSheet goblindamage;
SpriteSheet plant[7];
SpriteSheet plantdamage;
SpriteSheet skeleton[5];
SpriteSheet skeletondamage;
SpriteSheet wraith[8];
SpriteSheet wraithdamage;
SpriteSheet dragon[23];
SpriteSheet dragondamage;
//skills --------------------------------------------------------------------------------------------------------------------------------------------------------------
SpriteSheet skillSplit[11];  //heavan's split
SpriteSheet split;
SpriteSheet eoGeom[21]; //Blade Works  //might need to be split
SpriteSheet callsword;
// background pictures --------------------------------------------------------------------------------------------------------------------------------------------------------------
GLuint titlescreen;  
GLuint stagescreen;
//items --------------------------------------------------------------------------------------------------------------------------------------------------------------
GLuint potion;
GLuint divineEdge;
GLuint secondChance;
GLuint skillScroll;
//Entities
Entity bgPict;
Entity shrine;
Entity blacksmith;
Entity activeItem;
Entity player;
Entity monster;
Entity hiteffects;
//Names
std::string armorNames[11] = { "Unarmored", "Leather Suit", "Platemail", "Knight Cuirass", "Shiva's Guard", "Gatekeeper", "Dragon Mail", "Angel's Veil", "Ebony Mail", "Half-Ghost" };
std::string weaponNames[11] = { "Dagger", "Longsword", "Wakizashi", "Uchiganta", "Yasha", "Black Blade", "Bloodlust", "Muramasa", "Genji Blade", "Divine Rapier" };
int weaponIndex = 0;
int armorIndex = 0;

//animation

int playerIndex= 0;
int playerNumFrames;
float counter = 0;
float runningCounter = 0.0f;
int monsterIndex = 0;
int monsterNumFrames;
float elapsed = 0.0f;
float lastFrameTicks =0.0f;
float xOffset = 0.0f;
float yOffset = 0.0f;
bool xIncreasing = true;
bool yIncreasing = true;

// cheating
int goldcounter = 0;
gameApp::gameApp()
{
	Setup();
	state = MAIN_MENU;
	stage = IDLE;
	done = false;
}

gameApp::~gameApp()
{
	delete program;
	Mix_FreeChunk(swordhit);
	Mix_FreeChunk(monsterhit);
	Mix_FreeChunk(interact);
	Mix_FreeMusic(titlemewsic);
	Mix_FreeMusic(stagemewsic);
	Mix_FreeMusic(gameovermewsic);
	SDL_Quit();
}

void gameApp::Setup()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Touhou Quest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 880, 640, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glUseProgram(program->programID);
	glViewport(0, 0,880,640);
	projectionMatrix.setOrthoProjection(-4.0f, 4.0f, -4.0f, 4.0f, -1.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);
	//loadmusic------------------------------------------------------------------------------------------------------
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	swordhit = Mix_LoadWAV("music/swordslash.wav");
	monsterhit = Mix_LoadWAV("music/gettinghit.wav");
	interact = Mix_LoadWAV("music/upgrade.wav");
	titlemewsic = Mix_LoadMUS("music/title.ogg");
	stagemewsic = Mix_LoadMUS("music/quest.ogg");
	gameovermewsic = Mix_LoadMUS("music/Wingless.ogg");
	//load single images--------------------------------------------------------------------------------------------------------------------------------------------------------------
	fontSprites = LoadTexture("images/font1.png");
	shrineImg = LoadTexture("images/shrine.png");
	blacksmithImg = LoadTexture("images/blacksmith.png");
	itemshopImg = LoadTexture("images/house.png");

	stagescreen = LoadTexture("images/bg.png");
	titlescreen = LoadTexture24Bit("images/title.png");

	potion = LoadTexture("images/potion.png");
	divineEdge = LoadTexture("images/divineblade.png");
	skillScroll = LoadTexture("images/skillscroll.png");
	secondChance = LoadTexture("images/secondchance.png");
	//load hero sprite sheet animations--------------------------------------------------------------------------------------------------------------------------------------------------------------
	GLuint youmu = LoadTexture("images/youmu.png");

	attack1[0] = SpriteSheet(youmu, 1425.0f / 1642.0f, 75.0f / 879.0f, 77.0f / 1642.0f, 94.0f / 879.0f, 1.852);
	attack1[1] = SpriteSheet(youmu, 1149.0f / 1642.0f, 679.0f / 879.0f, 66.0f / 1642.0f, 122.0f / 879.0f, 1.852);
	attack1[2] = SpriteSheet(youmu, 1254.0f / 1642.0f, 0.0f / 879.0f, 85.0f / 1642.0f, 107.0f / 879.0f, 1.852);
	attack1[3] = SpriteSheet(youmu, 512.0f / 1642.0f, 669.0f / 879.0f, 96.0f / 1642.0f, 91.0f / 879.0f, 1.852);
	attack1[4] = SpriteSheet(youmu, 1170.0f / 1642.0f, 599.0f / 879.0f, 68.0f / 1642.0f, 79.0f / 879.0f, 1.852);
	attack1[5] = SpriteSheet(youmu, 1174.0f / 1642.0f, 417.0f / 879.0f, 73.0f / 1642.0f, 72.0f / 879.0f, 1.852);
	attack1[6] = SpriteSheet(youmu, 1097.0f / 1642.0f, 528.0f / 879.0f, 72.0f / 1642.0f, 97.0f / 879.0f, 1.852);
	attack1[7] = SpriteSheet(youmu, 1477.0f / 1642.0f, 170.0f / 879.0f, 71.0f / 1642.0f, 100.0f / 879.0f, 1.852);
	attack1[8] = SpriteSheet(youmu, 1507.0f / 1642.0f, 0.0f / 879.0f, 77.0f / 1642.0f, 92.0f / 879.0f, 1.852);
	attack1[9] = SpriteSheet(youmu, 1503.0f / 1642.0f, 93.0f / 879.0f, 76.0f / 1642.0f, 75.0f / 879.0f, 1.852);
	attack1[10] = SpriteSheet(youmu, 1124.0f / 1642.0f, 802.0f / 879.0f, 66.0f / 1642.0f, 77.0f / 879.0f, 1.852);

	attack2[0] = SpriteSheet(youmu, 1457.0f / 1642.0f, 372.0f / 879.0f, 66.0f / 1642.0f, 123.0f / 879.0f, 1.852);
	attack2[1] = SpriteSheet(youmu, 1322.0f / 1642.0f, 176.0f / 879.0f, 77.0f / 1642.0f, 105.0f / 879.0f, 1.852);
	attack2[2] = SpriteSheet(youmu, 0.0f / 1642.0f, 652.0f / 879.0f, 105.0f / 1642.0f, 79.0f / 879.0f, 1.852);
	attack2[3] = SpriteSheet(youmu, 213.0f / 1642.0f, 587.0f / 879.0f, 102.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	attack2[4] = SpriteSheet(youmu, 0.0f / 1642.0f, 358.0f / 879.0f, 135.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	attack2[5] = SpriteSheet(youmu, 226.0f / 1642.0f, 113.0f / 879.0f, 161.0f / 1642.0f, 68.0f / 879.0f, 1.852);
	attack2[6] = SpriteSheet(youmu, 359.0f / 1642.0f, 182.0f / 879.0f, 149.0f / 1642.0f, 70.0f / 879.0f, 1.852);
	attack2[7] = SpriteSheet(youmu, 0.0f / 1642.0f, 213.0f / 879.0f, 144.0f / 1642.0f, 72.0f / 879.0f, 1.852);
	attack2[8] = SpriteSheet(youmu, 136.0f / 1642.0f, 437.0f / 879.0f, 123.0f / 1642.0f, 74.0f / 879.0f, 1.852);
	attack2[9] = SpriteSheet(youmu, 739.0f / 1642.0f, 392.0f / 879.0f, 107.0f / 1642.0f, 70.0f / 879.0f, 1.852);
	attack2[10] = SpriteSheet(youmu, 0.0f / 1642.0f, 580.0f / 879.0f, 106.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	attack2[11] = SpriteSheet(youmu, 1400.0f / 1642.0f, 253.0f / 879.0f, 71.0f / 1642.0f, 73.0f / 879.0f, 1.852);
	attack2[12] = SpriteSheet(youmu, 982.0f / 1642.0f, 793.0f / 879.0f, 68.0f / 1642.0f, 77.0f / 879.0f, 1.852);

	attack3[0] = SpriteSheet(youmu, 989.0f / 1642.0f, 0.0f / 879.0f, 88.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	attack3[1] = SpriteSheet(youmu, 762.0f / 1642.0f, 80.0f / 879.0f, 112.0f / 1642.0f, 85.0f / 879.0f, 1.852);
	attack3[2] = SpriteSheet(youmu, 1017.0f / 1642.0f, 428.0f / 879.0f, 79.0f / 1642.0f, 107.0f / 879.0f, 1.852);
	attack3[3] = SpriteSheet(youmu, 214.0f / 1642.0f, 512.0f / 879.0f, 105.0f / 1642.0f, 74.0f / 879.0f, 1.852);
	attack3[4] = SpriteSheet(youmu, 320.0f / 1642.0f, 509.0f / 879.0f, 102.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	attack3[5] = SpriteSheet(youmu, 423.0f / 1642.0f, 509.0f / 879.0f, 101.0f / 1642.0f, 68.0f / 879.0f, 1.852);
	attack3[6] = SpriteSheet(youmu, 210.0f / 1642.0f, 752.0f / 879.0f, 101.0f / 1642.0f, 68.0f / 879.0f, 1.852);
	attack3[7] = SpriteSheet(youmu, 619.0f / 1642.0f, 437.0f / 879.0f, 109.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	attack3[8] = SpriteSheet(youmu, 1292.0f / 1642.0f, 345.0f / 879.0f, 71.0f / 1642.0f, 73.0f / 879.0f, 1.852);
	attack3[9] = SpriteSheet(youmu, 1319.0f / 1642.0f, 423.0f / 879.0f, 68.0f / 1642.0f, 77.0f / 879.0f, 1.852);

	attack4[0] = SpriteSheet(youmu, 107.0f / 1642.0f, 512.0f / 879.0f,	106.0f / 1642.0f, 67.0f / 879.0f, 1.852);
	attack4[1] = SpriteSheet(youmu, 800.0f / 1642.0f, 716.0f / 879.0f, 106.0f / 1642.0f, 74.0f / 879.0f, 1.852);
	attack4[2] = SpriteSheet(youmu, 1340.0f / 1642.0f, 83.0f / 879.0f, 81.0f / 1642.0f, 91.0f / 879.0f, 1.852);
	attack4[3] = SpriteSheet(youmu, 1316.0f / 1642.0f, 282.0f / 879.0f, 73.0f / 1642.0f, 62.0f / 879.0f, 1.852);
	attack4[4] = SpriteSheet(youmu, 395.0f / 1642.0f, 253.0f / 879.0f, 118.0f / 1642.0f, 108.0f / 879.0f, 1.852);
	attack4[5] = SpriteSheet(youmu, 1067.0f / 1642.0f, 164.0f / 879.0f, 85.0f / 1642.0f, 108.0f / 879.0f, 1.852);
	attack4[6] = SpriteSheet(youmu, 1174.0f / 1642.0f, 490.0f / 879.0f, 70.0f / 1642.0f, 108.0f / 879.0f, 1.852);
	attack4[7] = SpriteSheet(youmu, 1191.0f / 1642.0f, 802.0f / 879.0f, 64.0f / 1642.0f, 67.0f / 879.0f, 1.852);
	attack4[8] = SpriteSheet(youmu, 1388.0f / 1642.0f, 423.0f / 879.0f, 68.0f / 1642.0f, 60.0f / 879.0f, 1.852);
	attack4[9] = SpriteSheet(youmu, 199.0f / 1642.0f, 821.0f / 879.0f, 72.0f / 1642.0f, 54.0f / 879.0f, 1.852);

	block[0] = SpriteSheet(youmu, 609.0f / 1642.0f, 675.0f / 879.0f, 95.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	block[1] = SpriteSheet(youmu, 1455.0f / 1642.0f, 570.0f / 879.0f, 63.0f / 1642.0f, 102.0f / 879.0f, 1.852);

	crit[0] = SpriteSheet(youmu, 1541.0f / 1642.0f, 290.0f / 879.0f, 67.0f / 1642.0f, 80.0f / 879.0f, 1.852);
	crit[1] = SpriteSheet(youmu, 705.0f / 1642.0f, 716.0f / 879.0f, 94.0f / 1642.0f, 79.0f / 879.0f, 1.852);
	crit[2] = SpriteSheet(youmu, 1239.0f / 1642.0f, 599.0f / 879.0f, 67.0f / 1642.0f, 98.0f / 879.0f, 1.852);
	crit[3] = SpriteSheet(youmu, 1472.0f / 1642.0f, 271.0f / 879.0f, 68.0f / 1642.0f, 100.0f / 879.0f, 1.852);
	crit[4] = SpriteSheet(youmu, 1006.0f / 1642.0f, 633.0f / 879.0f, 72.0f / 1642.0f, 74.0f / 879.0f, 1.852);
	crit[5] = SpriteSheet(youmu, 1248.0f / 1642.0f, 419.0f / 879.0f, 70.0f / 1642.0f, 72.0f / 879.0f, 1.852);
	crit[6] = SpriteSheet(youmu, 609.0f / 1642.0f, 757.0f / 879.0f, 94.0f / 1642.0f, 96.0f / 879.0f, 1.852);
	crit[7] = SpriteSheet(youmu, 723.0f / 1642.0f, 547.0f / 879.0f, 95.0f / 1642.0f, 91.0f / 879.0f, 1.852);
	crit[8] = SpriteSheet(youmu, 877.0f / 1642.0f, 0.0f / 879.0f, 111.0f / 1642.0f, 91.0f / 879.0f, 1.852);
	crit[9] = SpriteSheet(youmu, 1425.0f / 1642.0f, 0.0f / 879.0f, 81.0f / 1642.0f, 74.0f / 879.0f, 1.852);
	crit[10] = SpriteSheet(youmu, 1582.0f / 1642.0f, 582.0f / 879.0f, 59.0f / 1642.0f, 76.0f / 879.0f, 1.852);
	crit[11] = SpriteSheet(youmu, 1400.0f / 1642.0f, 175.0f / 879.0f, 76.0f / 1642.0f, 77.0f / 879.0f, 1.852);
	crit[12] = SpriteSheet(youmu, 963.0f / 1642.0f, 247.0f / 879.0f, 86.0f / 1642.0f, 80.0f / 879.0f, 1.852);

	criteffect[0] = SpriteSheet(youmu, 312.0f / 1642.0f, 751.0f / 879.0f, 99.0f / 1642.0f, 80.0f / 879.0f, 1.852);
	criteffect[1] = SpriteSheet(youmu, 850.0f / 1642.0f, 376.0f / 879.0f, 106.0f / 1642.0f, 87.0f / 879.0f, 1.852);
	criteffect[2] = SpriteSheet(youmu, 628.0f / 1642.0f, 347.0f / 879.0f, 110.0f / 1642.0f, 89.0f / 879.0f, 1.852);
	criteffect[3] = SpriteSheet(youmu, 514.0f / 1642.0f, 261.0f / 879.0f, 113.0f / 1642.0f, 94.0f / 879.0f, 1.852);
	criteffect[4] = SpriteSheet(youmu, 392.0f / 1642.0f, 362.0f / 879.0f, 114.0f / 1642.0f, 98.0f / 879.0f, 1.852);
	criteffect[5] = SpriteSheet(youmu, 641.0f / 1642.0f, 101.0f / 879.0f, 115.0f / 1642.0f, 99.0f / 879.0f, 1.852);
	criteffect[6] = SpriteSheet(youmu, 642.0f / 1642.0f, 0.0f / 879.0f, 119.0f / 1642.0f, 100.0f / 879.0f, 1.852);
	criteffect[7] = SpriteSheet(youmu, 269.0f / 1642.0f, 344.0f / 879.0f, 122.0f / 1642.0f, 97.0f / 879.0f, 1.852);
	criteffect[8] = SpriteSheet(youmu, 396.0f / 1642.0f, 49.0f / 879.0f, 122.0f / 1642.0f, 93.0f / 879.0f, 1.852);
	criteffect[9] = SpriteSheet(youmu, 270.0f / 1642.0f, 253.0f / 879.0f, 124.0f / 1642.0f, 90.0f / 879.0f, 1.852);
	criteffect[10] = SpriteSheet(youmu, 519.0f / 1642.0f, 0.0f / 879.0f, 122.0f / 1642.0f, 88.0f / 879.0f, 1.852);
	criteffect[11] = SpriteSheet(youmu, 519.0f / 1642.0f, 89.0f / 879.0f, 121.0f / 1642.0f, 87.0f / 879.0f, 1.852);
	criteffect[12] = SpriteSheet(youmu, 514.0f / 1642.0f, 177.0f / 879.0f, 117.0f / 1642.0f, 83.0f / 879.0f, 1.852);

	death[0] = SpriteSheet(youmu, 0.0f / 1642.0f, 814.0f / 879.0f, 102.0f / 1642.0f, 52.0f / 879.0f, 1.852);
	death[1] = SpriteSheet(youmu, 369.0f / 1642.0f, 461.0f / 879.0f, 107.0f / 1642.0f, 47.0f / 879.0f, 1.852);

	encounter[0] = SpriteSheet(youmu, 757.0f / 1642.0f, 166.0f / 879.0f, 111.0f / 1642.0f, 72.0f / 879.0f, 1.852);
	encounter[1] = SpriteSheet(youmu, 412.0f / 1642.0f, 744.0f / 879.0f, 97.0f / 1642.0f, 76.0f / 879.0f, 1.852);
	encounter[2] = SpriteSheet(youmu, 1078.0f / 1642.0f, 82.0f / 879.0f, 87.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	encounter[3] = SpriteSheet(youmu, 855.0f / 1642.0f, 313.0f / 879.0f, 107.0f / 1642.0f, 62.0f / 879.0f, 1.852);
	encounter[4] = SpriteSheet(youmu, 260.0f / 1642.0f, 442.0f / 879.0f, 108.0f / 1642.0f, 61.0f / 879.0f, 1.852);
	encounter[5] = SpriteSheet(youmu, 855.0f / 1642.0f, 241.0f / 879.0f, 107.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	encounter[6] = SpriteSheet(youmu, 523.0f / 1642.0f, 591.0f / 879.0f, 98.0f / 1642.0f, 77.0f / 879.0f, 1.852);
	
	flashstepEff[0] = SpriteSheet(youmu, 103.0f / 1642.0f, 815.0f / 879.0f, 95.0f / 1642.0f, 60.0f / 879.0f, 1.852);
	flashstepEff[1] = SpriteSheet(youmu, 203.0f / 1642.0f, 182.0f / 879.0f, 155.0f / 1642.0f, 63.0f / 879.0f, 1.852);
	flashstepEff[2] = SpriteSheet(youmu, 0.0f / 1642.0f, 50.0f / 879.0f, 231.0 / 1642.0f, 61.0f / 879.0f, 1.852);
	flashstepEff[3] = SpriteSheet(youmu, 0.0f / 1642.0f, 0.0f / 879.0f, 249.0f / 1642.0f, 49.0f / 879.0f, 1.852);
	flashstepEff[4] = SpriteSheet(youmu, 0.0f / 1642.0f, 112.0f / 879.0f, 225.0f / 1642.0f, 45.0f / 879.0f, 1.852);
	flashstepEff[5] = SpriteSheet(youmu, 250.0f / 1642.0f, 0.0f / 879.0f, 197.0f / 1642.0f, 48.0f / 879.0f, 1.852);
	flashstepEff[6] = SpriteSheet(youmu, 0.0f / 1642.0f, 158.0f / 879.0f, 202.0f / 1642.0f, 54.0f / 879.0f, 1.852);
	flashstepEff[7] = SpriteSheet(youmu, 232.0f / 1642.0f, 50.0f / 879.0f, 163.0f / 1642.0f, 62.0f / 879.0f, 1.852);
	flashstepEff[8] = SpriteSheet(youmu, 0.0f / 1642.0f, 286.0f / 879.0f, 141.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	flashstepEff[9] = SpriteSheet(youmu, 719.0f / 1642.0f, 639.0f / 879.0f, 95.0f / 1642.0f, 76.0f / 879.0f, 1.852);
	
	flashstep[0] = SpriteSheet(youmu, 762.0f / 1642.0f, 0.0f / 879.0f, 114.0f / 1642.0f, 79.0f / 879.0f, 1.852);
	flashstep[1] = SpriteSheet(youmu, 704.0f / 1642.0f, 796.0f / 879.0f, 93.0f / 1642.0f, 75.0f / 879.0f, 1.852);
	flashstep[2] = SpriteSheet(youmu, 1153.0f / 1642.0f, 176.0f / 879.0f, 84.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	flashstep[3] = SpriteSheet(youmu, 1253.0f / 1642.0f, 108.0f / 879.0f, 84.0f / 1642.0f, 67.0f / 879.0f, 1.852);
	flashstep[4] = SpriteSheet(youmu, 1455.0f / 1642.0f, 496.0f / 879.0f, 65.0f / 1642.0f, 73.0f / 879.0f, 1.852);
	flashstep[5] = SpriteSheet(youmu, 1580.0f / 1642.0f, 93.0f / 879.0f, 59.0f / 1642.0f, 74.0f / 879.0f, 1.852);
	flashstep[6] = SpriteSheet(youmu, 893.0f / 1642.0f, 715.0f / 879.0f, 91.0f / 1642.0f, 63.0f / 879.0f, 1.852);
	flashstep[7] = SpriteSheet(youmu, 142.0f / 1642.0f, 286.0f / 879.0f, 127.0f / 1642.0f, 57.0f / 879.0f, 1.852);
	flashstep[8] = SpriteSheet(youmu, 0.0f / 1642.0f, 441.0f / 879.0f, 117.0f / 1642.0f, 57.0f / 879.0f, 1.852);
	flashstep[9] = SpriteSheet(youmu, 1017.0f / 1642.0f, 536.0f / 879.0f, 76.0f / 1642.0f, 96.0f / 879.0f, 1.852);
	flashstep[10] = SpriteSheet(youmu, 1167.0f / 1642.0f, 0.0f / 879.0f, 86.0f / 1642.0f, 92.0f / 879.0f, 1.852);
	flashstep[11] = SpriteSheet(youmu, 1153.0f / 1642.0f, 248.0f / 879.0f, 83.0f / 1642.0f, 75.0f / 879.0f, 1.852);
	flashstep[12] = SpriteSheet(youmu, 985.0f / 1642.0f, 715.0f / 879.0f, 70.0f / 1642.0f, 77.0f / 879.0f, 1.852);

	idle[0] = SpriteSheet(youmu, 819.0f / 1642.0f, 631.0f / 879.0f, 93.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	idle[1] = SpriteSheet(youmu, 914.0f / 1642.0f, 548.0f / 879.0f, 93.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	idle[2] = SpriteSheet(youmu, 826.0f / 1642.0f, 464.0f / 879.0f, 95.0f / 1642.0f, 80.0f / 879.0f, 1.852);
	idle[3] = SpriteSheet(youmu, 412.0f / 1642.0f, 663.0f / 879.0f, 99.0f / 1642.0f, 80.0f / 879.0f, 1.852);
	idle[4] = SpriteSheet(youmu, 107.0f / 1642.0f, 580.0f / 879.0f, 105.0f / 1642.0f, 79.0f / 879.0f, 1.852);
	idle[5] = SpriteSheet(youmu, 745.0f / 1642.0f, 239.0f / 879.0f, 109.0f / 1642.0f, 78.0f / 879.0f, 1.852);
	idle[6] = SpriteSheet(youmu, 0.0f / 1642.0f, 499.0f / 879.0f, 106.0f / 1642.0f, 80.0f / 879.0f, 1.852);
	idle[7] = SpriteSheet(youmu, 320.0f / 1642.0f, 581.0f / 879.0f, 101.0f / 1642.0f, 81.0f / 879.0f, 1.852);

	dash[0] = SpriteSheet(youmu, 632.0f / 1642.0f, 201.0f / 879.0f, 112.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	dash[1] = SpriteSheet(youmu, 740.0f / 1642.0f,318.0f / 879.0f, 109.0f / 1642.0f, 73.0f / 879.0f, 1.852);
	dash[2] = SpriteSheet(youmu, 507.0f / 1642.0f, 433.0f / 879.0f, 111.0f / 1642.0f, 72.0f / 879.0f, 1.852);
	dash[3] = SpriteSheet(youmu, 875.0f / 1642.0f, 92.0f / 879.0f, 111.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	dash[4] = SpriteSheet(youmu, 507.0f / 1642.0f, 362.0f / 879.0f, 112.0f / 1642.0f, 70.0f / 879.0f, 1.852);
	dash[5] = SpriteSheet(youmu, 869.0f / 1642.0f, 166.0f / 879.0f, 109.0f / 1642.0f, 74.0f / 879.0f, 1.852);
	dash[6] = SpriteSheet(youmu, 628.0f / 1642.0f, 273.0f / 879.0f, 111.0f / 1642.0f, 73.0f / 879.0f, 1.852);

	skillSplit[0] = SpriteSheet(youmu, 893.0f / 1642.0f, 779.0f / 879.0f, 88.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	skillSplit[1] = SpriteSheet(youmu, 1215.0f / 1642.0f, 332.0f / 879.0f, 76.0f / 1642.0f, 84.0f / 879.0f, 1.852);
	skillSplit[2] = SpriteSheet(youmu, 1056.0f / 1642.0f, 753.0f / 879.0f, 67.0f / 1642.0f, 101.0f / 879.0f, 1.852);
	skillSplit[3] = SpriteSheet(youmu, 1254.0f / 1642.0f, 492.0f / 879.0f, 68.0f / 1642.0f, 104.0f / 879.0f, 1.852);
	skillSplit[4] = SpriteSheet(youmu, 1347.0f / 1642.0f, 706.0f / 879.0f, 63.0f / 1642.0f, 101.0f / 879.0f, 1.852);
	skillSplit[5] = SpriteSheet(youmu, 1411.0f / 1642.0f, 697.0f / 879.0f, 62.0f / 1642.0f, 94.0f / 879.0f, 1.852);
	skillSplit[6] = SpriteSheet(youmu, 1382.0f / 1642.0f, 604.0f / 879.0f, 64.0f / 1642.0f, 92.0f / 879.0f, 1.852);
	skillSplit[7] = SpriteSheet(youmu, 1524.0f / 1642.0f, 376.0f / 879.0f, 66.0f / 1642.0f, 90.0f / 879.0f, 1.852);
	skillSplit[8] = SpriteSheet(youmu, 1237.0f / 1642.0f, 248.0f / 879.0f, 78.0f / 1642.0f, 83.0f / 879.0f, 1.852);
	skillSplit[9] = SpriteSheet(youmu, 1166.0f / 1642.0f, 93.0f / 879.0f, 85.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	skillSplit[10] = SpriteSheet(youmu, 979.0f / 1642.0f, 164.0f / 879.0f, 87.0f / 1642.0f, 82.0f / 879.0f, 1.852);

	split = SpriteSheet(youmu, 106.0f / 1642.0f, 660.0f / 879.0f, 103.0f / 1642.0f, 154.0f / 879.0f, 1.852);
	
	eoGeom[0] = SpriteSheet(youmu, 1340.0f / 1642.0f, 0.0f / 879.0f, 84.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	eoGeom[1] = SpriteSheet(youmu, 1097.0f / 1642.0f, 428.0f / 879.0f, 76.0f / 1642.0f, 99.0f / 879.0f, 1.852);
	eoGeom[2] = SpriteSheet(youmu, 1314.0f / 1642.0f, 501.0f / 879.0f, 67.0f / 1642.0f, 121.0f / 879.0f, 1.852);
	eoGeom[3] = SpriteSheet(youmu, 1549.0f / 1642.0f, 169.0f / 879.0f, 68.0f / 1642.0f, 120.0f / 879.0f, 1.852);
	eoGeom[4] = SpriteSheet(youmu, 1283.0f / 1642.0f, 706.0f / 879.0f, 63.0f / 1642.0f, 120.0f / 879.0f, 1.852);
	eoGeom[5] = SpriteSheet(youmu, 1519.0f / 1642.0f, 583.0f / 879.0f, 62.0f / 1642.0f, 119.0f / 879.0f, 1.852);
	eoGeom[6] = SpriteSheet(youmu, 1524.0f / 1642.0f, 463.0f / 879.0f, 64.0f / 1642.0f, 119.0f / 879.0f, 1.852);
	eoGeom[7] = SpriteSheet(youmu, 1388.0f / 1642.0f, 484.0f / 879.0f, 66.0f / 1642.0f, 119.0f / 879.0f, 1.852);
	eoGeom[8] = SpriteSheet(youmu, 1079.0f / 1642.0f, 633.0f / 879.0f, 69.0f / 1642.0f, 119.0f / 879.0f, 1.852);
	eoGeom[9] = SpriteSheet(youmu, 963.0f / 1642.0f, 328.0f / 879.0f, 84.0f / 1642.0f, 87.0f / 879.0f, 1.852);
	eoGeom[10] = SpriteSheet(youmu, 1050.0f / 1642.0f, 273.0f / 879.0f, 84.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	eoGeom[11] = SpriteSheet(youmu, 312.0f / 1642.0f, 669.0f / 879.0f, 99.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	eoGeom[12] = SpriteSheet(youmu, 423.0f / 1642.0f, 578.0f / 879.0f, 99.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	eoGeom[13] = SpriteSheet(youmu, 989.0f / 1642.0f, 82.0f / 879.0f, 88.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	eoGeom[14] = SpriteSheet(youmu, 1216.0f / 1642.0f, 698.0f / 879.0f, 66.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	eoGeom[15] = SpriteSheet(youmu, 1411.0f / 1642.0f, 792.0f / 879.0f, 61.0f / 1642.0f, 80.0f / 879.0f, 1.852);
	eoGeom[16] = SpriteSheet(youmu, 1048.0f / 1642.0f, 356.0f / 879.0f, 83.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	eoGeom[17] = SpriteSheet(youmu, 914.0f / 1642.0f, 630.0f / 879.0f, 91.0f / 1642.0f, 84.0f / 879.0f, 1.852);
	eoGeom[18] = SpriteSheet(youmu, 729.0f / 1642.0f, 463.0f / 879.0f, 96.0f / 1642.0f, 93.0f / 879.0f, 1.852);
	eoGeom[19] = SpriteSheet(youmu, 819.0f / 1642.0f, 547.0f / 879.0f, 94.0f / 1642.0f, 83.0f / 879.0f, 1.852);
	
	callsword = SpriteSheet(youmu, 142.0f / 1642.0f, 334.0f / 879.0f, 126.0f / 1642.0f, 92.0f / 879.0f, 1.852);

	takedamage[0] = SpriteSheet(youmu, 410.0f / 1642.0f, 547.0f / 761.0f, 96.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	takedamage[1] = SpriteSheet(youmu, 1364.0f / 1642.0f, 345.0f / 879.0f, 70.0f / 1642.0f, 77.0f / 879.0f, 1.852);
	takedamage[2] = SpriteSheet(youmu, 1135.0f / 1642.0f, 324.0f / 879.0f, 79.0f / 1642.0f, 76.0f / 879.0f, 1.852);

	useitem[0] = SpriteSheet(youmu, 525.0f / 1642.0f, 509.0f / 879.0f, 99.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	useitem[1] = SpriteSheet(youmu, 1078.0f / 1642.0f, 0.0f / 879.0f, 88.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	useitem[2] = SpriteSheet(youmu, 1307.0f / 1642.0f, 623.0f / 879.0f, 66.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	useitem[3] = SpriteSheet(youmu, 1474.0f / 1642.0f, 703.0f / 879.0f, 61.0f / 1642.0f, 80.0f / 879.0f, 1.852);
	useitem[3] = SpriteSheet(youmu, 1238.0f / 1642.0f, 176.0f / 879.0f, 83.0f / 1642.0f, 71.0f / 879.0f, 1.852);
	useitem[4] = SpriteSheet(youmu, 800.0f / 1642.0f, 791.0f / 879.0f, 91.0f / 1642.0f, 84.0f / 879.0f, 1.852);
	useitem[5] = SpriteSheet(youmu, 622.0f / 1642.0f, 591.0f / 879.0f, 96.0f / 1642.0f, 83.0f / 879.0f, 1.852);
	useitem[6] = SpriteSheet(youmu, 922.0f / 1642.0f, 464.0f / 879.0f, 94.0f / 1642.0f, 83.0f / 879.0f, 1.852);
	useitem[7] = SpriteSheet(youmu, 210.0f / 1642.0f, 669.0f / 879.0f, 101.0f / 1642.0f, 82.0f / 879.0f, 1.852);
	useitem[8] = SpriteSheet(youmu, 0.0f / 1642.0f, 732.0f / 879.0f, 104.0f / 1642.0f, 81.0f / 879.0f, 1.852);
	useitem[9] = SpriteSheet(youmu, 625.0f / 1642.0f, 509.0f / 879.0f, 97.0f / 1642.0f, 80.0f / 879.0f, 1.852);

	//load monster sprite sheet animations ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	GLuint balro = LoadTexture("images/monsters/balrog.png");
	balrog[0] = SpriteSheet(balro, 193.0f / 748.0f, 546.0f / 702.0f, 160.0f / 748.0f, 156.0f / 702.0f, 2.5f);
	balrog[1] = SpriteSheet(balro, 394.0f / 748.0f, 374.0f / 702.0f, 160.0f / 748.0f, 156.0f / 702.0f, 2.5f);
	balrog[2] = SpriteSheet(balro, 0.0f / 748.0f, 180.0f / 702.0f, 246.0f / 748.0f, 193.0f / 702.0f, 2.5f);
	balrog[3] = SpriteSheet(balro, 504.0f / 748.0f, 0.0f / 702.0f, 243.0f / 748.0f, 193.0f / 702.0f, 2.5f);
	balrog[4] = SpriteSheet(balro, 0.0f / 748.0f, 0.0f / 702.0f, 503.0f / 748.0f, 179.0f / 702.0f, 2.5f);
	balrog[5] = SpriteSheet(balro, 247.0f / 748.0f, 180.0f / 702.0f, 246.0f / 748.0f, 193.0f / 702.0f, 2.5f);
	balrog[6] = SpriteSheet(balro, 354.0f / 748.0f, 546.0f / 702.0f, 160.0f / 748.0f, 156.0f / 702.0f, 2.5f);
	balrog[7] = SpriteSheet(balro, 0.0f / 748.0f, 374.0f / 702.0f, 201.0f / 748.0f, 159.0f / 702.0f, 2.5f);
	balrog[8] = SpriteSheet(balro, 0.0f / 748.0f, 534.0f / 702.0f, 192.0f / 748.0f, 161.0f / 702.0f, 2.5f);
	balrog[9] = SpriteSheet(balro, 202.0f / 748.0f, 374.0f / 702.0f, 191.0f / 748.0f, 171.0f / 702.0f, 2.5f);
	balrog[10] = SpriteSheet(balro, 494.0f / 748.0f, 194.0f / 702.0f, 202.0f / 748.0f, 159.0f / 702.0f, 2.5f);
	balrogdamage = SpriteSheet(balro, 555.0f / 748.0f, 354.0f / 702.0f, 153.0f / 748.0f, 158.0f / 702.0f, 2.5f);

	GLuint gobli = LoadTexture("images/monsters/goblin.png");
	goblin[0] = SpriteSheet(gobli, 131.0f / 459.0f, 249.0f / 487.0f, 129.0f / 459.0f, 119.0f / 487.0f, 1.52); 
	goblin[1] = SpriteSheet(gobli, 131.0f / 459.0f, 369.0f / 487.0f, 129.0f / 459.0f, 118.0f / 487.0f, 1.52);
	goblin[2] = SpriteSheet(gobli, 168.0f / 459.0f, 0.0f / 487.0f, 159.0f / 459.0f, 125.0f / 487.0f, 1.52);
	goblin[3] = SpriteSheet(gobli, 0.0f / 459.0f, 0.0f / 487.0f, 167.0f / 459.0f, 124.0f / 487.0f, 1.52);
	goblin[4] = SpriteSheet(gobli, 0.0f / 459.0f, 125.0f / 487.0f, 161.0f / 459.0f, 122.0f / 487.0f, 1.52);
	goblin[5] = SpriteSheet(gobli, 328.0f / 459.0f, 0.0f / 487.0f, 130.0f / 459.0f, 114.0f / 487.0f, 1.52);
	goblin[6] = SpriteSheet(gobli, 0.0f / 459.0f, 248.0f / 487.0f, 130.0f / 459.0f, 112.0f / 487.0f, 1.52);
	goblin[7] = SpriteSheet(gobli, 299.0f / 459.0f, 126.0f / 487.0f, 130.0f / 459.0f, 114.0f / 487.0f, 1.52);
	goblin[8] = SpriteSheet(gobli, 0.0f / 459.0f, 361.0f / 487.0f, 130.0f / 459.0f, 111.0f / 487.0f, 1.52);
	goblin[9] = SpriteSheet(gobli, 261.0f / 459.0f, 249.0f / 487.0f, 126.0f / 459.0f, 109.0f / 487.0f, 1.52);
	goblindamage = SpriteSheet(gobli, 162.0f / 459.0f, 126.0f / 487.0f, 136.0f / 459.0f, 122.0f / 487.0f, 1.52);

	GLuint plan = LoadTexture("images/monsters/plant.png");
	plant[0] = SpriteSheet(plan, 376.0f / 458.0f, 0.0f / 313.0f, 81.0f / 458.0f, 112.0f / 313.0f, 1.52);
	plant[1] = SpriteSheet(plan, 0.0f / 458.0f, 204.0f / 313.0f, 78.0f / 458.0f, 109.0f / 313.0f, 1.52);
	plant[2] = SpriteSheet(plan, 282.0f / 458.0f, 102.0f / 313.0f, 81.0f / 458.0f, 104.0f / 313.0f, 1.52);
	plant[3] = SpriteSheet(plan, 364.0f / 458.0f, 113.0f / 313.0f, 81.0f / 458.0f, 104.0f / 313.0f, 1.52);
	plant[4] = SpriteSheet(plan, 0.0f / 458.0f, 0.0f / 313.0f, 228.0f / 458.0f, 101.0f / 313.0f, 1.52);
	plant[5] = SpriteSheet(plan, 0.0f / 458.0f, 102.0f / 313.0f, 181.0f / 458.0f, 101.0f / 313.0f, 1.52);
	plant[6] = SpriteSheet(plan, 229.0f / 458.0f, 0.0f / 313.0f, 146.0f / 458.0f, 101.0f / 313.0f, 1.52);
	plantdamage = SpriteSheet(plan, 182.0f / 458.0f, 102.0f / 313.0f, 99.0f / 458.0f, 120.0f / 313.0f, 1.52);

	GLuint skele = LoadTexture("images/monsters/skeleton.png");
	skeleton[0] = SpriteSheet(skele, 86.0f / 319.0f, 93.0f / 193.0f, 84.0f / 319.0f, 95.0f / 193.0f, 1.82);
	skeleton[1] = SpriteSheet(skele, 171.0f / 319.0f, 93.0f / 193.0f, 73.0f / 319.0f, 100.0f / 193.0f, 1.82);
	skeleton[2] = SpriteSheet(skele, 0.0f / 319.0f, 0.0f / 193.0f, 99.0f / 319.0f, 91.0f / 193.0f, 1.82);
	skeleton[3] = SpriteSheet(skele, 100.0f / 319.0f, 0.0f / 193.0f, 90.0f / 319.0f, 92.0f / 193.0f, 1.82);
	skeletondamage = SpriteSheet(skele, 0.0f / 319.0f, 92.0f / 193.0f, 85.0f / 319.0f, 96.0f / 193.0f, 1.82);
	skeleton[4] = SpriteSheet(skele, 245.0f / 319.0f, 0.0f / 193.0f, 73.0f / 319.0f, 93.0f / 193.0f, 1.82);

	GLuint wrait = LoadTexture("images/monsters/wraith.png");
	wraith[0] = SpriteSheet(wrait, 224.0f / 424.0f, 88.0f / 253.0f, 84.0f / 424.0f, 80.0f / 253.0f, 1.52);
	wraith[1] = SpriteSheet(wrait, 339.0f / 424.0f, 0.0f / 253.0f, 84.0f / 424.0f, 80.0f / 253.0f, 1.52);
	wraith[2] = SpriteSheet(wrait, 0.0f / 424.0f, 175.0f / 253.0f, 88.0f / 424.0f, 78.0f / 253.0f, 1.52);
	wraith[3] = SpriteSheet(wrait, 115.0f / 424.0f, 0.0f / 253.0f, 114.0f / 424.0f, 87.0f / 253.0f, 1.52);
	wraith[4] = SpriteSheet(wrait, 113.0f / 424.0f, 90.0f / 253.0f, 110.0f / 424.0f, 87.0f / 253.0f, 1.52);
	wraith[5] = SpriteSheet(wrait, 0.0f / 424.0f, 0.0f / 253.0f, 114.0f / 424.0f, 89.0f / 253.0f, 1.52);
	wraith[6] = SpriteSheet(wrait, 230.0f / 424.0f, 0.0f / 253.0f, 108.0f / 424.0f, 86.0f / 253.0f, 1.52);
	wraith[7] = SpriteSheet(wrait, 0.0f / 424.0f, 90.0f / 253.0f, 112.0f / 424.0f, 84.0f / 253.0f, 1.52);
	wraithdamage = SpriteSheet(wrait, 309.0f / 424.0f, 87.0f / 253.0f, 83.0f / 424.0f, 88.0f / 253.0f, 1.52);

	GLuint drago = LoadTexture("images/monsters/dragon.png");
	dragon[0] = SpriteSheet(drago, 1100.0f / 1853.0f, 331.0f / 1007.0f, 191.0f / 1853.0f, 262.0f / 1007.0f, 2.5f);
	dragon[1] = SpriteSheet(drago, 1099.0f / 1853.0f, 594.0f / 1007.0f, 184.0f / 1853.0f, 287.0f / 1007.0f, 2.5f);
	dragon[2] = SpriteSheet(drago, 247.0f / 1853.0f, 714.0f / 1007.0f, 216.0f / 1853.0f, 293.0f / 1007.0f, 2.5f);
	dragon[3] = SpriteSheet(drago, 475.0f / 1853.0f, 484.0f / 1007.0f, 216.0f / 1853.0f, 293.0f / 1007.0f, 2.5f);
	dragon[4] = SpriteSheet(drago, 692.0f / 1853.0f, 474.0f / 1007.0f, 213.0f / 1853.0f, 338.0f / 1007.0f, 2.5f);
	dragon[5] = SpriteSheet(drago, 1269.0f / 1853.0f, 0.0f / 1007.0f, 208.0f / 1853.0f, 330.0f / 1007.0f, 2.5f);
	dragon[6] = SpriteSheet(drago, 906.0f / 1853.0f, 542.0f / 1007.0f, 192.0f / 1853.0f, 326.0f / 1007.0f, 2.5f);
	dragon[7] = SpriteSheet(drago, 1474.0f / 1853.0f, 0.0f / 1007.0f, 191.0f / 1853.0f, 327.0f / 1007.0f, 2.5f);
	dragon[8] = SpriteSheet(drago, 1292.0f / 1853.0f, 331.0f / 1007.0f, 187.0f / 1853.0f, 327.0f / 1007.0f, 2.5f);
	dragon[9] = SpriteSheet(drago, 1666.0f / 1853.0f, 0.0f / 1007.0f, 186.0f / 1853.0f, 330.0f / 1007.0f, 2.5f);
	dragon[10] = SpriteSheet(drago, 1063.0f / 1853.0f, 0.0f / 1007.0f, 205.0f / 1853.0f, 296.0f / 1007.0f, 2.5f);
	dragon[11] = SpriteSheet(drago, 247.0f / 1853.0f, 484.0f / 1007.0f, 227.0f / 1853.0f, 229.0f / 1007.0f, 2.5f);
	dragon[12] = SpriteSheet(drago, 0.0f / 1853.0f, 478.0f / 1007.0f, 246.0f / 1853.0f, 232.0f / 1007.0f, 2.5f);
	dragon[13] = SpriteSheet(drago, 582.0f / 1853.0f, 0.0f / 1007.0f, 271.0f / 1853.0f, 236.0f / 1007.0f,2.5f);
	dragon[14] = SpriteSheet(drago, 294.0f / 1853.0f, 0.0f / 1007.0f, 287.0f / 1853.0f, 241.0f / 1007.0f, 2.5f);
	dragon[15] = SpriteSheet(drago, 0.0f / 1853.0f, 0.0f / 1007.0f, 293.0f / 1853.0f, 238.0f / 1007.0f, 2.5f);
	dragon[16] = SpriteSheet(drago, 582.0f / 1853.0f, 237.0f / 1007.0f, 271.0f / 1853.0f, 236.0f / 1007.0f, 2.5f);
	dragon[17] = SpriteSheet(drago, 294.0f / 1853.0f, 242.0f / 1007.0f, 287.0f / 1853.0f, 241.0f / 1007.0f, 2.5f);
	dragon[18] = SpriteSheet(drago, 0.0f / 1853.0f, 239.0f / 1007.0f, 293.0f / 1853.0f, 238.0f / 1007.0f, 2.5f);
	dragon[19] = SpriteSheet(drago, 0.0f / 1853.0f, 711.0f / 1007.0f, 246.0f / 1853.0f, 232.0f / 1007.0f, 2.5f);
	dragon[20] = SpriteSheet(drago, 854.0f / 1853.0f, 0.0f / 1007.0f, 208.0f / 1853.0f, 237.0f / 1007.0f, 2.5f);
	dragon[21] = SpriteSheet(drago, 906.0f / 1853.0f, 297.0f / 1007.0f, 193.0f / 1853.0f, 244.0f / 1007.0f, 2.5f);
	dragondamage = SpriteSheet(drago, 1480.0f / 1853.0f, 328.0f / 1007.0f, 184.0f / 1853.0f, 287.0f / 1007.0f, 2.5f);

	//Entities
	bgPict = Entity(SpriteSheet(stagescreen, .1, 1, 9000, 1, 8.0f));

	//shader
	// daynight = glGetUniformLocation(program->programID, "timer");

	//effects
	damageEffectM = LoadTexture("images/monsterhiteff.png");
	damageEffectH = LoadTexture("images/swordhitef.png");
}

void gameApp::DrawText(int fontTexture, std::string text, float size, float spacing)
{
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++)
	{
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(),
		{
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		}
		);
		texCoordData.insert(texCoordData.end(),
		{
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		}
		);
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
};

GLuint gameApp::LoadTexture(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	return textureID;
};

GLuint gameApp::LoadTexture24Bit(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
};

void gameApp::UpdateAndRender()
{
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	counter += elapsed;
	daynighttime += elapsed;
	glUniform1f(daynight, ticks);

	if (state == MAIN_QUEST && stage != MONSTER_ENCOUNTER && state != IDLE) { runningCounter += elapsed; }
	while (SDL_PollEvent(&event))
	{
		if (Mix_PlayingMusic() == 0 && state == MAIN_MENU){ Mix_FadeInMusic(titlemewsic, -1, 500); }
		if (Mix_PlayingMusic() == 0 && state == MAIN_QUEST){ Mix_FadeInMusic(stagemewsic, -1, 500); }
		if (Mix_PlayingMusic() == 0 && state == MAIN_DEATH){ Mix_FadeInMusic(gameovermewsic, -1, 500); }

		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			done = true;
		}

		if (state == MAIN_MENU)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_X)
			{
				state = MAIN_QUEST;
				stage = IDLE;
				runningCounter = 0;
				Mix_FadeOutMusic(200);
				initGameStats();
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
		}

		if (state == MAIN_QUEST)
		{
			if (stage == IDLE)
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_Z)
				{
					counter = 0;
					stage = RUNNING;
					runningCounter = 0;
					glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				}
			}
			if (stage == RUNNING)
			{
				if (runningCounter >= 7)
				{
					runningCounter = 0;
					std::string s = upcomingEvents[0];
					if (s == "Shrine") { stage = SHRINE_ENCOUNTER; }
					if (s == "Blacksmith") { stage = BLACKSMITH_ENCOUNTER; }
					if (s == "Item Shop") { stage = ITEMSHOP_ENCOUNTER; }
					if (s == "Encounter") { stage = MONSTER_ENCOUNTER; }
					dealwithEvents();
					reset();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_Z)
				{
					srand(time(NULL));
					int random = rand() % 10 + 1;
					if (random <= 2) { gold++; }
					else { goldcounter++; };
					if (goldcounter >= 10) { gold++; goldcounter = 0; }
				}
			}
			if (stage == SHRINE_ENCOUNTER)
			{

				if (runningCounter >= 7)
				{
					runningCounter = 0;
					dealwithEvents();
					stage = RUNNING;
					hasNotActed = true;
				}
				if (hasNotActed)
				{
					if (event.key.keysym.scancode == SDL_SCANCODE_X && gold >= numOfShrinesUsed * 1000)
					{
						//weak upgrade
						Mix_PlayChannel(-1, interact, 1);
						heroCurrentHp += (.05 * heroCurrentHp);
						heroMaxHp += (.05 * heroMaxHp);
						heroAtk += (.05 * heroAtk);
						heroDef += (.05 * heroDef);
						gold -= numOfShrinesUsed * 1000;
						numOfShrinesUsed++;
						hasNotActed = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_Z && gold >= numOfShrinesUsed * 5000)
					{
						//strong upgrade
						Mix_PlayChannel(-1, interact, 1);
						heroCurrentHp += (.2 * heroCurrentHp);
						heroMaxHp += (.2 * heroMaxHp);
						heroAtk += (.2 * heroAtk);
						heroDef += (.2 * heroDef);
						gold -= numOfShrinesUsed * 1000;
						numOfShrinesUsed++;
						hasNotActed = false;
					}
				}
			}
			if (stage == BLACKSMITH_ENCOUNTER)
			{
				if (runningCounter >= 7)
				{
					runningCounter = 0;
					dealwithEvents();
					stage = RUNNING;
					hasNotActed = true;
				}
				if (hasNotActed)
				{
					if (event.key.keysym.scancode == SDL_SCANCODE_X && gold >= (weaponIndex + 1) * 250)
					{
						//weapon upgrade;
						Mix_PlayChannel(-1, interact, 1);
						srand(time(NULL)); float r = 1 + (float(rand() % 100 + 1)) / 100.0f;
						heroAtk += 50 * r;
						gold -= (weaponIndex + 1) * 250;
						weaponIndex++;
						if (weaponIndex >= 11) { weaponIndex = 10; }
						hasNotActed = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_Z && gold >= (armorIndex + 1) * 250)
					{
						//armor upgrade
						Mix_PlayChannel(-1, interact, 1);
						srand(time(NULL)); float r = 1 + (float(rand() % 100 + 1)) / 100.0f;
						heroDef += 25 * r;
						heroMaxHp += (.25 * heroMaxHp)*r;
						gold -= (armorIndex + 1) * 250;
						armorIndex++;
						if (armorIndex >= 11) { armorIndex = 10; }
						hasNotActed = false;
					}
				}
			}
			if (stage == ITEMSHOP_ENCOUNTER)
			{
				if (runningCounter >= 7)
				{
					runningCounter = 0;
					dealwithEvents();
					stage = RUNNING;
					hasNotActed = true;
				}
				if (hasNotActed)
				{
					if (event.key.keysym.scancode == SDL_SCANCODE_X && gold >= 500 && hasItem == false)
					{
						//random item
						Mix_PlayChannel(-1, interact, 1);
						srand(time(NULL)); int random = rand() %8 + 1;
						if (random == 1) { item = "Divine Edge"; }
						if (random == 2) { item = "Skill Scroll"; }
						if (random == 3) { item = "Second Chance"; }
						if (random > 3) { item = "Potion"; }
						gold -= 500;
						hasNotActed = false;
						hasItem = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_Z && gold > 250)
					{
						//rest at inn
						Mix_PlayChannel(-1, interact, 1);
						heroCurrentHp = heroMaxHp;
						gold -= 250;
						hasNotActed = false;
					}
				}
			}
			if (stage == MONSTER_ENCOUNTER)
			{
				runningCounter = 0;
				if (heroCurrentHp <= 0) { state = MAIN_DEATH; }
				//if (monsterCurrentHp <= 0) { stage = RUNNING; }
				if (event.key.keysym.scancode == SDL_SCANCODE_Z)
				{
					srand(time(NULL));
					int random = rand() % 10 + 1;
					if (random <= 3) { gold++; }
					else { goldcounter++; };
					if (goldcounter >= 10) { gold++; goldcounter = 0; }
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_X)
				{
					if (hasItem)
					{
						if (item == "Potion") { item = "None"; heroCurrentHp = heroMaxHp; }
						if (item == "Skill Scroll")
						{
							item = "None";
							if (!crithit){ crithit = true; knownSkills += "CH"; }
							else if (!soulguard) { soulguard = true; knownSkills += " SG"; }
							else if (!lightningslash) { lightningslash = true; knownSkills += " LS"; }
							else if (!heavanSplit) { heavanSplit = true; knownSkills += " HS"; }
							else if (!bladeCall) { bladeCall = true; knownSkills += " BC"; }
							else { gold = gold * 5; }
						} //learn skill
						if (item == "Divine Edge") { item = "None"; usedDivineEdge = true; }
						if (item == "Second Chance") { item = "None"; usedSecondChance = true; }
						Mix_PlayChannel(-1, interact, 1);
						hasItem = false;
					}
				}
			}
		}
	}
		if (state == MAIN_DEATH)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
			{
				done = true;
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_X)
			{
				state = MAIN_MENU;
				stage = IDLE;
			}
		}
	//testing 
	Render();
	Update();
}

void gameApp::Render()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state)
	{
			case MAIN_MENU:
			{
									RenderMainMenu();
									break;
			}
			case MAIN_QUEST:
			{
									RenderQuest();
									break;
			}
			case MAIN_DEATH:
			{
									RenderGameOver();
									break;
			}
	}
}

void gameApp::RenderMainMenu()
{
	Entity ts = Entity(SpriteSheet(titlescreen, 1, 1, 1,1, 8.0f));
	ts.Draw(program);
	//implement shade in via shaders
	modelMatrix.identity();
	modelMatrix.Translate(-2.5f, 3.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Touhou Quest", .7f, -.22f);

	modelMatrix.identity();
	modelMatrix.Translate(-2.0f, 2.4f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Z, X Buttons To Interact", .3f, -.189f);

	modelMatrix.identity();
	modelMatrix.Translate(-2.0f, 2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Generate Income With Z", .3f, -.159f);
	SDL_GL_SwapWindow(displayWindow);

}

void gameApp::RenderQuest()
{
	modelMatrix.identity();
	bgPict.Draw(program);
	//implement shade in via shaders
	modelMatrix.identity();
	modelMatrix.Translate(-3.5f, 3.3f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "HP:" + std::to_string(heroCurrentHp) + "/" + std::to_string(heroMaxHp), .4f, -.189f);

	modelMatrix.identity();
	modelMatrix.Translate(-3.5f, 2.9f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Weapon:" + weaponNames[weaponIndex] , .3f, -.15f);

	modelMatrix.identity();
	modelMatrix.Translate(-3.5f, 2.7f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Armor:" + armorNames[armorIndex], .3f, -.15f);

	modelMatrix.identity();
	modelMatrix.Translate(-3.5f, 2.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Gold:" +std::to_string(gold), .3f, -.15f);

	modelMatrix.identity();
	modelMatrix.Translate(-3.5f, 2.2f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Known Skills:" + knownSkills , .28f, -.15f);

	modelMatrix.identity();
	modelMatrix.Translate(-3.5, 1.9f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Item:", .3f, -.15f);
	if (hasItem)
	{ 
		if (item == "Potion")
		{
			activeItem = Entity(SpriteSheet(potion, 1, 1, 1, 1, .4f));
			activeItem.x = -2.7f;
			activeItem.y = 1.9f;
			activeItem.Draw(program);
		}
		if (item == "Skill Scroll")
		{
			activeItem = Entity(SpriteSheet(skillScroll, 1, 1, 1, 1, .4f));
			activeItem.x = -2.7f;
			activeItem.y = 1.9f;
			activeItem.Draw(program);
		}
		if (item == "DivineEdge")
		{
			activeItem= Entity(SpriteSheet(divineEdge, 1, 1, 1, 1, .4f));
			activeItem.x = -2.7f;
			activeItem.y = 1.9f;
			activeItem.Draw(program);
		}
		if (item == "SecondChance")
		{
			activeItem = Entity(SpriteSheet(secondChance, 1, 1, 1, 1, .4f));
			activeItem.x = -2.7f;
			activeItem.y = 1.9f;
			activeItem.Draw(program);
		}
	}

	// upcoming events bar
	modelMatrix.identity();
	modelMatrix.Translate(2.6f, 3.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Encounters", .36f, -.23f);
	float xm = 3.3f;
	for (int i = 0; i < upcomingEvents.size(); i++)
	{
		modelMatrix.identity();
		modelMatrix.Translate(2.6f, xm, 0.0f);
		program->setModelMatrix(modelMatrix);
		modelMatrix.identity();
		DrawText(fontSprites, std::to_string(i+1) + upcomingEvents[i], .2f, -.1f);
		xm -= .2f;
	}

	//character sprites

	if (stage == IDLE)
	{
		playerNumFrames = sizeof(idle) / sizeof(idle[0]);
		if (counter > .15) 
		{
			playerIndex++; counter = 0; 
			if (playerIndex >= playerNumFrames) { playerIndex = 0; }
		}
		player = Entity(idle[playerIndex]);
		player.x = -2.0f; player.y = -1.0f;
		player.Draw(program);

	}

	if ( stage!= MONSTER_ENCOUNTER  && stage!= IDLE)
	{
		playerNumFrames = sizeof(dash) / sizeof(dash[0]);
		if (counter > .15)
		{
			playerIndex++; counter = 0;
			if (playerIndex >= playerNumFrames) { playerIndex = 0; }
		}
		player = Entity(dash[playerIndex]);
		player.x = -2.0f+xOffset; player.y = -1.0f +yOffset;
		player.Draw(program);
		scrollBackground();
		if (xIncreasing) { xOffset += .003f; }
		if (!xIncreasing) { xOffset -= .003f; }
		if (yIncreasing) { yOffset += .003f; }
		if (!yIncreasing) { yOffset -= .003f; }
		if (player.x <= MAX_LEFT) { xIncreasing = true; }
		if (player.x >= MAX_RIGHT) { xIncreasing = false; }
		if (player.y <= MAX_BOT) { yIncreasing = true; }
		if (player.y >= MAX_TOP) { yIncreasing = false; }
	}
	if (stage == SHRINE_ENCOUNTER)
	{
		shrine = Entity(SpriteSheet(shrineImg, 1, 1, 1, 1, 2.4f));
		scrollShrine();
		shrine.Draw(program);

		modelMatrix.identity();
		modelMatrix.Translate(-.5f, 3.3f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "Z: Great Blessing: " + std::to_string(numOfShrinesUsed*5000), .2f, -.10f);
		modelMatrix.identity();

		modelMatrix.Translate(-.5f, 3.0f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "X: Minor Blessing: " + std::to_string(numOfShrinesUsed*1000), .2f, -.10f);
	}
	if (stage == BLACKSMITH_ENCOUNTER)
	{
		blacksmith = Entity(SpriteSheet(blacksmithImg, 1, 1, 1, 1, 2.4f));
		scrollSmithy();
		blacksmith.Draw(program);

		modelMatrix.identity();
		modelMatrix.Translate(-.5f, 3.3f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "Z: Armor Upgrade: "+ std::to_string((armorIndex+1)*250), .2f, -.10f);

		modelMatrix.identity();
		modelMatrix.Translate(-.5f, 3.0f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "X: Weapon Upgrade: " + std::to_string((weaponIndex+1)*250), .2f, -.10f);
	}
	if (stage ==ITEMSHOP_ENCOUNTER)
	{
		itemshop = Entity(SpriteSheet(itemshopImg, 1, 1, 1, 1, 2.4f));
		scrollItemShop();
		itemshop.Draw(program);

		modelMatrix.identity();
		modelMatrix.Translate(-.5f, 3.3f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "Z: Rest at Inn:" + std::to_string(250) , .2f, -.10f);

		modelMatrix.identity();
		modelMatrix.Translate(-.5f, 3.0f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "X: Buy Random Item:" + std::to_string(500), .2f, -.10f);

	}
	if (stage == MONSTER_ENCOUNTER)
	{
		//play dash end animation
		if (hasNotPlayedStopAnimation)
		{
			playerNumFrames = sizeof(encounter) / sizeof(encounter[0]);
			if (counter > .25)
			{
				playerIndex++; counter = 0;
				if (playerIndex >= playerNumFrames) { playerIndex = 0; hasNotPlayedStopAnimation = false; }
			}
			player = Entity(encounter[playerIndex]);
			player.Draw(program);
		}
		//generates a monster
		if (!monsterExists)
		{
			srand(time(NULL));
			int random = rand() % 100 + 1;
			if (random < 30) { monstertype = PLANT; }
			if (random >= 30 && random < 58 ) { monstertype = GOBLIN; }
			if (random >= 58 && random < 78) { monstertype = SKELETON; }
			if (random >= 78 && random < 93) { monstertype = WRAITH; }
			if (random >= 93 && random < 97) { monstertype = DEMON; }
			if (random >= 97 && random <= 100) { monstertype = DRAGON; }
			/* 
			if (random == 100) //so help you god
			{
				stage = BOSS_ENCOUNTER;
				monstertype = BOSS;
			}
			*/
			if (monstertype == PLANT)
			{
				srand(time(NULL));
				float r = 1+ (rand() % 20 + 1)/100.0f;
				monsterMaxHp = (.4*heroMaxHp)*r;
				monsterCurrentHp = monsterMaxHp;
				monsterAtk = (.6*heroAtk) *r;
				monster = Entity(plant[0]);
				monster.x = player.x +2.0f;
				monster.y = player.y;
				monster.Draw(program);
			}
			if (monstertype == GOBLIN)
			{
				srand(time(NULL));
				float r = 1+ (rand() % 30 + 1) / 100.9f;
				monsterMaxHp = (.6*heroMaxHp)*r;
				monsterCurrentHp = monsterMaxHp;
				monsterAtk = (.5*heroAtk) *r;
				monster = Entity(goblin[0]);
				monster.x = player.x + 2.0f;
				monster.y = player.y;
				monster.Draw(program);
			}
			if (monstertype == SKELETON)
			{
				srand(time(NULL));
				float r = 1+ (rand() % 40 + 1) / 100.0f;
				monsterMaxHp = (1.2*heroMaxHp)*r;
				monsterCurrentHp = monsterMaxHp;
				monsterAtk = (.4*heroAtk) *r;
				monster = Entity(skeleton[0]);
				monster.x = player.x + 2.0f;
				monster.y = player.y;
				monster.Draw(program);
			}
			if (monstertype == WRAITH)
			{
				srand(time(NULL));
				float r = 1+ (rand() % 50 + 1) /100.0f;
				monsterMaxHp = (.4*heroMaxHp)*r;
				monsterCurrentHp = monsterMaxHp;
				monsterAtk = (1.2*heroAtk) *r;
				monster = Entity(wraith[0]);
				monster.x = player.x + 2.0f;
				monster.y = player.y;
				monster.Draw(program);
			}
			if (monstertype == DEMON)
			{
				srand(time(NULL));
				float r = 1+ (rand() % 90 + 1) / 100.0f;
				monsterMaxHp = (1.3*heroMaxHp)*r;
				monsterCurrentHp = monsterMaxHp;
				monsterAtk = (1.3*heroAtk) *r;
				monster = Entity(balrog[0]);
				monster.x = player.x + 2.0f;
				monster.y = player.y;
				monster.Draw(program);
			}
			if (monstertype == DRAGON)
			{
				srand(time(NULL));
				float r = 1+ (rand() % 130+ 1) / 100.0f;
				monsterMaxHp = (1.3*heroMaxHp)*r;
				monsterCurrentHp = monsterMaxHp;
				monsterAtk = (1.3*heroAtk) *r;
				monster = Entity(dragon[0]);
				monster.x = player.x + 2.0f;
				monster.y = player.y;
				monster.Draw(program);
			}
			/*
			if(monstertype == BOSS)
			{
				srand(time(NULL));
				float r = (rand() * 300 + 1) / 100.0f;
				monsterMaxHp = (2.5*heroMaxHp)*r;
				monsterCurrentHp = monsterMaxHp;
				monsterAtk = (2.5*heroAtk) *r;
			}
			*/
			monsterExists = true;
			
		}

		//player's turn to attack ------------------------------------------------------------------

		if (isPlayerTurn && !playerAnimationFinished &&  !hasNotPlayedStopAnimation)
		{
			if (crithit || soulguard || heavanSplit || bladeCall || lightningslash)
			{
				srand(time(NULL));
				int skillChance = rand() % 17 + 1;
				if (skillChance == 1)//17% to use a skill
				{
					srand(time(NULL));
					int selectedSkill = rand() % 100 + 1;
					if (selectedSkill > 5 && bladeCall) { attackTypeCounter = 4; } //bladeCall
					if (selectedSkill >= 5 && selectedSkill < 15 && heavanSplit) { attackTypeCounter == 5; } // heavan split
					if (selectedSkill >= 15 && selectedSkill < 30 && lightningslash) { attackTypeCounter == 6; } //lightning slash
					if (selectedSkill >= 30 && selectedSkill < 55 && soulguard) { attackTypeCounter == 7; } //soulguard
					else { attackTypeCounter == 8; } //critical hit
				}
			}
			//does one of four attack animations (or 4 skills if available)----------------------------------------------------
			hiteffects = Entity(SpriteSheet(damageEffectH, 1, 1, 1, 1, .4f));
			if (attackTypeCounter == 0)
			{
				playerNumFrames = 11;
				if (counter > .1)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames) 
					{ 
						playerIndex = 0; playerAnimationFinished = true; attackTypeCounter++; drift = 0.0; 
					}
				}
				player = Entity(attack1[playerIndex]);
				player.x += drift-1.5f;
				drift += .003;
				if (playerIndex >= (.5*playerNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
			}
			if (attackTypeCounter == 1)
			{
				playerNumFrames = 13;
				if (counter > .11)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames) { playerIndex = 0; playerAnimationFinished = true; attackTypeCounter++; drift = 0; }
				}
				player = Entity(attack2[playerIndex]);
				player.x += drift-1.5f;
				drift += .003;
				if (playerIndex >= (.5*playerNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
			}
			if (attackTypeCounter == 2)
			{
				playerNumFrames = 10;
				if (counter > .11)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames) { playerIndex = 0; playerAnimationFinished = true; attackTypeCounter++; drift = 0; }
				}
				player = Entity(attack3[playerIndex]);
				player.x += drift-1.5f;
				drift += .003;
				if (playerIndex >= (.5*playerNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
			}
			if (attackTypeCounter == 3)
			{
				playerNumFrames = 10;
				if (counter > .11)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames)
					{ 
						playerIndex = 0; playerAnimationFinished = true;
						attackTypeCounter = 0;
						drift = 0.0f;
					}
				}
				player = Entity(attack1[playerIndex]);
				player.x += drift-1.5f;
				drift += .003;
				if (playerIndex >= (.5*playerNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
			}
			// EoGeom, call and control the blade-------------------------------------
			if (attackTypeCounter == 4)
			{
				playerNumFrames = sizeof(eoGeom) / sizeof(eoGeom[0]);;
				if (counter > .2)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames)
					{
						playerIndex = 0; playerAnimationFinished = true;
						attackTypeCounter = 0;
						drift = 0.0f;
						curSkillUsed = "EoGeom";
					}
				}
				player = Entity(eoGeom[playerIndex]);
				for (int i = 0; i < playerNumFrames - 10; i++)
				{
					Entity geomSword = Entity(callsword);
					geomSword.x = player.x - 1.0f + (i*.2f);
					geomSword.y = player.y + (i*.2f);
					geomSword.Draw(program);
				}
			}
			// heavan's split --------------------------------
			if (attackTypeCounter == 5)
			{
				playerNumFrames = 10;
				if (counter > .21)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames)
					{
						playerIndex = 0; playerAnimationFinished = true;
						attackTypeCounter = 0;
						drift = 0.0f;
						curSkillUsed = "Split";
					}
				}
				player = Entity(skillSplit[playerIndex]);
				for (int i = 0; i < playerNumFrames-7; i++)
				{
					Entity spike = Entity(split);
					spike.x = monster.x +=(i*.02f);
					spike.y = monster.y;
					spike.Draw(program);
				}
			}
			// flash step ----------------------------------------------
			if (attackTypeCounter == 6)
			{
				playerNumFrames = 12;
				if (counter > .21)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames) 
					{ 
						playerIndex = 0; 
						playerAnimationFinished = true; 
						attackTypeCounter= 0; 
						drift = 0; 
						curSkillUsed = "Flashstep";
					}
				}
				player = Entity(flashstep[playerIndex]);
				player.x += drift - 1.0f;
				drift += .01;
				if (playerIndex >= 3)
				{
					Entity stepeffect = Entity(flashstepEff[playerIndex-3]);
					stepeffect.x = player.x - .5f;
					stepeffect.y = player.y;
					stepeffect.Draw(program);
				}
			}
			// soul guard----------------------------------------------------------
			if (attackTypeCounter == 7)
			{
				playerNumFrames = sizeof(useitem)/sizeof(useitem[0]);
				if (counter > .21)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames) 
					{ 
						playerIndex = 0; playerAnimationFinished = true; attackTypeCounter=0; drift = 0; 
						curSkillUsed = "Soulguard";
					}
				}
				player = Entity(useitem[playerIndex]);
			}
			//critical hit
			if (attackTypeCounter == 8)
			{
				playerNumFrames = sizeof(crit) / sizeof(crit[0]);
				if (counter > .21)
				{
					playerIndex++; counter = 0;
					if (playerIndex >= playerNumFrames)
					{
						playerIndex = 0; playerAnimationFinished = true; attackTypeCounter = 0; drift = 0;
						curSkillUsed = "CriticalHit";
					}
				}
				player = Entity(crit[playerIndex]);
				player.x += drift - 1.0f;
				drift += .003;
				Entity critEff = Entity(criteffect[playerIndex]);
				critEff.x = monster.x; critEff.y = monster.y;
				critEff.Draw(program);
			}
			if (monstertype == PLANT) { monster = Entity(plantdamage); monster.x += .5*drift; }
			if (monstertype == GOBLIN) { monster = Entity(goblindamage); monster.x += .5*drift; }
			if (monstertype == SKELETON) { monster = Entity(skeletondamage); monster.x += .5*drift; }
			if (monstertype == WRAITH) { monster = Entity(wraithdamage); monster.x += .5*drift; }
			if (monstertype == DEMON) { monster = Entity(balrogdamage); monster.x += .5*drift; }
			if (monstertype == DRAGON) { monster = Entity(dragondamage); monster.x += .5*drift; }


			// continues if the attack animation is entirely finished -----------------------------------------
			if (playerAnimationFinished)
			{
				//33% chnace to attack twice ----------------------------------------------------------------
				if (rand() % 3 + 1 == 1) { playerAnimationFinished = false; numberofHitsCounter++; }
				else {isPlayerTurn = false; }
				if (numberofHitsCounter > 2) { isPlayerTurn = false; numberofHitsCounter = 0; }
				// stuff after the hit like monster flinch and knockback and sound
				Mix_PlayChannel(-1, swordhit, 1);
				//hit marker
				//what the hit actually does -----------------------------------------------------
				if (usedDivineEdge){ monsterCurrentHp -= heroAtk * 1000; usedDivineEdge = false; curSkillUsed = ""; }
				else if (curSkillUsed == "EoGeom") { monsterCurrentHp -= heroAtk * 5; curSkillUsed = "";}
				else if (curSkillUsed == "Split") { monsterCurrentHp -= heroAtk *3.8; curSkillUsed = ""; }
				else if (curSkillUsed == "Flashstep") { monsterCurrentHp -= heroAtk * 3; curSkillUsed = ""; }
				else if (curSkillUsed == "SoulGuard") { monsterCurrentHp -= 1.5*heroAtk; heroCurrentHp += 1.5*heroAtk + .5*monsterAtk; curSkillUsed = ""; }
				else if (curSkillUsed == "CriticalHit") { monsterCurrentHp -= 2 * heroAtk; curSkillUsed = ""; }
				else { srand(time(NULL)); float mult = 1 + ((rand() % 30 + 1) / 100.0f);  monsterCurrentHp -= heroAtk*mult; }
				if (monsterCurrentHp <= 0)
				{
					stage = RUNNING;
					numberOfEncounters++;
					// get paid--------------------------------------------------------------------
					srand(NULL);
					double goldmultiplier = (1 + (rand() % 100 + 1) / 100.0f);
					double encounterBonus = (1 + (rand() % (numberOfEncounters*10) + 1) / 100.0f);
					if (monstertype == PLANT) { gold += 70 * goldmultiplier*encounterBonus; }
					if (monstertype == GOBLIN) { gold += 80 * goldmultiplier*encounterBonus; }
					if (monstertype == SKELETON) { gold += 110 * goldmultiplier*encounterBonus; }
					if (monstertype == WRAITH) { gold +=130 * goldmultiplier*encounterBonus; }
					if (monstertype == DEMON) { gold += 450 * goldmultiplier*encounterBonus; }
					if (monstertype == DRAGON) { gold += 500 * goldmultiplier*encounterBonus; }
					monster = Entity();
					monsterExists = false;
				}
				//sets up monster attacks
				monsterAnimationFinished = false;
				monsterNumFrames = 0;
				monsterIndex = 0;
			}
			monster.Draw(program);
			player.Draw(program);
		}

		//monsters turn----------------------------------------------------------------------------
		if (!isPlayerTurn && !monsterAnimationFinished)
		{
			hiteffects = Entity(SpriteSheet(damageEffectM, 1, 1, 1, 1, .4f));
			if (!monsterAnimationFinished)
			{
				// plays monster specific animation ------------------------------------------------------------
				if (monstertype == PLANT)
				{
					if (!monsterAnimationFinished)
					{
						monsterNumFrames = sizeof(plant) / sizeof(plant[0]);
						if (counter > .15)
						{
							monsterIndex++; counter = 0;
							if (monsterIndex >= monsterNumFrames) { monsterIndex = 0; monsterAnimationFinished = true; drift = 0; }
						}
						monster = Entity(plant[monsterIndex]);
						monster.x += drift + 1.05f;
						drift -= .003f;
					}
					if (monsterIndex >= (.5*monsterNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
				}
				if (monstertype == GOBLIN)
				{
					if (!monsterAnimationFinished)
					{
						monsterNumFrames = sizeof(goblin) / sizeof(goblin[0]);
						if (counter > .15)
						{
							monsterIndex++; counter = 0;
							if (monsterIndex >= monsterNumFrames) { monsterIndex = 0; monsterAnimationFinished = true; drift = 0.0f; }
						}
						monster = Entity(goblin[monsterIndex]);
						monster.x += drift + 1.5f;
						drift -= .003f;
					}
					if (monsterIndex >= (.5*monsterNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
				}
				if (monstertype == SKELETON)
				{
					if (!monsterAnimationFinished)
					{
						monsterNumFrames = 4;
						if (counter > .15)
						{
							monsterIndex++; counter = 0;
							if (monsterIndex >= monsterNumFrames) { monsterIndex = 0; monsterAnimationFinished = true; drift = 0.0f; }
						}
						monster = Entity(skeleton[monsterIndex]);
						monster.x += drift + 1.5f;
						drift -= .003f;
					}
					if (monsterIndex >= (.5*monsterNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
				}
				if (monstertype == WRAITH)
				{
					if (!monsterAnimationFinished)
					{
						monsterNumFrames = 8;
						if (counter > .15)
						{
							monsterIndex++; counter = 0;
							if (monsterIndex >= monsterNumFrames) { monsterIndex = 0; monsterAnimationFinished = true; drift = 0.0f; }
						}
						monster = Entity(wraith[monsterIndex]);
						monster.x += drift + 1.5f;
						drift -= .003f;
					}
					if (monsterIndex >= (.5*monsterNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
				}
				if (monstertype == DEMON)
				{
					if (!monsterAnimationFinished)
					{
						monsterNumFrames = sizeof(balrog) / sizeof(balrog[0]);
						if (counter > .15)
						{
							monsterIndex++; counter = 0;
							if (monsterIndex >= monsterNumFrames) { monsterIndex = 0; monsterAnimationFinished = true; drift = 0.0f; }
						}
						monster = Entity(balrog[monsterIndex]);
						monster.x += drift + 2.4f;
						drift -= .003f;
					}
					if (monsterIndex >= (.5*monsterNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
				}
				if (monstertype == DRAGON)
				{
					if (!monsterAnimationFinished)
					{
						monsterNumFrames = 20;
						if (counter > .15)
						{
							monsterIndex++; counter = 0;
							if (monsterIndex >= monsterNumFrames) { monsterIndex = 0; monsterAnimationFinished = true; drift = 0.0f; }
						}
						monster = Entity(dragon[monsterIndex]);
						monster.x += drift + 2.4f;
						drift -= .003f;
					}
					if (monsterIndex >= (.5*monsterNumFrames)) { hiteffects.x = monster.x + player.x / 2; hiteffects.y = player.y; hiteffects.Draw(program); }
				}
				player = Entity(takedamage[1]); player.x += .5*drift;
			}
			if (monsterAnimationFinished)
			{
					//25% chnace to attack twice ----------------------------------------------------------------
					if (rand() % 4 + 1 == 1) { monsterAnimationFinished = false; numberofHitsCounter++; }
					else { isPlayerTurn = true; }
					if (numberofHitsCounter > 2) { isPlayerTurn = false; numberofHitsCounter = 1; }
					// stuff after the hit like flinch and knockback and sound
					Mix_PlayChannel(-1, monsterhit, 1);
					player = Entity(takedamage[0]);
					//returns player to original position
					if (monsterAtk > heroDef) { heroCurrentHp -= (monsterAtk - heroDef); }
					else { heroCurrentHp--; }
					if (heroCurrentHp < 0)
					{
						if (usedSecondChance) { heroCurrentHp = (.5 * heroMaxHp); usedSecondChance = false; }
						else
						{
							state = MAIN_DEATH;
							Mix_FadeOutMusic(400);
						}
					}
					//sets up player animations
					playerAnimationFinished = false;
					playerNumFrames = 0;
					playerIndex = 0;
			}

			player.Draw(program);
			monster.Draw(program);
		}
		modelMatrix.identity();
		modelMatrix.Translate(-.5f, 3.3f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "Enemy HP:", .4f, -.189f);
		modelMatrix.identity();
		modelMatrix.Translate(-.5f, 3.0f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, std::to_string(monsterCurrentHp) + "/" + std::to_string(monsterMaxHp), .4f, -.189f);
	}
	SDL_GL_SwapWindow(displayWindow);

}

void gameApp::RenderGameOver()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix.identity();
	modelMatrix.Translate(0.0f, -2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	death[1].Draw(program);

	modelMatrix.identity();
	modelMatrix.Translate(-1.0f, 2.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Game Over", .55f, -.2f);

	modelMatrix.identity();
	modelMatrix.Translate(-1.0f, 2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Sleep now,", .5f, -.2f);

	modelMatrix.identity();
	modelMatrix.Translate(-2.6f, 1.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Half-ghost that seeks death", .4f, -.2f);

	modelMatrix.identity();
	modelMatrix.Translate(-2.6f, 1.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Monsters Defeated:" +std::to_string(numberOfEncounters), .4f, -.2f);

	SDL_GL_SwapWindow(displayWindow);
}

void gameApp::Update()
{
	// got lazy, not needed
}

bool gameApp::isDone()
{
	return done;
}

void gameApp::scrollBackground()
{
	bgPict.x = bgU;
	bgU -= .0225f;
}

void gameApp::scrollShrine()
{
	shrine.x = shrineU;
	shrineU -= .015f;
}

void gameApp::scrollItemShop()
{
	itemshop.x = itemU;
	itemU -= .015f;
}
void gameApp::scrollSmithy()
{
	blacksmith.x = smithU;
	smithU -= .015f;
}
void gameApp::initGameStats()
{
	srand(time(NULL));
	for (int i = 0; i < 4; i++)
	{
		int random = rand() % 7 + 1;
		if (random == 1) { upcomingEvents.push_back("Shrine"); }
		if (random == 2) { upcomingEvents.push_back("Blacksmith"); }
		if (random == 3) { upcomingEvents.push_back("Item Shop"); }
		if (random > 3) { upcomingEvents.push_back("Encounter"); }
	}
}

void gameApp::dealwithEvents()
{
	std::vector<std::string> vs;
	for (int i = 1; i < upcomingEvents.size(); i++)
	{
		vs.push_back(upcomingEvents[i]);
	}
	upcomingEvents = vs;
	srand(time(NULL));
	int random = rand() % 7 + 1;
	if (random == 1) { upcomingEvents.push_back("Shrine"); }
	if (random == 2) { upcomingEvents.push_back("Blacksmith"); }
	if (random == 3) { upcomingEvents.push_back("Item Shop"); }
	if (random > 3) { upcomingEvents.push_back("Encounter"); }
	hasNotActed = true;
}

void gameApp::reset()
{
	shrineU = .022f;
	smithU = .022f;
	itemU = .022f;
	shrineU = 5.0f; shrine.y = 1.0f;
	smithU = 5.0f; blacksmith.y = 1.0f;
	itemU = 5.0f; itemshop.y = 1.0f;
}