#include "Entity.h"

#include "commonOpenGL.h"

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
	float offsetW = width / 2.0f;
	float offsetH = height / 2.0f;
	glVertex3f(-offsetW, -offsetH, 0);
	glVertex3f(offsetW, -offsetH, 0);
	glVertex3f(offsetW, offsetH, 0);
	glVertex3f(-offsetW, offsetH, 0);
	glEnd();
	glPopMatrix();
}

void Entity::animate(int deltaTime)
{
	movementDirection.normalize();
	position += movementDirection * movementSpeed * ((float)deltaTime/1000);
}

std::vector<Vec3Df> Entity::getBoundingBox() {
	Vec3Df topLeft = Vec3Df(position[0] - width / 2.0f, position[1] - height / 2.0f, position[2]);
	Vec3Df bottomRight = Vec3Df(position[0] + width / 2.0f, position[1] + height / 2.0f, position[2]);

	//float angle = atan2f(movementDirection[1], movementDirection[0]);
	//float y = sin(angle) * (width / 2.0f);
	//float x = sin(angle) * (width / 2.0f);
	std::vector<Vec3Df> list = { topLeft, bottomRight };
	return list;
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
    float offsetW = width/2.0f;
	float offsetH = height/ 2.0f;
    glVertex3f(-offsetW, -offsetH, 0);
    glVertex3f(offsetW, -offsetH, 0);
    glVertex3f(offsetW, offsetH, 0);
    glVertex3f(-offsetW, offsetH, 0);
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

Character::Character()
{
	armAngle = 0.0f;
}

void Character::draw()
{
	const float ref_mag = 0.1f;
	float tex_u_max = 1.0f;    
	float tex_v_max = 1.0f;


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[1]);

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glScalef(0.4f, 0.4f, 0.4f);
	
	glPushMatrix();
	
	// Start of the Astronaut's body
	glColor4f(1.0, 1.0, 1.0, 1.0f);
	glNormal3f(0, 0, 1);

	glBegin(GL_QUADS);
		glNormal3f(-ref_mag, -ref_mag, 1.0f);
		glTexCoord2f(0.0f, tex_v_max);
		glVertex3f(-0.4f, -1.0f, -0.1f);

		glNormal3f(ref_mag, -ref_mag, 1.0f);
		glTexCoord2f(tex_u_max, tex_v_max);
		glVertex3f(0.4f, -1.0f, -0.1f);

		glNormal3f(ref_mag, ref_mag, 1.0f);
		glTexCoord2f(tex_u_max, 0.0f);
		glVertex3f(0.4f, 1.0f, -0.1f);

		glNormal3f(-ref_mag, ref_mag, 1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-0.4f, 1.0f, -0.1f);
	glEnd();
	glPopMatrix();

	// Start of the Astronaut's arm
	glBindTexture(GL_TEXTURE_2D, Texture[0]);

	glPushMatrix();
	glTranslatef(-0.25f, 0.3f, 0.0f);
	glRotatef(armAngle, 0.0f, 0.0f, 1.0f);			// should be changed towards the angle of shooting.
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, tex_v_max);		glVertex3f(-0.05f, -0.1f, 0.01f);
		glTexCoord2f(tex_u_max, tex_v_max);	glVertex3f(1.05f, -0.1f, 0.01f);
		glTexCoord2f(tex_u_max, 0.0f);		glVertex3f(1.05f, 0.55f, 0.01f);
		glTexCoord2f(0.0f, 0.0f);			glVertex3f(-0.05f, 0.55f, 0.01f);
	glEnd();
	glPopMatrix();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void Character::animate(int deltaTime)
{
	Entity::animate(deltaTime);
}

void Character::updateArmAngle(Vec3Df direction)
{
	armAngle = atan2f(direction[1], direction[0]) * 180 / M_PI;
}

void Character::initTexture()
{
	Texture.resize(2);

	Texture[0] = SOIL_load_OGL_texture(
		"./Textures/astronaut-arm.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT);

	Texture[1] = SOIL_load_OGL_texture(
		"./Textures/astronaut-body.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT);
}
