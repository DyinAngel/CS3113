#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#pragma once
#include "SpriteSheet.h"
#include <string>
#include <iostream>	
#include <vector>
enum ENTITYTYPE { PlayerEntity, EnemyEntity, BulletEntity, EnemyBulletEntity };

class Entity {
public:
	Entity(float x, float y, float h, float w, float vel, ENTITYTYPE type);
	float x;
	float y;
	ENTITYTYPE type;
	bool visible;
	float height;
	float width;
	float velocity;
	float direction;

	float xPosition();
	float yPosition();
	float getWidth();
	float getHeight();
	float getVelocity();
	float getDirection();

	bool isPlayer();
	bool isEnemy();
	bool isPBullet();
	bool isEBullet();

	bool hitwall(float distance);
	bool hitbottom();

	bool collidesWith(Entity* object);
	bool isVisible();

	void moveX(float x);
	void moveY(float y);
	void changeDirection();

	//for space game, delete here
	void dead();
	void stop();
	void go();
	void shoot();

	//end of space game stuff

	void Draw(ShaderProgram* program, Matrix& gameMatrix, SpriteSheet& sprite);

};