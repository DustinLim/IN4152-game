#include "commonOpenGL.h"

#include "argumentParser.h"


class Surface
{
public:
	Surface();
	~Surface();

	GLuint texture;

	// Background Speed and 'startlocation'
	float position;
	float speed;

	void move(void);

};


class Background : public Surface
{
public:
	Background();

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

	void draw(void);
	void initTexture(void);
};

class Groundfloor : public Surface
{
public:
	Groundfloor();

	void draw(void);
	void initTexture(void);

	float width;	// X
	float depth;	// Z
	float height;	// Y
	float startDepth;
};