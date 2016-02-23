#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "gameApp.h"

int main(int argc, char *argv[])
{
	gameApp spaceInvaders;
	while (!spaceInvaders.UpdateAndRender()) {}
	return 0;
}