#include "landscape.h"

// One should not use this constructor!
Ridge::Ridge()
{
	Ridge(0, 50, 10, 0, 0, 0, "./Textures/sand.ppm");
}

Ridge::Ridge(unsigned int rn, int resX, int resZ, float startPos, float spd, double depth, const char *texLoc)
{
	ridgeNumber = rn;

	numVertX = resX;
	numVertZ = resZ;
	numberOfGridpoints = numVertX * numVertZ;
	numberOfTriangles = (numVertX - 1) * (numVertZ - 1) * 2;

	lengthX = getRidgePeriod();
	stepSizeX = lengthX / (numVertX - 1);
	lengthZ = 1;
	stepSizeZ = lengthZ / (numVertZ - 1);

	// For now the same for every ridge.
	zMin = 0;
	zMax = lengthZ;
	scope = (zMax - zMin) / 2.0;

	// DISPLAY VARIABLES
	position = startPos;				
	speed = spd;			
	zDepth = depth;			
	boundaryLeft = -5;			// For now the same for every ridge
	boundaryRight = 5;			// For now the same for every ridge

	// INIT the vectors and Textures;
	initTexture(texLoc);

	meshVertices.resize(3 * numberOfGridpoints);
	meshNormals.resize(3 * numberOfGridpoints);
	meshTexCoords.resize(2 * numberOfGridpoints);
	meshTriangles.resize(3 * numberOfTriangles);
	meshColors.resize(3 * numberOfGridpoints);
	triangleNormals.resize(numberOfTriangles * 3);

	// And create the ridge off course!
	createRidge();
}

Ridge::~Ridge() {}

float Ridge::getHeight(float X, float Z)
{
	if (ridgeNumber == 1)
		return ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (std::max(0.0, 0.3 *sin(3 * X) + 1));
	if (ridgeNumber == 2)
		return ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (std::max(0.0, 0.3 *cos(3 * X) + 1));
	else
		return ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (std::max(0.0, 0.3 *sin(3 * X) + 1));
}

float Ridge::getRidgePeriod()
{
	// For now, the period is always the same :)
	if (ridgeNumber == 1)
		return (2.0 / 3.0) * M_PI;
	if (ridgeNumber == 2)
		return (2.0 / 3.0) * M_PI;
	else
		return (2.0 / 3.0) * M_PI;
}



// Update the parameters needed to let the landscape move <- called in 'main->animate'
void Ridge::move()
{
	position -= speed;
}


void Ridge::computeShadows()
{
	// Not implemented yet
}


void Ridge::draw()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Textures[0]);

	// start- and endpos are the start and end X-coördinate of a mountain fraction.
	float endpos = position + lengthX;
	float startpos = position;

	glPushMatrix();
	glTranslated(position, -1, zDepth);		// start position for drawing, which is moving in the -X direction.

	// Now, we only draw when our start position is inside the screen boundary.
	while (startpos < boundaryRight)
	{
		// we only stárt drawing as soon as our end position is in the screen.
		if (endpos > boundaryLeft)
		{
			for (int t = 0; t < meshTriangles.size(); t += 3)
			{
				glBegin(GL_TRIANGLES);

				for (int triVertex = 0; triVertex < 3; ++triVertex)
				{
					int vIndex = meshTriangles[t + triVertex];

					glTexCoord2fv(&(meshTexCoords[2 * vIndex]));
					glNormal3fv(&(meshNormals[3 * vIndex]));
					glColor3fv(&(meshColors[3 * vIndex]));
					glVertex3fv(&(meshVertices[3 * vIndex]));
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

void Ridge::createRidge()
{
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
			float Y = getHeight(X, Z);

			// Storing the absolute vertex coordinates.
			meshVertices[index] = X;
			meshVertices[index + 2] = -Z;
			meshVertices[index + 1] = Y;
						
			// Setting the meshColors - in this case following the coördinates and the 'B' value ('z' value) always 1;
			meshColors[index] = (Y >= 0.7) ? 1 : 0.5 + Y;
			meshColors[index + 1] = (Y >= 0.7) ? 1 : 0.5;
			meshColors[index + 2] = (Y >= 0.8) ? 1 : 0;
			
			index += 3;

			// Define the texture coördinates (2D), using stepSize I guess -> How 'larger' the coördinate, how 'finer' the texture!
			meshTexCoords[indexTexture] = stepSizeX * x;
			meshTexCoords[indexTexture + 1] = stepSizeZ * z;
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
				meshTriangles[index] = g;
				meshTriangles[index + 1] = g + 1;
				meshTriangles[index + 2] = g + numVertX;
				index += 3;
			}
			if (row > 0)			// Add a downward triangle
			{
				meshTriangles[index] = g;
				meshTriangles[index + 1] = g + 1 - numVertX;
				meshTriangles[index + 2] = g + 1;
				index += 3;
			}
		}
		else		// We reach the end of the row
			row++;
	}

	// Calculation of all Vertex Normals, having the following steps:
	// (1) calculate the normals of all meshTriangles and save these. 
	for (int i = 0; i < meshTriangles.size(); i = i + 3)
	{
		float x1 = meshVertices[meshTriangles[i + 1] * 3] - meshVertices[meshTriangles[i] * 3];
		float y1 = meshVertices[meshTriangles[i + 1] * 3 + 1] - meshVertices[meshTriangles[i] * 3 + 1];
		float z1 = meshVertices[meshTriangles[i + 1] * 3 + 2] - meshVertices[meshTriangles[i] * 3 + 2];

		float x2 = meshVertices[meshTriangles[i + 2] * 3] - meshVertices[meshTriangles[i] * 3];
		float y2 = meshVertices[meshTriangles[i + 2] * 3 + 1] - meshVertices[meshTriangles[i] * 3 + 1];
		float z2 = meshVertices[meshTriangles[i + 2] * 3 + 2] - meshVertices[meshTriangles[i] * 3 + 2];

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
		for (int i = 0; i < meshTriangles.size(); i++)
		{
			if (meshTriangles[i] == g)
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
		meshNormals[(g * 3)] = gpNormal[0];
		meshNormals[(g * 3) + 1] = gpNormal[1];
		meshNormals[(g * 3) + 2] = gpNormal[2];
	}
}

//this function loads the textures in the GPU memory
//the function is called once when the program starts
void Ridge::initTexture(const char *texLoc)
{
	Textures.resize(1);
	Textures[0] = 0;

	PPMImage sand(texLoc);
	glGenTextures(1, &Textures[0]);

	glBindTexture(GL_TEXTURE_2D, Textures[0]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sand.sizeX, sand.sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, sand.data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
}


