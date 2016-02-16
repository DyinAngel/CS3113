// Hua Yuan Yu CS3113
/*
	Notes:
	Error when starting the game that causes the ball to vanish?
*/


#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);
	glEnable(GL_TEXTURE_2D);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,  surface->pixels );
	//NOTE: Custom images need to be saved and edited into 32bit format
	SDL_FreeSurface(surface);
	return textureID;
}

class Ball
{
public:

	float xCoord = 0.0f;
	float yCoord = 0.0f;
	float speed = 0.35f;
	float xMovement = (float)rand();
	float yMovement = (float)rand();

	Ball() {}
	Ball(float xPixel, float yPixel, float moveSpeed, float xDir, float yDir ) : xCoord(xPixel), yCoord(yPixel), speed(moveSpeed), xMovement(xDir), yMovement(yDir) {}
	void centerBall()
	{
		float xCoord = 0.0f;
		float yCoord = 0.0f;
		float speed = 0.1f;
		float xMovement = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float yMovement = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}
	void move()
	{
		xCoord += (speed* xMovement);
		yCoord += (speed* yMovement);
	}
	void speedUp()
	{
		speed += (0.1f*speed);
	}

};

class Paddle
{
public:
	float left;
	float right;
	float up;
	float down;

	Paddle(float l, float r, float u, float d) : left(l), right(r), up(u), down(d) {}
};

ShaderProgram setup()
{
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Ping Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 360);
	return ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
}

int main(int argc, char *argv[])
{
	ShaderProgram program = setup();

	Matrix leftPaddle;
	Paddle leftPad(-2.5f, -2.5f, 0.5f, -0.5f);

	Matrix rightPaddle;
	Paddle rightPad(2.5f, 2.5f, 0.5f, -0.5f);

	Matrix ball;
	Ball theBall = Ball(0.0f, 0.0f, 0.2f, (float)rand(), (float)rand());

	Matrix projectionMatrix;
	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	Matrix modelMatrix;
	Matrix viewMatrix;

	GLuint block = LoadTexture("C:\\Users\\Hua Yuan Yu\\CS3113\\project_template\\Windows\\HW#2\\text.png"); //cannot seem to access with just text.png

	SDL_Event event;
	bool done = false;
	bool gameStart = false;
	glUseProgram(program.programID);
	
	float lastFrameTicks = 0.0f;

	while (!done)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		std::cout << theBall.xCoord;
		std::cout << theBall.yCoord;

		//paddle movement ok
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
				done = true;
			if (event.type == SDL_KEYDOWN)
			{
				//left pad controls
				if (event.key.keysym.scancode == SDL_SCANCODE_W)
				{
					if (leftPad.up < 2.0f)
					{
						leftPad.up += 0.2f;
						leftPad.down += 0.2f;
						leftPaddle.Translate(0.0f, 0.2f, 0.0f);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S)
				{
					if (leftPad.down > -2.0f)
					{
						leftPad.up -= 0.2f;
						leftPad.down -= 0.2f;
						leftPaddle.Translate(0.0f, -0.2f, 0.0f);
					}
				}
				//right paddle controls
				if (event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					if (rightPad.up < 2.0f)
					{
						rightPad.up += 0.2f;
						rightPad.down += 0.2f;
						rightPaddle.Translate(0.0f, 0.2f, 0.0f);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				{
					if (rightPad.down > -2.0f)
					{
						rightPad.up -= 0.2f;
						rightPad.down -= 0.2f;
						rightPaddle.Translate(0.0f, -0.2f, 0.0f);
					}
				}
				//gamestart
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
				{
					if (!gameStart)
						gameStart = true;
				}
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //black screen
		glClear(GL_COLOR_BUFFER_BIT);

		program.setModelMatrix(leftPaddle);
		program.setViewMatrix(viewMatrix);
		program.setProjectionMatrix(projectionMatrix);

		glUseProgram(program.programID);

		float textCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };

		//left
		float leftPadCoords[] = { -3.5f, -0.5f, -3.4f, -0.5f, -3.4f, 0.5f, -3.4f, 0.5f,  -3.5f, 0.5f, -3.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, leftPadCoords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, block);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//right
		program.setModelMatrix(rightPaddle);
		float rightPadCoords[] = { 3.4f, -0.5f, 3.5f, -0.5f, 3.5f, 0.5f, 3.5f, 0.5f, 3.4f, 0.5f, 3.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, rightPadCoords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, block);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		//ball
		program.setModelMatrix(ball);
		float ballCoords[] = { -0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f, 0.1f, -0.1f, -0.1f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballCoords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, block);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		if (gameStart)
		{
			if (theBall.xCoord - 0.1f <= leftPad.left)
			{

				gameStart = false;
				ball.Translate(-theBall.xCoord, -theBall.yCoord, 0.0f);
				theBall.centerBall();
				printf("Left Player has won the game! \n");
			}
			if (theBall.xCoord + 0.1f >= rightPad.right)
			{
				gameStart = false;
				ball.Translate(-theBall.xCoord, -theBall.yCoord, 0.0f);
				theBall.centerBall();
				printf("Right Player has won the game! \n");
			}
			else if (theBall.yCoord + 0.1f >= 2.0f || theBall.yCoord - 0.1f <= -2.0f)
			{
				theBall.xMovement *= -1;
				theBall.speedUp();
				theBall.move();
				ball.Translate(theBall.speed * theBall.xMovement, theBall.speed * theBall.yMovement, 0.0f);
			}
			else if ( (theBall.xCoord - 0.1f <= leftPad.left && theBall.yCoord - 0.1f <= leftPad.down && theBall.yCoord + 0.1f >= leftPad.up) ||
				(theBall.xCoord + 0.1f >= rightPad.right && theBall.yCoord - 0.1f <= rightPad.down && theBall.yCoord + 0.1f >= leftPad.up))
			{
				theBall.xCoord *= -1;
				theBall.move();
				ball.Translate((theBall.speed * theBall.xMovement), (theBall.speed *theBall.yMovement), 0.0f);
			}

			else
			{
				theBall.move();
				ball.Translate((theBall.speed * theBall.xMovement), (theBall.speed * theBall.yMovement), 0.0f);
			}
		}
		SDL_GL_SwapWindow(displayWindow);
	}
	

	SDL_Quit();
	return 0;
}

