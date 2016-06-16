#include "commonOpenGL.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define _USE_MATH_DEFINES			// Nodig op PI te gebruiken.
#include <math.h>

#include "argumentParser.h"
#include "calcUtilities.h"
#include "loadppm.h"


class Ridge
{
public:
	Ridge();
	Ridge(unsigned int rn, int resX, int resZ, float startPos, float spd, double depth);
	~Ridge();

	// contains one texture for all ridges
	static std::vector<GLuint> textureSet;
	static const int RIDGE_TEXTURE_ID = 0;
	static void initTexture(const char *texLoc);

	void move();
	void draw();

	// Different steps inside these method will depend on the ridgenumber.
	void createRidge();

	float getHeight(float X, float Z);	// Returns the 'y', thus here the ridge formulas are defined
	float getRidgePeriod();				// Here, the different ridge periods are stored!

	void computeShadows(void);			// Not implemented yet

	unsigned int ridgeNumber;

	// Vectors containing the mesh information for the first mountain ridge. The following notes:
	// TexCoords only have a X- and Y- coördinate.
	// For the triangle indices we have three successive entries: n1, n2, n3 represent a triangle, each n* is an index representing a vertex.
	// For the traingle normals we have three successive entries: xi,yi,zi of the normal of triangle 'i'
	std::vector<float> meshVertices, meshNormals, meshColors, meshTexCoords;
	std::vector<unsigned int> meshTriangles;
	std::vector<float> triangleNormals;

	// LANDSCAPE RESOLUTION VARIABLES 
	int numVertX;
	int numVertZ;
	int numberOfGridpoints;
	int numberOfTriangles;
	
	// SIZE VARIABLES
	double lengthX;
	double stepSizeX;
	double lengthZ;
	double stepSizeZ;

	// Used to create the 3D Z-relation.
	double zMin;
	double zMax;
	double scope;

	// DISPLAY VARIABLES
	float position;
	float speed;

	double boundaryLeft = -5;	// Represents the end of the screen at the left side (minimum needed drawn point)
	double boundaryRight = 5;	// Represents the end of the screen at the right side (maximum needed drawn point)
	double zDepth = -3;			// Represents starting depth of the ridge.

};
