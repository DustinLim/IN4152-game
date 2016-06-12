//
//  This is our common header file, containing includes 
//  that are needed for (practically) all of our files.
//  To avoid repetition of includes, use this file instead.
//  WARNING: keep the number of #imports here to a minimum
//

// Windows-specific imports
#if defined(_WIN32)
// Needed to expose OpenGL functions in versions > 1.1
#define GLEW_STATIC
#include <glew.h>
// GLUT is a library for managing a window, mouse/keyboard events etc.
#include <GL/glut.h>

// Mac-specific imports
#elif defined (__APPLE__)
#include <GLUT/glut.h>
#endif
