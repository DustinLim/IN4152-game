#include "landscape.h"

std::vector<GLuint> Ridge::textureSet;

// One should not use this constructor!
Ridge::Ridge()
{
	Ridge(0, 50, 10, 0, 0, 0);
}

Ridge::Ridge(unsigned int rn, int resX, int resZ, float startPos, float spd, double depth)
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
	boundaryLeft = -8;			// For now the same for every ridge
	boundaryRight = 8;			// For now the same for every ridge

	// INIT
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
	float Y;
	if (ridgeNumber == 1)
	{
		Y = ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) *
			(std::max(0.0, 0.3 *sin(3 * X) + 0.6 * cos(5.0 / 6.0 * X) + 0.5 * sin(2.0 / 3.0 * X) + 1.8));
	}
	else if (ridgeNumber == 2)
	{
		Y = ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) *
			(std::max(0.0, 0.2 *cos(1.0 / 3.0 * X) + 0.3 * sin(4.0 / 3.0 * X) - 0.1 * sin(2.0 *X) + 0.7));
	}
	else
	{
		// not really implemented yet, return easy sinus.
		return ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (std::max(0.0, 0.3 *sin(3 * X) + 1));
	}
	
	// When we are not living on the edge, add some random height.
	if (X != 0 && X != lengthX & Z != 0 & Z != lengthZ)
	{
		Y += (rand() % 10) * 0.05;
	}

	return Y;
}

float Ridge::getRidgePeriod()
{
	if (ridgeNumber == 1)
		return 12.0 * M_PI;				// With help from Wolfram Alpha
	else if (ridgeNumber == 2)
		return 6.0 * M_PI;
	else
		return (2.0 / 3.0) * M_PI;
}



// Update the parameters needed to let the landscape move <- called in 'main->animate'
void Ridge::move(float deltaTime)
{
	position -= deltaTime*speed/1000.0f;
}

void Ridge::draw()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureSet[RIDGE_TEXTURE_ID]);

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
			meshVertices[index + 2] = ((Z == 0 || Z == lengthZ) && (X != 0 && X != lengthX)) ? -Z + (rand() % 10 * 0.05) : -Z;
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

	// EXTRA - For each first and last gridpoint per row, we need to add these results, because the ridge continues.
	// We only look at the first gridpoint per row and then to the calculations for both the first and last gridpoint (as their result is the same!)
	for (int g = 0; g < numberOfGridpoints; g += numVertX)
	{
		// Calculate the actual normal for the gridpoints.
		std::vector<float> gpNormal;
		gpNormal.resize(3);
		gpNormal[0] = (meshNormals[(g * 3)] + meshNormals[((g + numVertX - 1) * 3)])			/ 2.0f;
		gpNormal[1] = (meshNormals[(g * 3) + 1] + meshNormals[((g + numVertX - 1) * 3) + 1])	/ 2.0f;
		gpNormal[2] = (meshNormals[(g * 3) + 2] + meshNormals[((g + numVertX - 1) * 3) + 2])	/ 2.0f;
		gpNormal = normalize(gpNormal);

		// Double save :)
		meshNormals[(g * 3)] = gpNormal[0];
		meshNormals[(g * 3) + 1] = gpNormal[1];
		meshNormals[(g * 3) + 2] = gpNormal[2];
		meshNormals[((g + numVertX - 1) * 3)] = gpNormal[0];
		meshNormals[((g + numVertX - 1) * 3) + 1] = gpNormal[1];
		meshNormals[((g + numVertX - 1) * 3) + 2] = gpNormal[2];
	}


}

//this function loads the textures in the GPU memory
//the function is called once when the program starts
void Ridge::initTexture(const char *texLoc)
{
	textureSet.resize(1);
	textureSet[RIDGE_TEXTURE_ID] = 0;

	PPMImage sand(texLoc);
	glGenTextures(1, &textureSet[RIDGE_TEXTURE_ID]);

	glBindTexture(GL_TEXTURE_2D, textureSet[RIDGE_TEXTURE_ID]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sand.sizeX, sand.sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, sand.data);
	glBindTexture(GL_TEXTURE_2D, RIDGE_TEXTURE_ID);
}



