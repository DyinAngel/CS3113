#include "gameApp.h"
#include <SDL_mixer.h>

Mix_Chunk *laser;
Mix_Chunk *death;
Mix_Music *bg;
Mix_Music *gameover;

gameApp::gameApp() :Enumber(ENEMIES), timeLeftOver(0.0), EbulletIndex(0), bulletIndex(0), done(false), lastFrameTicks(0.0f), win(false) 
{
	Setup();
}

gameApp::~gameApp() 
{
	delete program;
	Mix_FreeChunk(laser);
	Mix_FreeChunk(death);
	Mix_FreeMusic(bg);
	Mix_FreeMusic(gameover);
	SDL_Quit();
}

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

void gameApp::Setup() 
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 640, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glUseProgram(program->programID);
	glViewport(0, 0, 640, 640);
	projectionMatrix.setOrthoProjection(-4.0f, 4.0f, -4.0f, 4.0f, -1.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);
	player = new Entity(0.0f, -3.0f, 0.3f, 0.2f, 500.0f, PlayerEntity);

	//loads music effects
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	laser = Mix_LoadWAV("lasershoot.wav");
	death = Mix_LoadWAV("death.wav");

	bg = Mix_LoadMUS("ZUN-8bit- Love Colored Master Spark.mp3");
	gameover = Mix_LoadMUS("Touhou Death Arrange14.mp3");
	// music under ZUN is freeware and can be used freely as long as no profit is made

	float enemyXPos = -3.5f;
	float enemyYPos = 3.0f;

	// spawn enemies
	for (int i = 0; i < ENEMIES; i++) 
	{
		enemies.push_back(new Entity(enemyXPos, enemyYPos, 0.2, 0.2, 1.0f, EnemyEntity));
		enemyXPos += 1.0f;
		if (((i + 1) % 7 == 0) && i != 0) 
		{
			enemyXPos = -3.5f;
			enemyYPos -= 0.6f;
		}
	}
	float bulletXPos = 0.0;
	float bulletYPos = -4.5;
	for (int i = 0; i < MAX_BULLETS; i++)
		bullet.push_back(new Entity(bulletXPos, bulletYPos, 0.2, 0.1, 0.0f, BulletEntity));
	float EbulletXPos = 0.0;
	float EbulletYPos = 4.5;
	for (int i = 0; i < MAX_BULLETS; i++)
		EnemyBullets.push_back(new Entity(bulletXPos, bulletYPos, 0.2, 0.1, -2.0f, BulletEntity));
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	spaceInvaderSprites = LoadTexture("sheet.png"); //file has to be placed to the h file
	fontSprites = LoadTexture("font1.png");
	playerImg = SpriteSheet(spaceInvaderSprites, 224.0f / 1024.0f, 832.0f / 1024.0f, 99.0f / 1024.0f, 75.0f / 1024.0f, 0.3f);
	EnemyImg = SpriteSheet(spaceInvaderSprites, 423.0f / 1024.0f, 728.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.4f);
	bulletImg = SpriteSheet(spaceInvaderSprites, 856.0f / 1024.0f, 421.0f / 1024.0f, 9.0f / 1024.0f, 54.0f / 1024.0f, 0.1f);
	bulletImg2 = SpriteSheet(spaceInvaderSprites, 740.0f / 1024.0f, 686.0f / 1024.0f, 37.0f / 1024.0f, 38.0f / 1024.0f, 0.1f);
	state = STATE_MAIN_MENU;
	Render();
	SDL_GL_SwapWindow(displayWindow);



}

void gameApp::shootBullets(float x, float y)
{
	bullet[bulletIndex]->isVisible() == true;
	bullet[bulletIndex]->x = x;
	bullet[bulletIndex]->y = y;
	bullet[bulletIndex]->go();
	bulletIndex++;
	if (bulletIndex > MAX_BULLETS - 1){
		bulletIndex = 0;
	}
	Mix_PlayChannel(-1, laser, 0);
}

void gameApp::EshootBullets(float x, float y)
{
	EnemyBullets[EbulletIndex]->isVisible() == true;
	EnemyBullets[EbulletIndex]->x = x;
	EnemyBullets[EbulletIndex]->y = y;
	EnemyBullets[EbulletIndex]->shoot();
	EbulletIndex++;
	if (EbulletIndex > MAX_BULLETS - 1){ EbulletIndex = 0;}
}

void gameApp::Render() 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) 
	{
	case STATE_MAIN_MENU:
	{
							RenderMainMenu();
							Mix_PlayMusic(bg, - 1);
							break;
	}
	case STATE_GAME_LEVEL: 
	{
							   RenderGameLevel();
							   break;
	}
	case STATE_GAME_OVER: 
	{
							  Mix_PlayMusic(gameover, -1);
							  RenderGameOver();
							  break;
	}
	}
}

void gameApp::RenderMainMenu() 
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5f, 2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "This is", .8f, -0.4f);

	modelMatrix.identity();
	modelMatrix.Translate(-3.3f, 0.4f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "SPACE INVADERS!", 1.0f, -0.6f);

	modelMatrix.identity();
	modelMatrix.Translate(-0.2f, -3.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Press space to start", 0.5f, -0.3);
}

