#pragma once
#include "Vec3D.h"

class Character
{
public:
	Character();
	~Character(); //class destructor

	Vec3Df position = Vec3Df(0, 0, 1);
	float size = 0.5;

	void draw();
};

