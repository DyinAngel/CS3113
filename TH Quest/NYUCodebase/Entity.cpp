#include "Entity.h"

float lerp(float v0, float v1, float t) { return (1.0f - t) * v0 + t * v1; }

Entity::Entity(SpriteSheet sprite) : sprite(sprite), x(0), y(0), width(0), height(0){}
Entity::Entity() {}

bool Entity::collidesWith(Entity* object)
{
	return !(((y + (height / 2)) <= (object->yPosition() - (object->getHeight() / 2)))
		|| ((y - (height / 2)) >= (object->yPosition() + (object->getHeight() / 2)))
		|| (((x + (width / 2)) <= (object->xPosition() - (object->getWidth() / 2)))
		|| ((x - (width / 2)) >= (object->xPosition() + (object->getWidth() / 2)))));
}

void Entity::fixCollision(Entity* object)
{
	int collided = collidesWith(object);
	if (collided)
	{
		x - 40;
		object->x = object->x + 40;
	}
}

void Entity::Draw(ShaderProgram *program)
{
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0);

	program->setModelMatrix(modelMatrix);

	float vertices[] = { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
		width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2
	};
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	sprite.Draw(program);

	glDisableVertexAttribArray(program->positionAttribute);
}

float Entity::xPosition(){return x;}
float Entity::yPosition(){return y;}
float Entity::getWidth(){return width;}
float Entity::getHeight(){ return height; }
