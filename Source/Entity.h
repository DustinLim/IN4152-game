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

class Projectile: public Entity
{
public:
    Projectile(Vec3Df spawnPoint, Vec3Df direction);
    
    // Two textures *must* be included here
    static std::vector<GLuint> textureSet;
    
    // Overriding
    void draw();
    void animate(int deltaTime);
    
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

	void initTexture();
	void updateArmAngle(Vec3Df direction);

	// Overriding
	void draw();
	void animate(int deltaTime);

private:
	float armAngle;				// could be used to calculate angle for arm
	float turnAround;			// simulates boolean
};