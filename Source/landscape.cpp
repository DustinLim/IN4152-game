#include "landscape.h"

//an array of texture (indices)
std::vector<GLuint> Textures;

// Vectors containing the mesh information for the first mountain ridge. The following notes:
// TexCoords only have a X- and Y- coördinate.
// For the triangle indices we have three successive entries: n1, n2, n3 represent a triangle, each n* is an index representing a vertex.
// For the traingle normals we have three successive entries: xi,yi,zi of the normal of triangle 'i'
std::vector<float> meshVertices1, meshNormals1, meshColors1, meshTexCoords1;
std::vector<unsigned int> meshTriangles1;
std::vector<float> triangleNormals1;

// Vectors containing the mesh information for the second moutain ridge.
std::vector<float> meshVertices2, meshNormals2, meshColors2, meshTexCoords2;
std::vector<unsigned int> meshTriangles2;
std::vector<float> triangleNormals2;


// LANDSCAPE RESOLUTION VARIABLES 
// numVertX/numVertZ gives us the number of gridpoints in the X/Z direction pér mountain ridge
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
double zMin = 0;
double zMax = lengthZ;
double scope = (zMax - zMin) / 2.0;

// DISPLAY VARIABLES
float xMoved1 = 3;				// Updates the transformation matrix to create flowing mountain ridge 1
float speed1 = 0.01;			// Determines the moving speed of mountain ridge 1

float xMoved2 = 3;
float speed2 = 0.016;

double boundaryLeft = -5;	// Represents the end of the screen at the left side (minimum needed drawn point)
double boundaryRight = 5;	// Represents the end of the screen at the right side (maximum needed drawn point)

// Update the parameters needed to let the landscape move <- called in 'animate'
void moveMountains( )
{
	xMoved1 = xMoved1 + speed1;
	xMoved2 = xMoved2 + speed2;
}


void computeMountainShadows()
{
	// Doet nog niets :)
}


void drawMountains()
{
	drawMountainRidge(xMoved1, speed1, 0, meshTriangles1, meshVertices1, meshTexCoords1, meshColors1, meshNormals1);
	drawMountainRidge(xMoved2, speed2, 0, meshTriangles2, meshVertices2, meshTexCoords2, meshColors2, meshNormals2);
}

