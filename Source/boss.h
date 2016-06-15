#pragma once
#include "Vec3D.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <functional>
#include "mesh.h"
#include "grid.h"
#include "Entity.h"

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

class Boss:  public Entity
{

private:
	Leg legs[6];
	float slices;
	float body_height;
	float body_radius;
	Vec3Df center = Vec3Df();
	int move = 0;

	const Vec3Df* target;
	//float scale;

	float leg_size;
	float toe_distance_factor;
	float step_size_factor;
	float base_speed;
	float walking_speed;
	float cycle = 0;
	float linger_time = 0;
	bool odd_cycle = 0;
	Vec3Df movement_vec;
	Vec3Df destination;

	std::vector<Mesh >meshes = {};
	int meshIndex = 0;

	//These variables will by set by init()
	float body_elevation;
	float hip_radius;
	float toe_distance;
	float step_size;
	float cycle_duration;
	std::vector<std::function<void (Boss *)>> move_list;
	
	
	void init();
	void drawBody();
	void drawHead();
	void linger(int ms);

public:
	//float speed = 0.1;
	//Vec3Df position;

	Boss(Vec3Df pos = Vec3Df(0, 0, 0), float speed = 1, float scale = 1);
	void animate(float delta);
	
	void draw();
	void moveBody(Vec3Df delta);

	void setDestination(Vec3Df dest, float speed);
	void setWalkingSpeed(float speed);
	void setTarget(const Vec3Df* target);
	void nextMove(unsigned int n);

	void hit();
	float getHeadWidth();
	//Gets bounding box for head
	std::vector<Vec3Df> getBoundingBox();
	Mesh& getMesh();
	Vec3Df translation;
	float angleHeadZ, angleHeadY;
};