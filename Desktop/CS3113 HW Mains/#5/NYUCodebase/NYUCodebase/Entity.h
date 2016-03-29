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

enum EntityType { ENTITY_PLAYER, ENTITY_PLATFORM };

class Entity
{
public:
	Entity(SpriteSheet sprite);

	SpriteSheet sprite;

	float x, y;
	float width, height;
	// float velocity, direction;
	float xVelocity, yVelocity;
	float xAcceleration, yAcceleration;

	EntityType entityType;
	bool isStatic;

	bool collidedTop, collidedBottom, collidedLeft, collidedRight;

	void Update(float elapsed);
	void Draw(ShaderProgram *program);
	bool collidesWith(Entity *entity);

};