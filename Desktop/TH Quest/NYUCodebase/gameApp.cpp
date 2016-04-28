#include "gameApp.h"
#include <SDL_mixer.h>
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

Mix_Chunk *swordhit;
Mix_Chunk *monsterhit;
Mix_Chunk *interact;
Mix_Music *titlemewsic;
Mix_Music *stagemewsic;
Mix_Music *gameovermewsic;
// current sprites
GLuint fontSprites;
GLuint curPlayerImg; //youmu
GLuint shrineImg; 
GLuint curMonsterImg; //need to find pngs
// animation frame sprites
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
//skills
SpriteSheet skillSplit[11];  //heavan's split
SpriteSheet split;
SpriteSheet eoGeom[21]; //Blade Works  //might need to be split
SpriteSheet callsword;
// background pictures
GLuint titlescreen;  
GLuint stagescreen;
//items
GLuint potion;
GLuint divineEdge;
GLuint secondChance;
GLuint skillScroll;

gameApp::gameApp()
{
	Setup();
	state = MAIN_MENU;
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
	displayWindow = SDL_CreateWindow("Touhou Quests", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 880, 640, SDL_WINDOW_OPENGL);
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
	//loadmusic
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	swordhit = Mix_LoadWAV("music/swordslash.wav");
	monsterhit = Mix_LoadWAV("music/gettinghitwave");
	interact = Mix_LoadWAV("music/upgrade.wave");
	titlemewsic = Mix_LoadMUS("music/The Flower Like the Fantasy - Phantasmagoria of Flower View.mp3");
	stagemewsic = Mix_LoadMUS("music/Mystic Oriental Dream - Ancient Temple.mp3");
	gameovermewsic = Mix_LoadMUS("music/Wingless.mp3");
	//load images
	fontSprites = LoadTexture("images/font1.png");
	shrineImg = LoadTexture("images/shrine.png");

	stagescreen = LoadTexture("images/bg.png");
	titlescreen = LoadTexture24Bit("images/title.png");

	potion = LoadTexture("images/potion.png");
	divineEdge = LoadTexture("images/divineblade.png");
	skillScroll = LoadTexture("images/skillscroll.png");
	secondChance = LoadTexture("images/secondchance.png");
	//load sprite sheet animations into arrays [oh man, this would have been easier with an even sprite sheet]
	GLuint youmu = LoadTexture("images/youmu.png");

	attack1[0] = SpriteSheet(youmu, 1425.0f / 1642.0f, 75.0f / 879.0f, 77.0f / 1642.0f, 94.0f / 879.0f, 1.11f);
	attack1[1] = SpriteSheet(youmu, 1149.0f / 1642.0f, 679.0f / 879.0f, 66.0f / 1642.0f, 122.0f / 879.0f, 1.11f);
	attack1[2] = SpriteSheet(youmu, 1254.0f / 1642.0f, 0.0f / 879.0f, 85.0f / 1642.0f, 107.0f / 879.0f, 1.11f);
	attack1[3] = SpriteSheet(youmu, 512.0f / 1642.0f, 669.0f / 879.0f, 96.0f / 1642.0f, 91.0f / 879.0f, 1.11f);
	attack1[4] = SpriteSheet(youmu, 1170.0f / 1642.0f, 599.0f / 879.0f, 68.0f / 1642.0f, 79.0f / 879.0f, 1.11f);
	attack1[5] = SpriteSheet(youmu, 1174.0f / 1642.0f, 417.0f / 879.0f, 73.0f / 1642.0f, 72.0f / 879.0f, 1.11f);
	attack1[6] = SpriteSheet(youmu, 1097.0f / 1642.0f, 528.0f / 879.0f, 72.0f / 1642.0f, 97.0f / 879.0f, 1.11f);
	attack1[7] = SpriteSheet(youmu, 1477.0f / 1642.0f, 170.0f / 879.0f, 71.0f / 1642.0f, 100.0f / 879.0f, 1.11f);
	attack1[8] = SpriteSheet(youmu, 1507.0f / 1642.0f, 0.0f / 879.0f, 77.0f / 1642.0f, 92.0f / 879.0f, 1.11f);
	attack1[9] = SpriteSheet(youmu, 1503.0f / 1642.0f, 93.0f / 879.0f, 76.0f / 1642.0f, 75.0f / 879.0f, 1.11f);
	attack1[10] = SpriteSheet(youmu, 1124.0f / 1642.0f, 802.0f / 879.0f, 66.0f / 1642.0f, 77.0f / 879.0f, 1.11f);

	attack2[0] = SpriteSheet(youmu, 1457.0f / 1642.0f, 372.0f / 879.0f, 66.0f / 1642.0f, 123.0f / 879.0f, 1.11f);
	attack2[1] = SpriteSheet(youmu, 1322.0f / 1642.0f, 176.0f / 879.0f, 77.0f / 1642.0f, 105.0f / 879.0f, 1.11f);
	attack2[2] = SpriteSheet(youmu, 0.0f / 1642.0f, 652.0f / 879.0f, 105.0f / 1642.0f, 79.0f / 879.0f, 1.11f);
	attack2[3] = SpriteSheet(youmu, 213.0f / 1642.0f, 587.0f / 879.0f, 102.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	attack2[4] = SpriteSheet(youmu, 0.0f / 1642.0f, 358.0f / 879.0f, 135.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	attack2[5] = SpriteSheet(youmu, 226.0f / 1642.0f, 113.0f / 879.0f, 161.0f / 1642.0f, 68.0f / 879.0f, 1.11f);
	attack2[6] = SpriteSheet(youmu, 359.0f / 1642.0f, 182.0f / 879.0f, 149.0f / 1642.0f, 70.0f / 879.0f, 1.11f);
	attack2[7] = SpriteSheet(youmu, 0.0f / 1642.0f, 213.0f / 879.0f, 144.0f / 1642.0f, 72.0f / 879.0f, 1.11f);
	attack2[8] = SpriteSheet(youmu, 136.0f / 1642.0f, 437.0f / 879.0f, 123.0f / 1642.0f, 74.0f / 879.0f, 1.11f);
	attack2[9] = SpriteSheet(youmu, 739.0f / 1642.0f, 392.0f / 879.0f, 107.0f / 1642.0f, 70.0f / 879.0f, 1.11f);
	attack2[10] = SpriteSheet(youmu, 0.0f / 1642.0f, 580.0f / 879.0f, 106.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	attack2[11] = SpriteSheet(youmu, 1400.0f / 1642.0f, 253.0f / 879.0f, 71.0f / 1642.0f, 73.0f / 879.0f, 1.11f);
	attack2[12] = SpriteSheet(youmu, 982.0f / 1642.0f, 793.0f / 879.0f, 68.0f / 1642.0f, 77.0f / 879.0f, 1.11f);

	attack3[0] = SpriteSheet(youmu, 989.0f / 1642.0f, 0.0f / 879.0f, 88.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	attack3[1] = SpriteSheet(youmu, 762.0f / 1642.0f, 80.0f / 879.0f, 112.0f / 1642.0f, 85.0f / 879.0f, 1.11f);
	attack3[2] = SpriteSheet(youmu, 1017.0f / 1642.0f, 428.0f / 879.0f, 79.0f / 1642.0f, 107.0f / 879.0f, 1.11f);
	attack3[3] = SpriteSheet(youmu, 214.0f / 1642.0f, 512.0f / 879.0f, 105.0f / 1642.0f, 74.0f / 879.0f, 1.11f);
	attack3[4] = SpriteSheet(youmu, 320.0f / 1642.0f, 509.0f / 879.0f, 102.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	attack3[5] = SpriteSheet(youmu, 423.0f / 1642.0f, 509.0f / 879.0f, 101.0f / 1642.0f, 68.0f / 879.0f, 1.11f);
	attack3[6] = SpriteSheet(youmu, 210.0f / 1642.0f, 752.0f / 879.0f, 101.0f / 1642.0f, 68.0f / 879.0f, 1.11f);
	attack3[7] = SpriteSheet(youmu, 619.0f / 1642.0f, 437.0f / 879.0f, 109.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	attack3[8] = SpriteSheet(youmu, 1292.0f / 1642.0f, 345.0f / 879.0f, 71.0f / 1642.0f, 73.0f / 879.0f, 1.11f);
	attack3[9] = SpriteSheet(youmu, 1319.0f / 1642.0f, 423.0f / 879.0f, 68.0f / 1642.0f, 77.0f / 879.0f, 1.11f);

	attack4[0] = SpriteSheet(youmu, 107.0f / 1642.0f, 512.0f / 879.0f,	106.0f / 1642.0f, 67.0f / 879.0f, 1.11f);
	attack4[1] = SpriteSheet(youmu, 800.0f / 1642.0f, 716.0f / 879.0f, 106.0f / 1642.0f, 74.0f / 879.0f, 1.11f);
	attack4[2] = SpriteSheet(youmu, 1340.0f / 1642.0f, 83.0f / 879.0f, 81.0f / 1642.0f, 91.0f / 879.0f, 1.11f);
	attack4[3] = SpriteSheet(youmu, 1316.0f / 1642.0f, 282.0f / 879.0f, 73.0f / 1642.0f, 62.0f / 879.0f, 1.11f);
	attack4[4] = SpriteSheet(youmu, 395.0f / 1642.0f, 253.0f / 879.0f, 118.0f / 1642.0f, 108.0f / 879.0f, 1.11f);
	attack4[5] = SpriteSheet(youmu, 1067.0f / 1642.0f, 164.0f / 879.0f, 85.0f / 1642.0f, 108.0f / 879.0f, 1.11f);
	attack4[6] = SpriteSheet(youmu, 1174.0f / 1642.0f, 490.0f / 879.0f, 70.0f / 1642.0f, 108.0f / 879.0f, 1.11f);
	attack4[7] = SpriteSheet(youmu, 1191.0f / 1642.0f, 802.0f / 879.0f, 64.0f / 1642.0f, 67.0f / 879.0f, 1.11f);
	attack4[8] = SpriteSheet(youmu, 1388.0f / 1642.0f, 423.0f / 879.0f, 68.0f / 1642.0f, 60.0f / 879.0f, 1.11f);
	attack4[9] = SpriteSheet(youmu, 199.0f / 1642.0f, 821.0f / 879.0f, 72.0f / 1642.0f, 54.0f / 879.0f, 1.11f);

	block[0] = SpriteSheet(youmu, 609.0f / 1642.0f, 675.0f / 879.0f, 95.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	block[1] = SpriteSheet(youmu, 1455.0f / 1642.0f, 570.0f / 879.0f, 63.0f / 1642.0f, 102.0f / 879.0f, 1.11f);

	crit[0] = SpriteSheet(youmu, 1541.0f / 1642.0f, 290.0f / 879.0f, 67.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
	crit[1] = SpriteSheet(youmu, 705.0f / 1642.0f, 716.0f / 879.0f, 94.0f / 1642.0f, 79.0f / 879.0f, 1.11f);
	crit[2] = SpriteSheet(youmu, 1239.0f / 1642.0f, 599.0f / 879.0f, 67.0f / 1642.0f, 98.0f / 879.0f, 1.11f);
	crit[3] = SpriteSheet(youmu, 1472.0f / 1642.0f, 271.0f / 879.0f, 68.0f / 1642.0f, 100.0f / 879.0f, 1.11f);
	crit[4] = SpriteSheet(youmu, 1006.0f / 1642.0f, 633.0f / 879.0f, 72.0f / 1642.0f, 74.0f / 879.0f, 1.11f);
	crit[5] = SpriteSheet(youmu, 1248.0f / 1642.0f, 419.0f / 879.0f, 70.0f / 1642.0f, 72.0f / 879.0f, 1.11f);
	crit[6] = SpriteSheet(youmu, 609.0f / 1642.0f, 757.0f / 879.0f, 94.0f / 1642.0f, 96.0f / 879.0f, 1.11f);
	crit[7] = SpriteSheet(youmu, 723.0f / 1642.0f, 547.0f / 879.0f, 95.0f / 1642.0f, 91.0f / 879.0f, 1.11f);
	crit[8] = SpriteSheet(youmu, 877.0f / 1642.0f, 0.0f / 879.0f, 111.0f / 1642.0f, 91.0f / 879.0f, 1.11f);
	crit[9] = SpriteSheet(youmu, 1425.0f / 1642.0f, 0.0f / 879.0f, 81.0f / 1642.0f, 74.0f / 879.0f, 1.11f);
	crit[10] = SpriteSheet(youmu, 1582.0f / 1642.0f, 582.0f / 879.0f, 59.0f / 1642.0f, 76.0f / 879.0f, 1.11f);
	crit[11] = SpriteSheet(youmu, 1400.0f / 1642.0f, 175.0f / 879.0f, 76.0f / 1642.0f, 77.0f / 879.0f, 1.11f);
	crit[12] = SpriteSheet(youmu, 963.0f / 1642.0f, 247.0f / 879.0f, 86.0f / 1642.0f, 80.0f / 879.0f, 1.11f);

	criteffect[0] = SpriteSheet(youmu, 312.0f / 1642.0f, 751.0f / 879.0f, 99.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
	criteffect[1] = SpriteSheet(youmu, 850.0f / 1642.0f, 376.0f / 879.0f, 106.0f / 1642.0f, 87.0f / 879.0f, 1.11f);
	criteffect[2] = SpriteSheet(youmu, 628.0f / 1642.0f, 347.0f / 879.0f, 110.0f / 1642.0f, 89.0f / 879.0f, 1.11f);
	criteffect[3] = SpriteSheet(youmu, 514.0f / 1642.0f, 261.0f / 879.0f, 113.0f / 1642.0f, 94.0f / 879.0f, 1.11f);
	criteffect[4] = SpriteSheet(youmu, 392.0f / 1642.0f, 362.0f / 879.0f, 114.0f / 1642.0f, 98.0f / 879.0f, 1.11f);
	criteffect[5] = SpriteSheet(youmu, 641.0f / 1642.0f, 101.0f / 879.0f, 115.0f / 1642.0f, 99.0f / 879.0f, 1.11f);
	criteffect[6] = SpriteSheet(youmu, 642.0f / 1642.0f, 0.0f / 879.0f, 119.0f / 1642.0f, 100.0f / 879.0f, 1.11f);
	criteffect[7] = SpriteSheet(youmu, 269.0f / 1642.0f, 344.0f / 879.0f, 122.0f / 1642.0f, 97.0f / 879.0f, 1.11f);
	criteffect[8] = SpriteSheet(youmu, 396.0f / 1642.0f, 49.0f / 879.0f, 122.0f / 1642.0f, 93.0f / 879.0f, 1.11f);
	criteffect[9] = SpriteSheet(youmu, 270.0f / 1642.0f, 253.0f / 879.0f, 124.0f / 1642.0f, 90.0f / 879.0f, 1.11f);
	criteffect[10] = SpriteSheet(youmu, 519.0f / 1642.0f, 0.0f / 879.0f, 122.0f / 1642.0f, 88.0f / 879.0f, 1.11f);
	criteffect[11] = SpriteSheet(youmu, 519.0f / 1642.0f, 89.0f / 879.0f, 121.0f / 1642.0f, 87.0f / 879.0f, 1.11f);
	criteffect[12] = SpriteSheet(youmu, 514.0f / 1642.0f, 177.0f / 879.0f, 117.0f / 1642.0f, 83.0f / 879.0f, 1.11f);

	death[0] = SpriteSheet(youmu, 0.0f / 1642.0f, 814.0f / 879.0f, 102.0f / 1642.0f, 52.0f / 879.0f, 1.11f);
	death[1] = SpriteSheet(youmu, 369.0f / 1642.0f, 461.0f / 879.0f, 107.0f / 1642.0f, 47.0f / 879.0f, 1.11f);

	encounter[0] = SpriteSheet(youmu, 757.0f / 1642.0f, 166.0f / 879.0f, 111.0f / 1642.0f, 72.0f / 879.0f, 1.11f);
	encounter[1] = SpriteSheet(youmu, 412.0f / 1642.0f, 744.0f / 879.0f, 97.0f / 1642.0f, 76.0f / 879.0f, 1.11f);
	encounter[2] = SpriteSheet(youmu, 1078.0f / 1642.0f, 82.0f / 879.0f, 87.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	encounter[3] = SpriteSheet(youmu, 855.0f / 1642.0f, 313.0f / 879.0f, 107.0f / 1642.0f, 62.0f / 879.0f, 1.11f);
	encounter[4] = SpriteSheet(youmu, 260.0f / 1642.0f, 442.0f / 879.0f, 108.0f / 1642.0f, 61.0f / 879.0f, 1.11f);
	encounter[5] = SpriteSheet(youmu, 855.0f / 1642.0f, 241.0f / 879.0f, 107.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	encounter[6] = SpriteSheet(youmu, 523.0f / 1642.0f, 591.0f / 879.0f, 98.0f / 1642.0f, 77.0f / 879.0f, 1.11f);
	
	flashstepEff[0] = SpriteSheet(youmu, 103.0f / 1642.0f, 815.0f / 879.0f, 95.0f / 1642.0f, 60.0f / 879.0f, 1.11f);
	flashstepEff[1] = SpriteSheet(youmu, 203.0f / 1642.0f, 182.0f / 879.0f, 155.0f / 1642.0f, 63.0f / 879.0f, 1.11f);
	flashstepEff[2] = SpriteSheet(youmu, 0.0f / 1642.0f, 50.0f / 879.0f, 231.0 / 1642.0f, 61.0f / 879.0f, 1.11f);
	flashstepEff[3] = SpriteSheet(youmu, 0.0f / 1642.0f, 0.0f / 879.0f, 249.0f / 1642.0f, 49.0f / 879.0f, 1.11f);
	flashstepEff[4] = SpriteSheet(youmu, 0.0f / 1642.0f, 112.0f / 879.0f, 225.0f / 1642.0f, 45.0f / 879.0f, 1.11f);
	flashstepEff[5] = SpriteSheet(youmu, 250.0f / 1642.0f, 0.0f / 879.0f, 197.0f / 1642.0f, 48.0f / 879.0f, 1.11f);
	flashstepEff[6] = SpriteSheet(youmu, 0.0f / 1642.0f, 158.0f / 879.0f, 202.0f / 1642.0f, 54.0f / 879.0f, 1.11f);
	flashstepEff[7] = SpriteSheet(youmu, 232.0f / 1642.0f, 50.0f / 879.0f, 163.0f / 1642.0f, 62.0f / 879.0f, 1.11f);
	flashstepEff[8] = SpriteSheet(youmu, 0.0f / 1642.0f, 286.0f / 879.0f, 141.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	flashstepEff[9] = SpriteSheet(youmu, 719.0f / 1642.0f, 639.0f / 879.0f, 95.0f / 1642.0f, 76.0f / 879.0f, 1.11f);
	
	flashstep[0] = SpriteSheet(youmu, 762.0f / 1642.0f, 0.0f / 879.0f, 114.0f / 1642.0f, 79.0f / 879.0f, 1.11f);
	flashstep[1] = SpriteSheet(youmu, 704.0f / 1642.0f, 796.0f / 879.0f, 93.0f / 1642.0f, 75.0f / 879.0f, 1.11f);
	flashstep[2] = SpriteSheet(youmu, 1153.0f / 1642.0f, 176.0f / 879.0f, 84.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	flashstep[3] = SpriteSheet(youmu, 1253.0f / 1642.0f, 108.0f / 879.0f, 84.0f / 1642.0f, 67.0f / 879.0f, 1.11f);
	flashstep[4] = SpriteSheet(youmu, 1455.0f / 1642.0f, 496.0f / 879.0f, 65.0f / 1642.0f, 73.0f / 879.0f, 1.11f);
	flashstep[5] = SpriteSheet(youmu, 1580.0f / 1642.0f, 93.0f / 879.0f, 59.0f / 1642.0f, 74.0f / 879.0f, 1.11f);
	flashstep[6] = SpriteSheet(youmu, 893.0f / 1642.0f, 715.0f / 879.0f, 91.0f / 1642.0f, 63.0f / 879.0f, 1.11f);
	flashstep[7] = SpriteSheet(youmu, 142.0f / 1642.0f, 286.0f / 879.0f, 127.0f / 1642.0f, 57.0f / 879.0f, 1.11f);
	flashstep[8] = SpriteSheet(youmu, 0.0f / 1642.0f, 441.0f / 879.0f, 117.0f / 1642.0f, 57.0f / 879.0f, 1.11f);
	flashstep[9] = SpriteSheet(youmu, 1017.0f / 1642.0f, 536.0f / 879.0f, 76.0f / 1642.0f, 96.0f / 879.0f, 1.11f);
	flashstep[10] = SpriteSheet(youmu, 1167.0f / 1642.0f, 0.0f / 879.0f, 86.0f / 1642.0f, 92.0f / 879.0f, 1.11f);
	flashstep[11] = SpriteSheet(youmu, 1153.0f / 1642.0f, 248.0f / 879.0f, 83.0f / 1642.0f, 75.0f / 879.0f, 1.11f);
	flashstep[12] = SpriteSheet(youmu, 985.0f / 1642.0f, 715.0f / 879.0f, 70.0f / 1642.0f, 77.0f / 879.0f, 1.11f);

	idle[0] = SpriteSheet(youmu, 819.0f / 1642.0f, 631.0f / 879.0f, 93.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	idle[1] = SpriteSheet(youmu, 914.0f / 1642.0f, 548.0f / 879.0f, 93.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	idle[2] = SpriteSheet(youmu, 826.0f / 1642.0f, 464.0f / 879.0f, 95.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
	idle[3] = SpriteSheet(youmu, 412.0f / 1642.0f, 663.0f / 879.0f, 99.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
	idle[4] = SpriteSheet(youmu, 107.0f / 1642.0f, 580.0f / 879.0f, 105.0f / 1642.0f, 79.0f / 879.0f, 1.11f);
	idle[5] = SpriteSheet(youmu, 745.0f / 1642.0f, 239.0f / 879.0f, 109.0f / 1642.0f, 78.0f / 879.0f, 1.11f);
	idle[6] = SpriteSheet(youmu, 0.0f / 1642.0f, 499.0f / 879.0f, 106.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
	idle[7] = SpriteSheet(youmu, 320.0f / 1642.0f, 581.0f / 879.0f, 101.0f / 1642.0f, 81.0f / 879.0f, 1.11f);

	dash[0] = SpriteSheet(youmu, 632.0f / 1642.0f, 201.0f / 879.0f, 112.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	dash[1] = SpriteSheet(youmu, 740.0f / 1642.0f,318.0f / 879.0f, 109.0f / 1642.0f, 73.0f / 879.0f, 1.11f);
	dash[2] = SpriteSheet(youmu, 507.0f / 1642.0f, 433.0f / 879.0f, 111.0f / 1642.0f, 72.0f / 879.0f, 1.11f);
	dash[3] = SpriteSheet(youmu, 875.0f / 1642.0f, 92.0f / 879.0f, 111.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	dash[4] = SpriteSheet(youmu, 507.0f / 1642.0f, 362.0f / 879.0f, 112.0f / 1642.0f, 70.0f / 879.0f, 1.11f);
	dash[5] = SpriteSheet(youmu, 869.0f / 1642.0f, 166.0f / 879.0f, 109.0f / 1642.0f, 74.0f / 879.0f, 1.11f);
	dash[6] = SpriteSheet(youmu, 628.0f / 1642.0f, 273.0f / 879.0f, 111.0f / 1642.0f, 73.0f / 879.0f, 1.11f);

	skillSplit[0] = SpriteSheet(youmu, 893.0f / 1642.0f, 779.0f / 879.0f, 88.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	skillSplit[1] = SpriteSheet(youmu, 1215.0f / 1642.0f, 332.0f / 879.0f, 76.0f / 1642.0f, 84.0f / 879.0f, 1.11f);
	skillSplit[2] = SpriteSheet(youmu, 1056.0f / 1642.0f, 753.0f / 879.0f, 67.0f / 1642.0f, 101.0f / 879.0f, 1.11f);
	skillSplit[3] = SpriteSheet(youmu, 1254.0f / 1642.0f, 492.0f / 879.0f, 68.0f / 1642.0f, 104.0f / 879.0f, 1.11f);
	skillSplit[4] = SpriteSheet(youmu, 1347.0f / 1642.0f, 706.0f / 879.0f, 63.0f / 1642.0f, 101.0f / 879.0f, 1.11f);
	skillSplit[5] = SpriteSheet(youmu, 1411.0f / 1642.0f, 697.0f / 879.0f, 62.0f / 1642.0f, 94.0f / 879.0f, 1.11f);
	skillSplit[6] = SpriteSheet(youmu, 1382.0f / 1642.0f, 604.0f / 879.0f, 64.0f / 1642.0f, 92.0f / 879.0f, 1.11f);
	skillSplit[7] = SpriteSheet(youmu, 1524.0f / 1642.0f, 376.0f / 879.0f, 66.0f / 1642.0f, 90.0f / 879.0f, 1.11f);
	skillSplit[8] = SpriteSheet(youmu, 1237.0f / 1642.0f, 248.0f / 879.0f, 78.0f / 1642.0f, 83.0f / 879.0f, 1.11f);
	skillSplit[9] = SpriteSheet(youmu, 1166.0f / 1642.0f, 93.0f / 879.0f, 85.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	skillSplit[0] = SpriteSheet(youmu, 979.0f / 1642.0f, 164.0f / 879.0f, 87.0f / 1642.0f, 82.0f / 879.0f, 1.11f);

	split = SpriteSheet(youmu, 106.0f / 1642.0f, 660.0f / 879.0f, 103.0f / 1642.0f, 154.0f / 879.0f, 1.11f);
	
	eoGeom[0] = SpriteSheet(youmu, 1340.0f / 1642.0f, 0.0f / 879.0f, 84.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	eoGeom[1] = SpriteSheet(youmu, 1097.0f / 1642.0f, 428.0f / 879.0f, 76.0f / 1642.0f, 99.0f / 879.0f, 1.11f);
	eoGeom[2] = SpriteSheet(youmu, 1314.0f / 1642.0f, 501.0f / 879.0f, 67.0f / 1642.0f, 121.0f / 879.0f, 1.11f);
	eoGeom[3] = SpriteSheet(youmu, 1549.0f / 1642.0f, 169.0f / 879.0f, 68.0f / 1642.0f, 120.0f / 879.0f, 1.11f);
	eoGeom[4] = SpriteSheet(youmu, 1283.0f / 1642.0f, 706.0f / 879.0f, 63.0f / 1642.0f, 120.0f / 879.0f, 1.11f);
	eoGeom[5] = SpriteSheet(youmu, 1519.0f / 1642.0f, 583.0f / 879.0f, 62.0f / 1642.0f, 119.0f / 879.0f, 1.11f);
	eoGeom[6] = SpriteSheet(youmu, 1524.0f / 1642.0f, 463.0f / 879.0f, 64.0f / 1642.0f, 119.0f / 879.0f, 1.11f);
	eoGeom[7] = SpriteSheet(youmu, 1388.0f / 1642.0f, 484.0f / 879.0f, 66.0f / 1642.0f, 119.0f / 879.0f, 1.11f);
	eoGeom[8] = SpriteSheet(youmu, 1079.0f / 1642.0f, 633.0f / 879.0f, 69.0f / 1642.0f, 119.0f / 879.0f, 1.11f);
	eoGeom[9] = SpriteSheet(youmu, 963.0f / 1642.0f, 328.0f / 879.0f, 84.0f / 1642.0f, 87.0f / 879.0f, 1.11f);
	eoGeom[10] = SpriteSheet(youmu, 1050.0f / 1642.0f, 273.0f / 879.0f, 84.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	eoGeom[11] = SpriteSheet(youmu, 312.0f / 1642.0f, 669.0f / 879.0f, 99.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	eoGeom[12] = SpriteSheet(youmu, 423.0f / 1642.0f, 578.0f / 879.0f, 99.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	eoGeom[13] = SpriteSheet(youmu, 989.0f / 1642.0f, 82.0f / 879.0f, 88.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	eoGeom[14] = SpriteSheet(youmu, 1216.0f / 1642.0f, 698.0f / 879.0f, 66.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	eoGeom[15] = SpriteSheet(youmu, 1411.0f / 1642.0f, 792.0f / 879.0f, 61.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
	eoGeom[16] = SpriteSheet(youmu, 1048.0f / 1642.0f, 356.0f / 879.0f, 83.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	eoGeom[17] = SpriteSheet(youmu, 914.0f / 1642.0f, 630.0f / 879.0f, 91.0f / 1642.0f, 84.0f / 879.0f, 1.11f);
	eoGeom[18] = SpriteSheet(youmu, 729.0f / 1642.0f, 463.0f / 879.0f, 96.0f / 1642.0f, 93.0f / 879.0f, 1.11f);
	eoGeom[19] = SpriteSheet(youmu, 819.0f / 1642.0f, 547.0f / 879.0f, 94.0f / 1642.0f, 83.0f / 879.0f, 1.11f);
	
	callsword = SpriteSheet(youmu, 142.0f / 1642.0f, 334.0f / 879.0f, 126.0f / 1642.0f, 92.0f / 879.0f, 1.11f);

	takedamage[0] = SpriteSheet(youmu, 410.0f / 1642.0f, 547.0f / 761.0f, 96.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	takedamage[1] = SpriteSheet(youmu, 1364.0f / 1642.0f, 345.0f / 879.0f, 70.0f / 1642.0f, 77.0f / 879.0f, 1.11f);
	takedamage[2] = SpriteSheet(youmu, 1135.0f / 1642.0f, 324.0f / 879.0f, 79.0f / 1642.0f, 76.0f / 879.0f, 1.11f);

	useitem[0] = SpriteSheet(youmu, 525.0f / 1642.0f, 509.0f / 879.0f, 99.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	useitem[1] = SpriteSheet(youmu, 1078.0f / 1642.0f, 0.0f / 879.0f, 88.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	useitem[2] = SpriteSheet(youmu, 1307.0f / 1642.0f, 623.0f / 879.0f, 66.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	useitem[3] = SpriteSheet(youmu, 1474.0f / 1642.0f, 703.0f / 879.0f, 61.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
	useitem[3] = SpriteSheet(youmu, 1238.0f / 1642.0f, 176.0f / 879.0f, 83.0f / 1642.0f, 71.0f / 879.0f, 1.11f);
	useitem[4] = SpriteSheet(youmu, 800.0f / 1642.0f, 791.0f / 879.0f, 91.0f / 1642.0f, 84.0f / 879.0f, 1.11f);
	useitem[5] = SpriteSheet(youmu, 622.0f / 1642.0f, 591.0f / 879.0f, 96.0f / 1642.0f, 83.0f / 879.0f, 1.11f);
	useitem[6] = SpriteSheet(youmu, 922.0f / 1642.0f, 464.0f / 879.0f, 94.0f / 1642.0f, 83.0f / 879.0f, 1.11f);
	useitem[7] = SpriteSheet(youmu, 210.0f / 1642.0f, 669.0f / 879.0f, 101.0f / 1642.0f, 82.0f / 879.0f, 1.11f);
	useitem[8] = SpriteSheet(youmu, 0.0f / 1642.0f, 732.0f / 879.0f, 104.0f / 1642.0f, 81.0f / 879.0f, 1.11f);
	useitem[9] = SpriteSheet(youmu, 625.0f / 1642.0f, 509.0f / 879.0f, 97.0f / 1642.0f, 80.0f / 879.0f, 1.11f);
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
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			done = true;
		}
	}
	// Render();
	//doBattle + encounter goes here.
	// Update(); //changes states and stages

	//testing 
	RenderMainMenu();
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
							Mix_FadeInMusic(titlemewsic, -1, 500);
	}
	case MAIN_QUEST:
	{
							RenderQuest();
							Mix_FadeInMusic(stagemewsic, -1, 1000);
	}
	case MAIN_DEATH:
	{
							RenderGameOver();
							Mix_FadeInMusic(gameovermewsic, -1, 300);
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
	DrawText(fontSprites, "Touhou Quest", .45f, -.22f);

	modelMatrix.identity();
	modelMatrix.Translate(-2.0f, 2.4f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Two Buttons To Play", .3f, -.189f);

	modelMatrix.identity();
	modelMatrix.Translate(-1.8f, 2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Z, X", .3f, -.189f);
	SDL_GL_SwapWindow(displayWindow);
}

void gameApp::RenderQuest()
{

}

void gameApp::RenderGameOver()
{
	
}

void gameApp::Update()
{

}

bool gameApp::isDone()
{
	return done;
}