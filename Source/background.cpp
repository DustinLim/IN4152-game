#include "background.h"
#include "SOIL.h"

Surface::Surface(){}
Surface::~Surface(){}

#pragma region "Background"

GLuint Background::texture;

Background::Background()
{
	heightMin = -5;			// <- could we set abóve the plateau, so that it reduces drawing stuff :)
	heightMax = 5;
	widthMin = -8;
	widthMax = 8;
	depth = -5;
	quadWidth = 8;
	quadHeight = 6;
	position = 0;
	speed = 0.1;
}

void Surface::move(float deltaTime)
{
	position = (position >= 1) ? position - 1 + deltaTime*speed / 1000.0f : position + deltaTime*speed / 1000.0f;
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

#pragma endregion

#pragma region "Groundfloor"

GLuint Groundfloor::texture;

Groundfloor::Groundfloor()
{
	position = 0;
	speed = 1;
	width = 12.5f;
	height = -1.0f;
	depth = 5.0f;
	startDepth = 2.0f;
	textureWidth = 3;
	textureDepth = textureWidth*depth / (width);
	speed *= textureWidth / width;
}

void Groundfloor::draw()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3f(1, 1, 1);
	glNormal3f(0, 1, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBegin(GL_QUADS);
		glTexCoord2f(0 + position, textureDepth);				glVertex3f(-width / 2.0f, height, startDepth);
		glTexCoord2f(textureWidth + position, textureDepth);	glVertex3f(width / 2.0f, height, startDepth);
		glTexCoord2f(textureWidth + position, 0);				glVertex3f(width / 2.0f, height, startDepth - depth);
		glTexCoord2f(0 + position, 0);							glVertex3f(-width / 2.0f, height, startDepth - depth);
	glEnd();

    // FIXME: balancing with this pop breaks enemy drawing..
    //glPopAttrib();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

#pragma endregion