#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/glut.h>
#elif defined (__APPLE__)
#include <GLUT/glut.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define _USE_MATH_DEFINES			// Nodig op PI te gebruiken.
#include <math.h>

#include "argumentParser.h"
#include "calcUtilities.h"
#include "loadppm.h"

void moveMountains(void);

void initMountains(void);
void createMountainRidge(int ridgeNumber);

void drawMountains(void);
void drawMountainRidge(float xMoved, float speed, unsigned int textureNum, std::vector<unsigned int> triangles, std::vector<float> vertices,
	std::vector<float> texcoords, std::vector<float> colors, std::vector<float> normals);

void displayMountains(void);
void computeMountainShadows(void);
void initMountainTextures(void);