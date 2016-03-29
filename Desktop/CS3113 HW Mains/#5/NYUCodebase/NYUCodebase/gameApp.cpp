#include "gameApp.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEP 6

GLuint LoadTexture(const char *image_path)
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

/*
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
*/

void worldToTileCoordinates(float worldX, float worldY, int &gridX, int &gridY)
{
	gridX = (int)(worldX / TILE_SIZE);
	gridY = (int)(worldY / TILE_SIZE);
}

gameApp::gameApp() { Setup(); }
gameApp::~gameApp() 
{
	delete program;
	SDL_Quit();
}

void gameApp::Setup() 
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("The Martian", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _WINDOWS
	glewInit();
#endif
	done = false;

	srand((unsigned int)time(NULL));

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	tileTexture = LoadTexture("sheet_4.png");
	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	//generate world

	//up down chunks and border
	for (int i = 0; i < LEVEL_HEIGHT; i++) 
	{
		for (int j = 0; j < LEVEL_WIDTH; j++) 
		{
			if (i == 0 || j == 0 || j == LEVEL_WIDTH - 1) { levelData[i][j] = 13; }
			else { levelData[i][j] = 254; }
		}
	}

	// left right platforms and chunk
	for (int k = 2; k < LEVEL_WIDTH - 5;) 
	{
		for (int r = rand() % 3; r < 3; r++) 
		{
			int j = rand() % (LEVEL_HEIGHT - 4) + 4;
			for (int i = rand() % 4; i < 6; i++) 
			{
				levelData[j][k + i] = 13;
				if (i == 4 && (k + i) > LEVEL_WIDTH - 11) 
				{
					levelData[j - 1][k + i] = 113; //plant
					r = 3;
				}
			}
		}
		k += 7 + rand() % 3;
	}

	SpriteSheet playerSprite = SpriteSheet((LoadTexture("p1_front.png")), 1, 1);
	player = new Entity(playerSprite);
	entities.push_back(player);
	player->width = 0.225f;
	player->height = 0.3f;
	player->x = TILE_SIZE * 6 + player->width / 2;
	player->y = -TILE_SIZE * 2;
	player->isStatic = false;
	xBase = 6;
}

void gameApp::UpdateAndRender() 
{
	while (SDL_PollEvent(&event)) 
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastElapsedTime;
	lastElapsedTime = ticks;

	float fixedElapsed = elapsed;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEP)
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEP;
	while (fixedElapsed > FIXED_TIMESTEP) 
	{
		fixedElapsed -= FIXED_TIMESTEP;
		Update(FIXED_TIMESTEP);
	}
	Update(fixedElapsed);
	Render();
}

void gameApp::Update(float elapsed) 
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	//Player controls
	if (keys[SDL_SCANCODE_UP] && player->collidedBottom) 
	{
		player->yVelocity = 6.2f;
		player->collidedBottom = false;
	}
	if (keys[SDL_SCANCODE_LEFT]) 
	{
		player->xAcceleration = -6.2f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) 
	{
		player->xAcceleration = 6.2f;
	}
	else 
	{
		player->xAcceleration = 0;
	}

	//update nonstatic entities (only player 4nw)
	for (size_t i = 0; i < entities.size(); i++)
	if (!entities[i]->isStatic) 
	{

		//accel + vels
		entities[i]->Update(elapsed);

		int gridX = 0, gridY = 0;

		//y collision
		entities[i]->y += entities[i]->yVelocity * elapsed;
		worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, gridX, gridY);
		if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13)
		{
			entities[i]->y = gridY * TILE_SIZE + entities[i]->height / 2 + 0.000001f;
			entities[i]->yVelocity = 0;
			entities[i]->collidedBottom = true;
			xBase = gridX;
		}
		worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, gridX, gridY);
		if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13) 
		{
			entities[i]->y = (gridY - 1) * TILE_SIZE - entities[i]->height / 2 - 0.000001f;
			entities[i]->yVelocity = 0;
			entities[i]->collidedTop = true;
		}

		//x collision
		entities[i]->x += entities[i]->xVelocity * elapsed;
		worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, gridX, gridY);
		if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13) 
		{
			entities[i]->x = (gridX + 1) * TILE_SIZE + entities[i]->width / 2 + 0.000001f;
			entities[i]->xVelocity = 0;
			entities[i]->collidedLeft = true;
		}
		worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, gridX, gridY);
		if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13) 
		{
			entities[i]->x = gridX * TILE_SIZE - entities[i]->width / 2 - 0.000001f;
			entities[i]->xVelocity = 0;
			entities[i]->collidedRight = true;
		}

		if (entities[i]->y < TILE_SIZE * -LEVEL_HEIGHT) 
		{
			entities[i]->y = TILE_SIZE * -2;
			entities[i]->x = TILE_SIZE * xBase + entities[i]->width / 2;
			entities[i]->yVelocity = 0;
		}
	}
}

void gameApp::Render() 
{
	glClear(GL_COLOR_BUFFER_BIT);

	viewMatrix.identity();
	viewMatrix.Translate(-player->x, -player->y, 0);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	int drawCounter = 0;
	for (int y = 0; y < LEVEL_HEIGHT; y++) 
	{
		for (int x = 0; x < LEVEL_WIDTH; x++) 
		{
			if (levelData[y][x] != 254)
			{
				float u = (float)(((int)levelData[y][x]) % 14) / (float)14;
				float v = (float)(((int)levelData[y][x]) / 14) / (float)8;

				float spriteWidth = 1.0f / (float)14;
				float spriteHeight = 1.0f / (float)8;

				vertexData.insert(vertexData.end(), 
				{
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),

					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * y
				});

				texCoordData.insert(texCoordData.end(), 
				{
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,

					u, v,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
			else
				drawCounter++;
		}
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, tileTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * (LEVEL_HEIGHT * LEVEL_WIDTH - drawCounter));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

	for (size_t i = 0; i < entities.size(); i++)
		entities[i]->Draw(program);

	SDL_GL_SwapWindow(displayWindow);
}

bool gameApp::isDone() { return done; }