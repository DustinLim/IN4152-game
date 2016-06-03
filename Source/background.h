#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/glut.h>
#elif defined (__APPLE__)
#include <GLUT/glut.h>
#endif

#include "argumentParser.h"
#include "loadppm.h"


class Background
{
public:
	Background();
	~Background();

	void move(void);
	void draw(void);
	void initTexture(void);

	std::vector<GLuint> Texture;

	// BOUNDARY VALUES - should follow from the screen size.
	int heightMin;
	int heightMax;
	int widthMin;
	int widthMax;

	// Z-value for the background
	int depth;

	// RESOLUTION VARIABLES
	int quadWidth;
	int quadHeight;

	// Background Speed and 'startlocation'
	float position;
	float speed;

};