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

	glTexImage2D //debug
		(
		GL_TEXTURE_2D, //ok
		0,	//ok
		GL_RGBA, //ok
		surface->w, 
		surface->h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE, 
		surface->pixels //NOTE: Custom images need to be saved and edited into 32bit format
		);

	SDL_FreeSurface(surface);
	return textureID;
}


int main(int argc, char *argv[])
{
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	#ifdef _WINDOWS
		glewInit();
	#endif

	SDL_Event event;
	bool done = false;
	
	glViewport(0, 0, 640, 360);

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;

	float lastFrameTicks = 0.0f;
	float angle = 0.0;

	GLuint bg = LoadTexture("C:\\Users\\Hua Yuan Yu\\CS3113\\project_template\\Windows\\NYUCodebase\\bg.png");
	GLuint praise = LoadTexture("C:\\Users\\Hua Yuan Yu\\CS3113\\project_template\\Windows\\NYUCodebase\\praise.png");
	GLuint sun = LoadTexture("C:\\Users\\Hua Yuan Yu\\CS3113\\project_template\\Windows\\NYUCodebase\\sun.png");
	GLuint tex[3] = { bg, praise, sun };

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	while (!done) 
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) 
			{
				done = true;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT || GL_DEPTH_BUFFER_BIT);

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		angle += elapsed;
		glEnable(GL_BLEND); // how does one blend seperate sides of the screen differently??
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		float vertices[] = 
		{  
			-1.5f, -1.5f, 1.5f, -1.5f, 1.5f, 1.5f, -1.5f, -1.5f, 1.5f, 1.5f, -1.5f, 1.5f,
			-3.5f, -1.0f, -1.5f, -1.0f, -1.5f, 1.0f, -3.5f, -1.0f, -1.5f, 1.0, -3.5f, 1.0f,
			1.5f, -1.0f, 3.5f, -1.0f, 3.5f, 1.0f, 1.5f, -1.0f, 3.5f, 1.0f, 1.5f, 1.0f
		};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords[] = 
		{ 
			0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0
		};
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, tex[1]);
		glDrawArrays(GL_TRIANGLES, 6, 6);
		
		glBindTexture(GL_TEXTURE_2D, tex[2]);
		modelMatrix.Rotate(angle);
		program.setModelMatrix(modelMatrix);
		modelMatrix.identity();
		glDrawArrays(GL_TRIANGLES, 12, 6);


		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}

