#include "Character.h"
#include <GL/glut.h>


Character::Character()
{
}
Character::~Character()
{
}

void Character::draw()
{
	// Simple quad character
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

void Character::animate()
{
	movementDirection.normalize();
	position += movementDirection * movementSpeed;
}
