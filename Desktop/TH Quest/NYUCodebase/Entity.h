#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "SpriteSheet.h"
#include <math.h>

enum EntityType { ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_EVENT };

class Entity
{
public:
	Entity(SpriteSheet sprite);

	SpriteSheet sprite;

	float x, y;
	float width, height;

	EntityType entityType;

	void Draw(ShaderProgram *program);
	bool collidesWith(Entity *entity);
	void fixCollision(Entity *entity);
	float xPosition();
	float yPosition();
	float getWidth();
	float getHeight();
};