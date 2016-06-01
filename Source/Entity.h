#pragma once
#include "Vec3D.h"

class Entity
{
public:
	Entity();
	~Entity(); //class destructor

	Vec3Df position = Vec3Df(0, 0, 0);
	Vec3Df movementDirection = Vec3Df(0, 0, 0);
	float movementSpeed = 1; //units per second
	float size = 0.5;
	Vec3Df color = Vec3Df(1, 1, 1);

	void draw();
	void animate(int deltaTime);
};

