#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glut.h>
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
void drawMountains(void);
void displayMountains(void);
void computeMountainShadows(void);
void initMountainTextures(void);