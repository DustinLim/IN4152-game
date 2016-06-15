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


#pragma region "Enemy"

Enemy::Enemy()
{
	// overwrite height and width;
	width = 0.7f;
	angle = 0.0f;
	up = 1;
	shake_speed = rand() % 3;
	elevate_speed = (rand() % 5) * 0.5;
	elevation = (rand() % 11) * 0.03;
	initTexture();
}

void Enemy::animate(int deltaTime)
{
	if (angle > 15)
		up = 0;
	if (angle < -15)
		up = 1;
	angle = (up == 1) ? angle + shake_speed : angle - shake_speed;

	movementDirection.normalize();
	position[1] -= elevation * cos(position[0] * M_PI * elevate_speed);
	position += movementDirection * movementSpeed * ((float)deltaTime / 1000);
	position[1] += elevation * cos(position[0] * M_PI * elevate_speed);
}

void Enemy::draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	
	glNormal3f(0, 0, 1);
	
	float offsetH = height / 2.0f;
	float offsetW = width / 2.0f;
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);		glVertex3f(-offsetW, -offsetH, 0);
		glTexCoord2f(1.0f, 1.0f);		glVertex3f(offsetW, -offsetH, 0);
		glTexCoord2f(1.0f, 0.0f);		glVertex3f(offsetW, offsetH, 0);
		glTexCoord2f(0.0f, 0.0f);		glVertex3f(-offsetW, offsetH, 0);
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void Enemy::initTexture()
{
	Texture.resize(1);

	Texture[0] = SOIL_load_OGL_texture(
		"./Textures/alien_1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT);
}

#pragma endregion

#pragma region "Projectile"

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

std::vector<Vec3Df> Projectile::getBoundingBox() {
	int temp = scale;
	scale = scale * 0.1f;
	Vec3Df topLeft = Vec3Df(position[0] - (width / 2.0f) * scale, position[1] - (height / 2.0f) * scale, position[2]);
	Vec3Df bottomRight = Vec3Df(position[0] + (width / 2.0f) * scale, position[1] + (height / 2.0f) * scale, position[2]);
	scale = temp;

	std::vector<Vec3Df> list = { topLeft, bottomRight };
	return list;
}

#pragma endregion

#pragma region "Character"
Character::Character()
{
	armAngle = 0.0f;
	turnAround = 0;
	width = 0.8f;			
	height = 2.0f;			
	scale = 0.4f;

	setShoulderPos();
	
	// Only change these values when you want to change individual proporties. Otherwise, use 'scale'
	armWidth = 0.25f;
	armLength = 0.9f;						// arm- and gunLength are related / coupled.
	gunLength = (0.7f / 0.9f) * armLength;
	gunHeight = 0.4f;
}

// Because we use the shoulder position at multiple points, it is defined in this method.
// Note that this position is a relative one!
void Character::setShoulderPos()
{
	shoulderPos = Vec3Df((-0.625f + (1.25f * turnAround)) * (width / 2.0f), 0.4f * (height / 2.0f), 0.0f);
}

// To calculate the correct arm angle and shooting direction, we want to approach the location of the gun
// Because the gun lies parallel with the arm, we update the x/y-value using the unit circle.
Vec3Df Character::getAngleRefPos()
{
	float anglePI = armAngle * M_PI / 180;
	float refScale = gunHeight * scale * 0.5f;			// with the last value we can tweak the 'height' of the reference point.

	Vec3Df refPos = position + shoulderPos * scale;
	refPos[0] += (turnAround == 0) ? -sin(anglePI) * refScale :  sin(anglePI) * refScale;
	refPos[1] += (turnAround == 0) ?  cos(anglePI) * refScale : -cos(anglePI) * refScale;

	return refPos;
}

float Character::getArmRadius()
{
	return scale * (armLength + gunLength * 0.5f);		// with the last value we can tweak the spawn point of a bullit
}

void Character::updateArmAngle(Vec3Df mousePos)
{
	Vec3Df direction = mousePos - getAngleRefPos();
	armAngle = atan2f(direction[1], direction[0]) * 180 / M_PI;

	float oldTurnAround = turnAround;
	turnAround = (armAngle > 90 || armAngle < -90) ? 1 : 0;
	if (oldTurnAround != turnAround)
		setShoulderPos();
}

void Character::animate(int deltaTime)
{
	movementDirection.normalize();
	position += movementDirection * movementSpeed * ((float)deltaTime / 1000);
	// update the armAngle!
}

void Character::draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[1]);

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glScalef(scale, scale, scale);
	
	glPushMatrix();
	
	// Start of the astronaut's body
	glColor4f(1.0, 1.0, 1.0, 1.0f);
	glNormal3f(0, 0, 1);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f + turnAround, 1.0f); 		glVertex3f(-width / 2.0f, -height / 2.0f, -0.1f);
		glTexCoord2f(1.0f - turnAround, 1.0f);		glVertex3f(width / 2.0f, -height / 2.0f, -0.1f);
		glTexCoord2f(1.0f - turnAround, 0.0f);		glVertex3f(width / 2.0f, height / 2.0f, -0.1f);
		glTexCoord2f(0.0f + turnAround, 0.0f);		glVertex3f(-width / 2.0f, height / 2.0f, -0.1f);
	glEnd();
	glPopMatrix();

	// Start of the astronaut's arm
	// To achieve a rotation point as if it is the upper shoulder, we decouple the forward and backward pose.
	glBindTexture(GL_TEXTURE_2D, Texture[0]);

	glPushMatrix();
	// put the arm 'turning-point' on the correct start position; position scales with the size of the character!
	glTranslatef(shoulderPos[0], shoulderPos[1], shoulderPos[2]);				
	glRotatef(armAngle, 0.0f, 0.0f, 1.0f);

	glPushMatrix();		// save this as the start configuration for the gun!
	
	if (turnAround == 0)
	{
		glRotatef(180, 0.0f, 0.0f, 1.0f);
	}
		
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);

	// Difference in drawing lies in whether we draw a rectangle upwards or downwards.
	if (turnAround == 0)
	{
		glTexCoord2f(1.0f, 0.0f);		glVertex3f(+0.05f - armLength, -0.1f, 0.01f);
		glTexCoord2f(0.0f, 0.0f);		glVertex3f(+0.05f, -0.1f, 0.01f);
		glTexCoord2f(0.0f, 1.0f);		glVertex3f(+0.05f, -0.1f + armWidth, 0.01f);
		glTexCoord2f(1.0f, 1.0f);		glVertex3f(+0.05f - armLength, -0.1f + armWidth, 0.01f);
	}
	else
	{
		glTexCoord2f(0.0f, 0.0f);		glVertex3f(-0.05f, -0.1f, 0.01f);
		glTexCoord2f(1.0f, 0.0f);		glVertex3f(-0.05f + armLength, -0.1f, 0.01f);
		glTexCoord2f(1.0f, 1.0f);		glVertex3f(-0.05f + armLength, -0.1f + armWidth, 0.01f);
		glTexCoord2f(0.0f, 1.0f);		glVertex3f(-0.05f, -0.1f + armWidth, 0.01f);
	}
	glEnd();
	glPopMatrix();

	// Start of the astronaut's gun.
	glBindTexture(GL_TEXTURE_2D, Texture[2]);

	// put the gun on the correct start position at the arm; scales with the size of the arm.
	glTranslatef(-0.05f + (0.555f * armLength), 0.1f - ((0.2f + gunHeight) * turnAround), 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f - turnAround);		glVertex3f(0.0f, 0.0f, 0.01f);
		glTexCoord2f(1.0f, 1.0f - turnAround);		glVertex3f(gunLength, 0.0f, 0.01f);
		glTexCoord2f(1.0f, 0.0f + turnAround);		glVertex3f(gunLength, gunHeight, 0.01f);
		glTexCoord2f(0.0f, 0.0f + turnAround);		glVertex3f(0.0f, gunHeight, 0.01f);
	glEnd();

	glPopMatrix();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	//DEBUG
	drawAngleRefPos();
}

// Debug purposes; draw a yellow sphere at the location of the reference position.
void Character::drawAngleRefPos()
{
	Vec3Df refPos = getAngleRefPos();
	glColor3f(1, 1, 0);
	glPushMatrix();
	glTranslatef(refPos[0], refPos[1], 0.0f);
	glutSolidSphere(0.1, 6, 6);
	glPopMatrix();
	glPopAttrib();
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

#pragma endregion