void gameApp::RenderGameLevel()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	player->Draw(program, modelMatrix, playerImg);
	//draw enemies
	for (int i = 0; i < enemies.size(); i++) 
	{
		if (enemies[i]->isVisible() == true) 
		{
			enemies[i]->Draw(program, modelMatrix, EnemyImg);
		}
	}
	//draw bullets
	for (int i = 0; i < bullet.size(); i++)
	{
		if (bullet[i]->isVisible()){
			bullet[i]->Draw(program, modelMatrix, bulletImg2);
		}
	}
	//draw enemy bullets
	for (int i = 0; i < EnemyBullets.size(); i++)
	{
		if (EnemyBullets[i]->isVisible())
		{
			EnemyBullets[i]->Draw(program, modelMatrix, bulletImg);
		}
	}
	SDL_GL_SwapWindow(displayWindow);
}

void gameApp::RenderGameOver() 
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix.identity();
	modelMatrix.Translate(-2.5f, 1.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Commander", 1.1f, -0.6f);

	if (win == true) 
	{
		modelMatrix.identity();
		modelMatrix.Translate(-3.4f, 0.35f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "Well Done.", 1.0f, -0.38f);
	}
	else 
	{
		modelMatrix.identity();
		modelMatrix.Translate(-2.0f, 0.0f, 0.0f);
		program->setModelMatrix(modelMatrix);
		DrawText(fontSprites, "You failed us.", 0.6f, -0.4f);
	}

	modelMatrix.identity();
	modelMatrix.Translate(-0.2f, -3.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(fontSprites, "Press esc to close", 0.5f, -0.3);
	SDL_GL_SwapWindow(displayWindow);
}



void gameApp::ProcessInput(float elasped) 
{
	while (SDL_PollEvent(&event)) 
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) 
		{
			done = true;
		}
		if (state == STATE_MAIN_MENU) 
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
			{
				state = STATE_GAME_LEVEL;
			}
		}
		if (state == STATE_GAME_LEVEL)
		{
			if (event.type == SDL_KEYDOWN) 
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) 
				{
					if (player->xPosition() >= -1 * MAXXPOS)
					{
						player->moveX(-0.4);
					}
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) 
				{
					if (player->yPosition() <= MAXXPOS)
					{
						player->moveX(0.4);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) 
				{
					shootBullets(player->x, player->y);
				}
			}
		}
		if (state == STATE_GAME_OVER) 
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) 
			{
				done = true;
			}
		}
	}
}

bool gameApp::UpdateAndRender() 
{
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elasped = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	float fixedElasped = elasped + timeLeftOver;

	Update(elasped);
	ProcessInput(elasped);
	Render();

	return done;
}

bool gameApp::CheckWall()
{
	for (int j = 0; j < enemies.size(); j++)
	{
		if (enemies[j]->hitwall(-0.2))
		{
			if (enemies[j]->isVisible() == true)
			{
				return true;
			}
		}
	}
}

float gameApp::randNum()
{
	return floor(rand() % ENEMIES);
}

void gameApp::Update(float elasped) 
{
	if (state == STATE_GAME_LEVEL)
	{
		if (gameOver() == true) 
		{
			state = STATE_GAME_OVER;
		}
		EnemyBullet += elasped;
		if (EnemyBullet > 0.2f) 
		{
			int index = randNum();
			EnemyBullet = 0.0f;
			if (enemies[index]->isVisible())
			{
				EshootBullets(enemies[index]->x, enemies[index]->y);
			}
			else
			{
				for (int i = 0; i < enemies.size(); i++)
				{
					if (enemies[i]->isVisible())
					{
						EshootBullets(enemies[index]->x, enemies[index]->y);
					}
				}
			}
		}
		for (int i = 0; i < EnemyBullets.size(); i++) 
		{
			if (EnemyBullets[i]->isVisible()) 
			{
				EnemyBullets[i]->moveY(EnemyBullets[i]->getVelocity() * elasped);
				if (EnemyBullets[i]->yPosition() < -1 * MAXYPOS) 
				{
					EnemyBullets[i]->stop();
				}
				else if (EnemyBullets[i]->collidesWith(player)) 
				{
					EnemyBullets[i]->stop();
					player->dead();
					Mix_PlayChannel(-1, death, 0);
					state = STATE_GAME_OVER;
					break;
				}
			}
		}
		for (int i = 0; i < bullet.size(); i++) 
		{
			if (bullet[i]->isVisible()) 
			{
				bullet[i]->moveY(bullet[i]->getVelocity() * elasped);
				if (bullet[i]->yPosition() > MAXYPOS)
				{
					bullet[i]->stop();
				}
				for (int j = 0; j < enemies.size(); j++) 
				{
					if (enemies[j]->isVisible() && bullet[i]->collidesWith(enemies[j])) 
					{
						enemies[j]->dead();
						Mix_PlayChannel(-1, death, 0);
						bullet[i]->stop();
						--Enumber;
						if (Enumber == 0)
							win = true;
						break;
					}
				}
			}
		}
		if (CheckWall() == true)
		{
			for (int i = 0; i < enemies.size(); i++) 
			{
				enemies[i]->changeDirection();
				enemies[i]->moveX(enemies[i]->getDirection() * enemies[i]->getVelocity() * elasped + (enemies[i]->getDirection() * 0.1));
				enemies[i]->moveY(-0.3f);
				if (player->collidesWith(enemies[i])) 
				{
					state = STATE_GAME_OVER;
					break;
				}
			}
		}
		else {
			for (int i = 0; i < enemies.size(); i++) 
			{
				enemies[i]->moveX(enemies[i]->getDirection() * enemies[i]->getVelocity() * elasped);
				if (player->collidesWith(enemies[i])) 
				{
					state = STATE_GAME_OVER;
					break;
				}
			}
		}
	}
}

bool gameApp::gameOver()
{
	return Enumber == 0 || !player->isVisible();
}