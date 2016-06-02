#include "calcUtilities.h"

// Given two vectors 'a' and 'b', return the normal
std::vector<float> calcNormal(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
	std::vector<float> normal;
	normal.resize(3);

	float x = a_y * b_z - a_z * b_y;
	float y = a_z * b_x - a_x * b_z;
	float z = a_x * b_y - a_y * b_x;

	float length = sqrt(x*x + y*y + z*z);

	normal[0] = x / length;
	normal[1] = y / length;
	normal[2] = z / length;

	return normal;
}

// Given a vector, return a normalized one.
std::vector<float> normalize(std::vector<float> toBeNormalized)
{
	std::vector<float> normal;
	normal.resize(3);

	float x = toBeNormalized[0];
	float y = toBeNormalized[1];
	float z = toBeNormalized[2];

	double length = sqrt(x*x + y*y + z*z);

	normal[0] = x / length;
	normal[1] = y / length;
	normal[2] = z / length;

	return normal;
}