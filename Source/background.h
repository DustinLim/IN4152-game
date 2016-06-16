#include "commonOpenGL.h"

#include "argumentParser.h"


class Surface
{
public:
	Surface();
	~Surface();

	// Background Speed and 'startlocation'
	float position;
	float speed;

	void move(float deltaTime);
	virtual void draw() {};

};


class Background : public Surface
{
public:
	Background();

	static GLuint texture;

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

	void draw();
};

class Groundfloor : public Surface
{
public:
	Groundfloor();

	static GLuint texture;

	void draw();

	float width;	// X
	float depth;	// Z
	float height;	// Y
	float startDepth;
	float textureWidth;
	float textureDepth;
};