#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/glut.h>
#elif defined (__APPLE__)
#include <GLUT/glut.h>
#endif

#include "argumentParser.h"
#include "loadppm.h"

void moveBackground(void);
void drawBackground(void);
void initBackgroundTexture(void);