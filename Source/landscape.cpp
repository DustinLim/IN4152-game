#include "landscape.h"

//an array of texture (indices)
std::vector<GLuint> Texture;

//vertices
std::vector<float> SurfaceVertices3f;
//normals
std::vector<float> SurfaceNormals3f;
//colors
std::vector<float> SurfaceColors3f;
//tex coords
std::vector<float> SurfaceTexCoords2f;
//triangle indices (three successive entries: n1, n2, n3 represent a triangle, each n* is an index representing a vertex.)
std::vector<unsigned int> SurfaceTriangles3ui;
//triangle normals(three successive entries: xi,yi,zi of the normal of triangle 'i'
std::vector<float> TriangleNormals3f;


// LANDSCAPE RESOLUTION VARIABLES
// numVertX/numVertZ gives us the number of gridpoints in the X/Z direction
int numVertX = 50;
int numVertZ = 10;
int numberOfGridpoints = numVertX * numVertZ;
int numberOfTriangles = (numVertX - 1) * (numVertZ - 1) * 2;

// SIZE VARIABLES
double lengthX = (2.0/3.0) * M_PI;			// <- is thus the length in the x-direction.
double lengthZ = 1;
double stepSizeZ = lengthZ / (numVertZ - 1);
double stepSizeX = lengthX / (numVertX - 1);

// Used to create the 3D Z-relation, can be used to built to create multiple functions.
double z_min = 0;
double z_max = lengthZ;
double scope = (z_max - z_min) / 2.0;

// DISPLAY VARIABLES
float time = 0;				// Updates the transformation matrix to create flowing mountrains
double boundaryLeft = -4;	// Represents the end of the screen at the left side (minimum needed drawn point)
double boundaryRight = 4;	// Represents the end of the screen at the right side (maximum needed drawn point)

// Update the parameters needed to let the landscape move <- called in 'animate'
void moveMountains( )
{
	time = time + 0.005;
}


void computeMountainShadows()
{
	// Doet nog niets :)
}


