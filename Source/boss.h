#pragma once
#include "Vec3D.h"
#define _USE_MATH_DEFINES
#include <math.h>

class Leg
{
private:
	float height;
	float pawHeight, pawWidth, pawDepth;
	Vec3Df toe_base, toe, hip;
	float angle_x, angle_y, angle_alpha;
	void drawPaw();
	void init();

public:
	Leg(){};
	Leg(float height, Vec3Df hip, Vec3Df toe);
	Vec3Df getToeBase();
	void moveToe(Vec3Df new_toe){ toe = new_toe; init(); }
	void moveHip(Vec3Df new_hip){ hip = new_hip; init(); }
	void drawLeg();
};

class Boss
{

private:
	Leg legs[6];
	float slices;
	float body_height;
	float body_radius;
	Vec3Df center = Vec3Df();
	Vec3Df position;

	float leg_size;
	float toe_distance_factor;
	float step_size_factor;
	float base_speed;
	float walking_speed;
	float cycle = 0;
	bool odd_cycle = 0;
	Vec3Df movement_vec;
	Vec3Df destination;

	//These variables will by set by init()
	float body_elevation;
	float hip_radius;
	float toe_distance;
	float step_size;
	float cycle_duration;

	
	void init();
	void drawBody();
	void drawHead();

public:
	//float speed = 0.1;

	Boss(Vec3Df pos = Vec3Df(0, 0, 0), float speed = 1);
	void animate(float delta);
	
	void drawBoss(float scale = 1.0);
	void moveBody(Vec3Df delta);

	void setDestination(Vec3Df dest, float speed);
	void setWalkingSpeed(float speed);
};