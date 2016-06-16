#pragma once
#include "commonOpenGL.h"
#include "Vec3D.h"
#include "argumentParser.h"
#include "SOIL.h"
#include <vector>

class Entity
{
public:
	Vec3Df position = Vec3Df(0, 0, 0);
	float scale = 1.0f;
    
    // Movement
	Vec3Df movementDirection = Vec3Df(0, 0, 0);
	float movementSpeed = 1; //units per second
    
    // Appearance
	float width = 0.5f;
	float height = 0.5f;
	Vec3Df color = Vec3Df(1, 1, 1);

	virtual void draw();
	virtual void animate(int deltaTime);
	virtual std::vector<Vec3Df> getBoundingBox();
	static GLuint shadowTexture;
};

class Enemy : public Entity
{
public:
	Enemy();

	// Contains one texture
	static std::vector<GLuint> textureSet;
	static const int ENEMY_TEXTURE_ID = 0;

	// Overriding
	void draw();
	void animate(int deltaTime);	// uses the base.

private:
	float angle;
	unsigned int up;					// determines motion direction (up/down)
	float shake_speed;					// determines rapidness of shaking motion (random over all enemies)
	float elevate_speed;				// determines rapidness of the up-downward movement (random over all enemies)
	float elevation;					// determines the amplitude of the up-downward movement
};

class Projectile: public Entity
{
public:
    Projectile(Vec3Df spawnPoint, Vec3Df direction);
    
    // Two textures *must* be included here
    static std::vector<GLuint> textureSet;
	static const int BLUE_TEXTURE_ID = 0;
	static const int RED_TEXTURE_ID = 1;
    
    // Overriding
    void draw();
    void animate(int deltaTime);
	std::vector<Vec3Df> getBoundingBox();

private:
    Vec3Df spawnPoint;
    float propelledDistance = 0;
    GLuint texture;
    int timeAccumulator = 0;
};

class Character : public Entity
{
public:
	Character();

	// Three textures are needed for the Character.
	static std::vector<GLuint> textureSet;
	static const int BODY_TEXTURE_ID = 0;
	static const int ARM_TEXTURE_ID = 1;
	static const int GUN_TEXTURE_ID = 2;

	void updateArmAngle(Vec3Df mousePos);
	void setShoulderPos();

	// Overriding
	void draw();
	void animate(int deltaTime);

	// Correct arm and bullets
	Vec3Df getAngleRefPos();
	void drawAngleRefPos();
	float getArmRadius();

private:
	Vec3Df shoulderPos;			// depends on 'turnAround'
	float armAngle;				// could be used to calculate angle for arm
	float turnAround;			// simulates boolean
	float gunAngle;				// used to adapt the armAngle to let the gun aim at the cursor.

	// Used to scale the arm / gun solely - only consequences for drawing.
	float drawAngle;
	float armLength;
	float armWidth;
	float gunLength;
	float gunHeight;
};