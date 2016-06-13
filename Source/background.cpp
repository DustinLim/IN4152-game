#include "background.h"
#include "SOIL.h"

Surface::Surface(){}
Surface::~Surface(){}

Background::Background()
{
	heightMin = -5;			// <- could we set abóve the plateau, so that it reduces drawing stuff :)
	heightMax = 5;
	widthMin = -6;
	widthMax = 8;
	depth = -5;
	quadWidth = 8;
	quadHeight = 8;
	position = 0;
	speed = 0.001;

	initTexture();
}

void Surface::move()
{
	position = (position >= 1) ? 0 : position + speed;
}

void Background::draw()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	
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

void Background::initTexture()
{
	texture = SOIL_load_OGL_texture(
		"./Textures/space-background.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT);
}



Groundfloor::Groundfloor()
{
	position = 0;
	speed = 0.002;

	initTexture();
}

void Groundfloor::draw()
{
	glColor3f(1, 1, 1);
	glNormal3f(0, 1, 0);

	glBegin(GL_QUADS);
		glVertex3f(-3.0f, -1.0f, 5.0f);
		glVertex3f(3.0f, -1.0f, 5.0f);
		glVertex3f(3.0f, -1.0f, -3.0f);
		glVertex3f(-3.0f,-1.0f, -3.0f);
	glEnd();
}

void Groundfloor::initTexture()
{
	texture = SOIL_load_OGL_texture(
		"./Textures/moon-surface.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_DDS_LOAD_DIRECT);
}