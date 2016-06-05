#include "Entity.h"

#if defined(_WIN32)
#include <GL/glut.h>
#elif defined (__APPLE__)
#include <GLUT/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

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

Projectile::Projectile(Vec3Df spawnPoint, Vec3Df direction)
{
    this->spawnPoint = spawnPoint;
    movementDirection = direction;
}

// Overrides base method
void Projectile::draw()
{
    glColor3f(color[0], color[1], color[2]);
    glNormal3f(0, 0, 1);
    
    glPushMatrix();
    
    // 1. Translate projectile to its spawn point
    glTranslatef(spawnPoint[0], spawnPoint[1], spawnPoint[2]);
    // 2. Rotate projectile to face its heading
    float angle = atan2f(movementDirection[1], movementDirection[0]) * 180 / M_PI;
    glRotated(angle, 0, 0, 1.0);
    // 3. Translate 'forward' as the projectile travels
    glTranslatef(propelledDistance, 0, 0);
    
    glBegin(GL_QUADS);
    float offset = size/2.0f;
    glVertex3f(-offset, -offset, 0);
    glVertex3f(offset, -offset, 0);
    glVertex3f(offset, offset, 0);
    glVertex3f(-offset, offset, 0);
    glEnd();
    
    glPopMatrix();
}

// Overrides base method
void Projectile::animate(int deltaTime)
{
    propelledDistance += movementSpeed * ((float)deltaTime/1000);
    
    // In this case, we need to derive the new position separately
    movementDirection.normalize();
    position = spawnPoint + movementDirection * propelledDistance;
}
