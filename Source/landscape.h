#if defined(_WIN32)
#include <windows.h>
#endif

#include <GL/glut.h>
#include <math.h>

#include "loadppm.h"
#include "argumentParser.h"
#include "calcUtilities.h"

void moveMountains(void);
void initMountains(void);
void drawMountains(void);
void displayMountains(void);
void computeMountainShadows(void);
void initMountainTexture(void);