#pragma once

#if defined(_WIN32)
#include <GL/glut.h>
#include <GL/glext.h>
#elif defined (__APPLE__)
#include <GLUT/glut.h>
#endif

#include "Vec3D.h"
#include "argumentParser.h"

#include "SOIL.h"

class Entity
{
public:
	Entity();
	~Entity(); //class destructor

	Vec3Df position = Vec3Df(0, 0, 0);
	std::vector<GLuint> Texture;
    
    // Movement
	Vec3Df movementDirection = Vec3Df(0, 0, 0);
	float movementSpeed = 1; //units per second
    
    // Appearance
	float size = 0.5;
	Vec3Df color = Vec3Df(1, 1, 1);

	void draw();
	void animate(int deltaTime);
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
	void updateArmAngle(Vec3Df direction);

	// Overriding
	void draw();
	void animate(int deltaTime);

private:
	float armAngle;				// could be used to calculate angle for arm
};