void drawMountainRidge(float xMoved, float speed, unsigned int textureNum, std::vector<unsigned int> triangles, std::vector<float> vertices, 
							std::vector<float> texcoords, std::vector<float> colors, std::vector<float> normals)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Textures[textureNum]);

	// start- and endpos are the start and end X-coördinate of a mountain fraction.
	float endpos = -xMoved + lengthX;
	float startpos = -xMoved;

	glPushMatrix();
	glTranslated(-xMoved, -1, 0);		// start position for drawing, which is moving in the -X direction.

	// Now, we only draw when our start position is inside the screen boundary.
	while (startpos < boundaryRight)
	{
		// we only stárt drawing as soon as our end position is in the screen.
		if (endpos > boundaryLeft)
		{
			for (int t = 0; t < triangles.size(); t += 3)
			{
				glBegin(GL_TRIANGLES);

				for (int triVertex = 0; triVertex < 3; ++triVertex)
				{
					int vIndex = triangles[t + triVertex];

					glTexCoord2fv(&(texcoords[2 * vIndex]));
					glNormal3fv(&(normals[3 * vIndex]));
					glColor3fv(&(colors[3 * vIndex]));
					glVertex3fv(&(vertices[3 * vIndex]));
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
	createMountainRidge(1);
	createMountainRidge(2);
}

void createMountainRidge(int ridgeNumber)
{
	std::vector<float> vertices, normals, colors, textcoords, triangleNormals;
	std::vector<unsigned int> triangles;

	vertices.resize(3 * numberOfGridpoints);
	normals.resize(3 * numberOfGridpoints);
	textcoords.resize(2 * numberOfGridpoints);
	triangles.resize(3 * numberOfTriangles);
	colors.resize(3 * numberOfGridpoints);
	triangleNormals.resize(numberOfTriangles * 3);

	// The mountain ridges will be places behind each other, thus the Z-coordinate needs a starting value;
	double zStart = -zMax * (ridgeNumber - 1);
	
	int index = 0;				// index used in arrays where we take a 'three'-step
	int indexTexture = 0;		// index used in the texture vector where we take a 'two'-step (we don't have a z-value)
	
	// Creating the gridpoint, the order is (0,0) - (1,0) - (0,1) - (1,1) when numVertX, numVertZ = 2
	for (int z = 0; z < numVertZ; z++)
	{
		for (int x = 0; x < numVertX; x++)
		{
			// Setting the Vertex-coordinates
			float X = stepSizeX * x;
			float Z = stepSizeZ * z;		// relative coordinate, when we save we 'add' the zStart;

			float y;						// Every ridge will have it's own height function.
			if (ridgeNumber == 1)
				y = ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (max(0, 0.3 *sin(3* X)+1));
			else
				y = ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (max(0, 0.3 *cos(3 * X) + 1));

			// Storing the absolute vertex coordinates.
			vertices[index] = X;
			vertices[index + 2] = -Z + zStart;
			vertices[index + 1] = y;
						
			// Setting the Colors - in this case following the coördinates and the 'B' value ('z' value) always 1;
			colors[index] = (y >= 0.7) ? 1 : 0.5 + y;
			colors[index + 1] = (y >= 0.7) ? 1 : 0.5;
			colors[index + 2] = (y >= 0.8) ? 1 : 0;
			
			index += 3;

			// Define the texture coördinates (2D), using stepSize I guess -> How 'larger' the coördinate, how 'finer' the texture!
			textcoords[indexTexture] = stepSizeX * x;
			textcoords[indexTexture + 1] = stepSizeZ * z;
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
				triangles[index] = g;
				triangles[index + 1] = g + 1;
				triangles[index + 2] = g + numVertX;
				index += 3;
			}
			if (row > 0)			// Add a downward triangle
			{
				triangles[index] = g;
				triangles[index + 1] = g + 1 - numVertX;
				triangles[index + 2] = g + 1;
				index += 3;
			}
		}
		else		// We reach the end of the row
			row++;
	}

	// Calculation of all Vertex Normals, having the following steps:
	// (1) calculate the normals of all triangles and save these. 
	for (int i = 0; i < triangles.size(); i = i + 3)
	{
		float x1 = vertices[triangles[i + 1] * 3] - vertices[triangles[i] * 3];
		float y1 = vertices[triangles[i + 1] * 3 + 1] - vertices[triangles[i] * 3 + 1];
		float z1 = vertices[triangles[i + 1] * 3 + 2] - vertices[triangles[i] * 3 + 2];

		float x2 = vertices[triangles[i + 2] * 3] - vertices[triangles[i] * 3];
		float y2 = vertices[triangles[i + 2] * 3 + 1] - vertices[triangles[i] * 3 + 1];
		float z2 = vertices[triangles[i + 2] * 3 + 2] - vertices[triangles[i] * 3 + 2];

		std::vector<float> normal = calcNormal(x1, y1, z1, x2, y2, z2);
		triangleNormals[i] = normal[0];
		triangleNormals[i + 1] = normal[1];
		triangleNormals[i + 2] = normal[2];
	}

	// (2) For each gridpoint, add all normals of triangles this gridpoint is a part of.
	for (int g = 0; g < numberOfGridpoints; g++)
	{
		std::vector<float> gpNormal;
		gpNormal.resize(3);
		int surfaces = 0;
		for (int i = 0; i < triangles.size(); i++)
		{
			if (triangles[i] == g)
			{
				int triangle = floor(i / 3);
				gpNormal[0] += triangleNormals[(triangle * 3)];
				gpNormal[1] += triangleNormals[(triangle * 3) + 1];
				gpNormal[2] += triangleNormals[(triangle * 3) + 2];
				surfaces++;
			}
		}

		// (3) Average and normalize all these normal-sums and save as the vertex normal.
		gpNormal[0] = gpNormal[0] / surfaces;
		gpNormal[1] = gpNormal[1] / surfaces;
		gpNormal[2] = gpNormal[2] / surfaces;
		gpNormal = normalize(gpNormal);
		normals[(g * 3)] = gpNormal[0];
		normals[(g * 3) + 1] = gpNormal[1];
		normals[(g * 3) + 2] = gpNormal[2];
	}


	if (ridgeNumber == 1)
	{
		meshVertices1 = vertices;
		meshNormals1 = normals;
		meshColors1 = colors;
		meshTexCoords1 = textcoords;
		meshTriangles1 = triangles;
		triangleNormals1 = triangleNormals;
	}
	if (ridgeNumber == 2)
	{
		meshVertices2 = vertices;
		meshNormals2 = normals;
		meshColors2 = colors;
		meshTexCoords2 = textcoords;
		meshTriangles2 = triangles;
		triangleNormals2 = triangleNormals;
	}

}

//this function loads the textures in the GPU memory
//the function is called once when the program starts
void initMountainTextures()
{
	Textures.resize(1);
	Textures[0] = 0;

	
	PPMImage sand("./Textures/sand.ppm");
	glGenTextures(1, &Textures[0]);

	glBindTexture(GL_TEXTURE_2D, Textures[0]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sand.sizeX, sand.sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, sand.data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}


