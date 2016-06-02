#include "Entity.h"
#include <GL/glut.h>


Entity::Entity()
{
}
Entity::~Entity()
{
}

void Entity::draw()
{
	// Simple quad Entity
	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glColor3f(color[0], color[1], color[2]);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	float offset = size/2.0f;
	glVertex3f(-offset, -offset, 0);
	glVertex3f(offset, -offset, 0);
	glVertex3f(offset, offset, 0);
	glVertex3f(-offset, offset, 0);
	glEnd();
	glPopMatrix();
}

void Entity::animate(int deltaTime)
{
	movementDirection.normalize();
	position += movementDirection * movementSpeed * ((float)deltaTime/1000);
}
