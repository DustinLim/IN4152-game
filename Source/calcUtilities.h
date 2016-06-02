#include "argumentParser.h"

#define _USE_MATH_DEFINES			// Nodig op PI te gebruiken.
#include <math.h>

std::vector<float> normalize(std::vector<float> toBeNormalized);
std::vector<float> calcNormal(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z);