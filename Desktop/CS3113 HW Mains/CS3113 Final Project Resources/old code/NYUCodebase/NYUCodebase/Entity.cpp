//Documentation
#include "Entity.h"

Entity::Entity(float xPosition, float yPosition, float h, float w, float vel, ENTITYTYPE types)
{
	x = xPosition;
	y = yPosition;
	height = h;
	width = w;
	type = types;
	velocity = vel;
	visible = true;
	direction = 1.0f;
}

bool Entity::hitwall(float distance) { return (x + distance < -1 * 4.0) || (x - distance > 4.0f); }
// left,right
bool Entity::hitbottom() { return y < -2.9; }
// bottom

bool Entity::collidesWith(Entity* object)
{
	return !(((y + (height / 2)) <= (object->yPosition() - (object->getHeight() / 2)))
		|| ((y - (height / 2)) >= (object->yPosition() + (object->getHeight() / 2)))
		|| (((x + (width / 2)) <= (object->xPosition() - (object->getWidth() / 2)))
		|| ((x - (width / 2)) >= (object->xPosition() + (object->getWidth() / 2)))));
}

void Entity::moveX(float plusx) { x += plusx; }
void Entity::moveY(float plusy) { y += plusy; }
void Entity::changeDirection() { direction *= -1;}
void Entity::dead() { visible = false; }



void Entity::Draw(ShaderProgram* program, Matrix& gameMatrix, SpriteSheet& sprite)
{
	sprite.Draw(program, gameMatrix, x, y);
}

float Entity::xPosition(){
	return x;
}
float Entity::yPosition(){
	return y;
}
float Entity::getWidth(){
	return width;
}
float Entity::getHeight(){
	return height;
}
float Entity::getVelocity(){
	return velocity;
}
float Entity::getDirection(){
	return direction;
}
bool Entity::isVisible(){
	return visible;
}
