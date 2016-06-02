#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glut.h>

#include "argumentParser.h"
#include "loadppm.h"

void moveBackground(void);
void drawBackground(void);
void initBackgroundTexture(void);