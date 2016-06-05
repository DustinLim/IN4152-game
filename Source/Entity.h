#pragma once
#include "Vec3D.h"

class Entity
{
public:
	Entity();
	~Entity(); //class destructor

	Vec3Df position = Vec3Df(0, 0, 0);
    
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