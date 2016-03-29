#include "Entity.h"

float lerp(float v0, float v1, float t) { return (1.0f - t) * v0 + t * v1; }

Entity::Entity(SpriteSheet sprite) : sprite(sprite), x(0), y(0), width(0), height(0), xVelocity(0), yVelocity(0), xAcceleration(0), yAcceleration(0), collidedTop(false), collidedBottom(false), collidedLeft(false), collidedRight(false) {
}

void Entity::Update(float elapsed)
{
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	//Friction
	xVelocity = lerp(xVelocity, 0.0f, elapsed * 3.0f);	
	xVelocity += xAcceleration * elapsed;

	yVelocity = lerp(yVelocity, 0.0f, elapsed * 1.0f);	
	yVelocity += yAcceleration * elapsed;
	yVelocity += -3.0f * elapsed;
}

bool collidesWith(Entity *entity) { return true; }

void Entity::Draw(ShaderProgram *program) 
{
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0);

	program->setModelMatrix(modelMatrix);

	float vertices[] =  { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
						   width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2 
						};
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	sprite.Draw(program);

	glDisableVertexAttribArray(program->positionAttribute);
}