void drawMountains()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);

	// start- and endpos are the start and end X-coördinate of a mountain fraction.
	float endpos = -time + lengthX;
	float startpos = -time;

	glPushMatrix();
	glTranslated(-time, -1, -4);		// start position for drawing, which is moving in the -X direction.

	// Now, we only draw when our start position is inside the screen boundary.
	while (startpos < boundaryRight)
	{
		// we only stárt drawing as soon as our end position is in the screen.
		if (endpos > boundaryLeft)
		{
			for (int t = 0; t < SurfaceTriangles3ui.size(); t += 3)
			{
				glBegin(GL_TRIANGLES);

				for (int triVertex = 0; triVertex < 3; ++triVertex)
				{
					int vIndex = SurfaceTriangles3ui[t + triVertex];

					glTexCoord2fv(&(SurfaceTexCoords2f[2 * vIndex]));
					glNormal3fv(&(SurfaceNormals3f[3 * vIndex]));
					glColor3fv(&(SurfaceColors3f[3 * vIndex]));
					glVertex3fv(&(SurfaceVertices3f[3 * vIndex]));
				}
				glEnd();
			}
		}

		endpos += lengthX;
		startpos += lengthX;
		glTranslated(lengthX, 0, 0);
	}

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void initMountains()
{
	SurfaceVertices3f.resize(3 * numberOfGridpoints);
	SurfaceNormals3f.resize(3 * numberOfGridpoints);
	SurfaceTexCoords2f.resize(2 * numberOfGridpoints);
	SurfaceTriangles3ui.resize(3 * numberOfTriangles);
	SurfaceColors3f.resize(3 * numberOfGridpoints);
	TriangleNormals3f.resize(numberOfTriangles * 3);

	int index = 0;				// index used in arrays where we take a 'three'-step
	int indexTexture = 0;		// index used in the texture vector where we take a 'two'-step (we don't have a z-value)
	
	// Creating the gridpoint, the order is (0,0) - (1,0) - (0,1) - (1,1) when numVertX, numVertZ = 2
	for (int z = 0; z < numVertZ; z++)
	{
		for (int x = 0; x < numVertX; x++)
		{
			// Setting the Vertex-coordinates
			float X = stepSizeX * x;
			float Z = stepSizeZ * z;
			float y = ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (max(0, 3.3 *sin(3* X)+1));

			SurfaceVertices3f[index] = stepSizeX * x;
			SurfaceVertices3f[index + 2] = -stepSizeZ * z;
			SurfaceVertices3f[index + 1] = y;
						
			// Setting the Colors - in this case following the coördinates and the 'B' value ('z' value) always 1;
			SurfaceColors3f[index] = 0.5 + y;
			SurfaceColors3f[index + 1] = 0.5 + y;
			SurfaceColors3f[index + 2] = 0;
			
			index += 3;

			// Define the texture coördinates (2D), using stepSize I guess -> How 'larger' the coördinate, how 'finer' the texture!
			SurfaceTexCoords2f[indexTexture] = stepSizeX * x;
			SurfaceTexCoords2f[indexTexture + 1] = stepSizeZ * z;
			indexTexture += 2;
		}
	}

	// Save the triangle vertices, where we iterate through the gridpoints
	// (note the gridpoint relations; ask Marlou if confused)
	int row = 0;		// note: 'row' has a maximum of NbVertZ - 1!
	index = 0;
	for (int g = 0; g < numberOfGridpoints; g++) {
		// Note: We do nothing when our gridpoint is the last in a row (NbVertX + (NbVertX + 1) * row)
		if (g != (numVertX - 1) + (numVertX) * row)
		{
			if (row < (numVertZ - 1))		// Add an upstanding triangle
			{
				SurfaceTriangles3ui[index] = g;
				SurfaceTriangles3ui[index + 1] = g + 1;
				SurfaceTriangles3ui[index + 2] = g + numVertX;
				index += 3;
			}
			if (row > 0)			// Add a downward triangle
			{
				SurfaceTriangles3ui[index] = g;
				SurfaceTriangles3ui[index + 1] = g + 1 - numVertX;
				SurfaceTriangles3ui[index + 2] = g + 1;
				index += 3;
			}
		}
		else		// We reach the end of the row
			row++;
	}

	// Calculation of all Vertex Normals, having the following steps:
	// (1) calculate the normals of all triangles and save these. 
	for (int i = 0; i < SurfaceTriangles3ui.size(); i = i + 3)
	{
		float x1 = SurfaceVertices3f[SurfaceTriangles3ui[i + 1] * 3] - SurfaceVertices3f[SurfaceTriangles3ui[i] * 3];
		float y1 = SurfaceVertices3f[SurfaceTriangles3ui[i + 1] * 3 + 1] - SurfaceVertices3f[SurfaceTriangles3ui[i] * 3 + 1];
		float z1 = SurfaceVertices3f[SurfaceTriangles3ui[i + 1] * 3 + 2] - SurfaceVertices3f[SurfaceTriangles3ui[i] * 3 + 2];

		float x2 = SurfaceVertices3f[SurfaceTriangles3ui[i + 2] * 3] - SurfaceVertices3f[SurfaceTriangles3ui[i] * 3];
		float y2 = SurfaceVertices3f[SurfaceTriangles3ui[i + 2] * 3 + 1] - SurfaceVertices3f[SurfaceTriangles3ui[i] * 3 + 1];
		float z2 = SurfaceVertices3f[SurfaceTriangles3ui[i + 2] * 3 + 2] - SurfaceVertices3f[SurfaceTriangles3ui[i] * 3 + 2];

		std::vector<float> normal = calcNormal(x1, y1, z1, x2, y2, z2);
		TriangleNormals3f[i] = normal[0];
		TriangleNormals3f[i + 1] = normal[1];
		TriangleNormals3f[i + 2] = normal[2];
	}

	// (2) For each gridpoint, add all normals of triangles this gridpoint is a part of.
	for (int g = 0; g < numberOfGridpoints; g++)
	{
		std::vector<float> gpNormal;
		gpNormal.resize(3);
		int surfaces = 0;
		for (int i = 0; i < SurfaceTriangles3ui.size(); i++)
		{
			if (SurfaceTriangles3ui[i] == g)
			{
				int triangle = floor(i / 3);
				gpNormal[0] += TriangleNormals3f[(triangle * 3)];
				gpNormal[1] += TriangleNormals3f[(triangle * 3) + 1];
				gpNormal[2] += TriangleNormals3f[(triangle * 3) + 2];
				surfaces++;
			}
		}

		// (3) Average and normalize all these normal-sums and save as the vertex normal.
		gpNormal[0] = gpNormal[0] / surfaces;
		gpNormal[1] = gpNormal[1] / surfaces;
		gpNormal[2] = gpNormal[2] / surfaces;
		gpNormal = normalize(gpNormal);
		SurfaceNormals3f[(g * 3)] = gpNormal[0];
		SurfaceNormals3f[(g * 3) + 1] = gpNormal[1];
		SurfaceNormals3f[(g * 3) + 2] = gpNormal[2];
	}
}

//this function loads the textures in the GPU memory
//the function is called once when the program starts
void initMountainTexture()
{
	Texture.resize(1);
	Texture[0] = 0;

	//PPMImage image = PPMImage("sand.ppm");
	//glGenTextures(1, &Texture[0]);

	//glBindTexture(GL_TEXTURE_2D, Texture[0]);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY,
	//	GL_RGB, GL_UNSIGNED_BYTE, image.data);
	//glBindTexture(GL_TEXTURE_2D, 0);
}


