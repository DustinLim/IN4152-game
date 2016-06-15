#pragma once
#include "commonOpenGL.h"
#include "Vec3D.h"
#include "argumentParser.h"
#include "SOIL.h"
#include <vector>

class Entity
{
public:
	Entity();
	~Entity(); //class destructor

	Vec3Df position = Vec3Df(0, 0, 0);
	float scale = 1.0f;
	std::vector<GLuint> Texture;
    
    // Movement
	Vec3Df movementDirection = Vec3Df(0, 0, 0);
	float movementSpeed = 1; //units per second
    
    // Appearance
	float width = 0.5f;
	float height = 0.5f;
	Vec3Df color = Vec3Df(1, 1, 1);

	void draw();
	void animate(int deltaTime);

	std::vector<Vec3Df> getBoundingBox();
};

class Enemy : public Entity
{
public:
	Enemy();

	void initTexture();

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
    
    // Overriding
    void draw();
    void animate(int deltaTime);
    
private:
    Vec3Df spawnPoint;
    float propelledDistance = 0;
};

class Character : public Entity
{
public:
	Character();

	void initTexture();
	void updateArmAngle(Vec3Df mousePos);
	void setShoulderPos();

	// Overriding
	void draw();
	void animate(int DeltaTime);

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