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
	Vec3Df topLeft = Vec3Df(position[0] - (width / 2.0f) * scale, position[1] - (height / 2.0f) * scale, position[2]);
	Vec3Df bottomRight = Vec3Df(position[0] + (width / 2.0f) * scale, position[1] + (height / 2.0f) * scale, position[2]);

	//float angle = atan2f(movementDirection[1], movementDirection[0]);
	//float y = sin(angle) * (width / 2.0f);
	//float x = sin(angle) * (width / 2.0f);
	std::vector<Vec3Df> list = { topLeft, bottomRight };
	return list;
}

std::vector<GLuint> Projectile::textureSet;

Projectile::Projectile(Vec3Df spawnPoint, Vec3Df direction)
{
    this->spawnPoint = spawnPoint;
    movementDirection = direction;
    texture = textureSet[0];
}

// Overrides base method
void Projectile::draw()
{
    glColor3f(1,1,1);
    glNormal3f(0, 0, 1);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    // 1. Translate projectile to its spawn point
    glTranslatef(spawnPoint[0], spawnPoint[1], spawnPoint[2]);
    // 2. Rotate projectile to face its heading
    float angle = atan2f(movementDirection[1], movementDirection[0]) * 180 / M_PI;
    glRotated(angle, 0, 0, 1.0);
    // 3. Translate 'forward' as the projectile travels
    glTranslatef(propelledDistance, 0, 0);
    
    glBegin(GL_QUADS);
    /*
        float offset = size/2.0f;
        glTexCoord2f(0,1);
        glVertex3f(-offset, -offset, 0);
        glTexCoord2f(1,1);
        glVertex3f(offset, -offset, 0);
        glTexCoord2f(1,0);
        glVertex3f(offset, offset, 0);
        glTexCoord2f(0,0);
        glVertex3f(-offset, offset, 0);
    */
        float offsetW = width/2.0f;
        float offsetH = height/ 2.0f;
        glTexCoord2f(0,1);
        glVertex3f(-offsetW, -offsetH, 0);
        glTexCoord2f(1,1);
        glVertex3f(offsetW, -offsetH, 0);
        glTexCoord2f(1,0);
        glVertex3f(offsetW, offsetH, 0);
        glTexCoord2f(0,0);
        glVertex3f(-offsetW, offsetH, 0);
    glEnd();
    
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

// Overrides base method
void Projectile::animate(int deltaTime)
{
    // Alternate between textures
    timeAccumulator += deltaTime;
    if (timeAccumulator >= 200)
    {
        texture = (texture == textureSet[0]) ? textureSet[1] : textureSet[0];
        timeAccumulator = 0;
    }
    
    propelledDistance += movementSpeed * ((float)deltaTime/1000);
    
    // In this case, we need to derive the new position separately
    movementDirection.normalize();
    position = spawnPoint + movementDirection * propelledDistance;
}

Character::Character()
{
	armAngle = 0.0f;
	turnAround = 0;
	width = 0.8f;			// <- Not that if this changes, the arm + gun is not correct anymore, due to hard-coding
	height = 2.0f;			// <- Not that if this changes, the arm + gun is not correct anymore, due to hard-coding
	scale = 0.4f;
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
	glScalef(scale, scale, scale);
	
	glPushMatrix();
	
	// Start of the Astronaut's body
	glColor4f(1.0, 1.0, 1.0, 1.0f);
	glNormal3f(0, 0, 1);

	glBegin(GL_QUADS);
		glNormal3f(-ref_mag, -ref_mag, 1.0f);
		glTexCoord2f(0.0f + turnAround, 1.0f);
		glVertex3f(-width / 2.0f, -height / 2.0f, -0.1f);

		glNormal3f(ref_mag, -ref_mag, 1.0f);
		glTexCoord2f(1.0f - turnAround, 1.0f);
		glVertex3f(width / 2.0f, -height / 2.0f, -0.1f);

		glNormal3f(ref_mag, ref_mag, 1.0f);
		glTexCoord2f(1.0f - turnAround, 0.0f);
		glVertex3f(width / 2.0f, height / 2.0f, -0.1f);

		glNormal3f(-ref_mag, ref_mag, 1.0f);
		glTexCoord2f(0.0f + turnAround, 0.0f);
		glVertex3f(-width / 2.0f, height / 2.0f, -0.1f);
	glEnd();
	glPopMatrix();

	// Start of the Astronaut's arm
	glBindTexture(GL_TEXTURE_2D, Texture[0]);

	glPushMatrix();
	// put the arm on the correct start position.
	glTranslatef(-0.25f + (0.5f * turnAround), 0.3f + (0.05f * turnAround), 0.0f);				
	glRotatef(armAngle, 0.0f, 0.0f, 1.0f);
	
	glPushMatrix();		// save configuration for the gun!
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f - turnAround);		glVertex3f(-0.05f, -0.1f, 0.01f);
		glTexCoord2f(1.0f, 1.0f - turnAround);		glVertex3f(0.85f, -0.1f, 0.01f);
		glTexCoord2f(1.0f, 0.0f + turnAround);		glVertex3f(0.85f, 0.15f, 0.01f);
		glTexCoord2f(0.0f, 0.0f + turnAround);		glVertex3f(-0.05f, 0.15f, 0.01f);
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, Texture[2]);

	// put the gun on the correct start position
	glTranslatef(0.7f, 0.12f - (0.59f * turnAround), 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f - turnAround);		glVertex3f(-.25f, 0.0f, 0.01f);
	glTexCoord2f(1.0f, 1.0f - turnAround);		glVertex3f(0.45f, 0.0f, 0.01f);
	glTexCoord2f(1.0f, 0.0f + turnAround);		glVertex3f(0.45f, 0.4f, 0.01f);
	glTexCoord2f(0.0f, 0.0f + turnAround);		glVertex3f(-.25f, 0.4f, 0.01f);
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
	turnAround = (armAngle > 90 || armAngle < -90) ? 1 : 0;
}

void Character::initTexture()
{
	Texture.resize(3);

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

	Texture[2] = SOIL_load_OGL_texture(
		"./Textures/astronaut-gun.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT);
}
