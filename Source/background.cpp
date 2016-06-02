#include "background.h"

std::vector<GLuint> Texture;

// BOUNDARY VALUES - should follow from the screen size.
int heightMin = -5;			// <- could we set abóve the plateau, so that it reduces drawing stuff :)
int heightMax = 5;
int widthMin = -6;
int widthMax = 4;

// Z-value for the background
int depth = -5;

// RESOLUTION VARIABLES
int quadWidth = 3;
int quadHeight = 3;

// Background Speed and 'startlocation'
float position = 0;
float speed = 0.01;

void moveBackground()
{
	position = (position >= 1) ? 0 : position + 0.005;
}

void drawBackground()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// We are going to fill the Z plane at 'depth' width with quads of size quadWidth * quadHeight
	// As done in the landscape, we are drawing the quad given some boundaryvalues.
	for (int y = heightMin; y < heightMax; y += quadHeight)
	{
		for (int x = widthMin; x < widthMax; x += quadWidth)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0 + position, 1);
			glVertex3f(x, y, depth);

			glTexCoord2f(1 + position, 1);
			glVertex3f(x + quadWidth, y, depth);

			glTexCoord2f(1 + position, 0);
			glVertex3f(x + quadWidth, y + quadHeight, depth);

			glTexCoord2f(0 + position, 0);
			glVertex3f(x, y + quadHeight, depth);
			glEnd();
		}
	}

	glPopAttrib();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void initBackgroundTexture()
{
	Texture.resize(1);
	Texture[0] = 0;

	PPMImage stars("./Textures/img_stars.ppm");
	glGenTextures(1, &Texture[0]);

	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, stars.sizeX, stars.sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, stars.data);
	glBindTexture(GL_TEXTURE_2D, 0);
}