#if defined(_WIN32)
#include <windows.h>
#endif
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "trackball.h"
#include "argumentParser.h"
#include <stdio.h>
#include <string.h>
#include "loadppm.h"

#define _USE_MATH_DEFINES			// Nodig op PI te gebruiken.
#include <math.h>

unsigned int W_fen = 800;  // screen width
unsigned int H_fen = 800;  // screen height


//an array of texture (indices)
std::vector<GLuint> Texture;

//light position in the scene
float LightPos[4] = {0,0,1,1};

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


// RESOLUTION VARIABLES
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

// Declarated functions;
std::vector<float> calcNormal(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z);
std::vector<float> normalize(std::vector<float> toBeNormalized);
void calcAllNormals(void);


////////// Draw Functions 

//function that draws the light source as a sphere
void drawLight()
{	
	//remember all states of the GPU
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//deactivate the lighting state
	glDisable(GL_LIGHTING);
	//yellow sphere at light position
	glColor3f(1,1,0);
	glPushMatrix();
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]); 
	glutSolidSphere(0.1,6,6);
	glPopMatrix();

	//reset to previous state
	glPopAttrib();
}



/**
 * Animation
 */
void animate( )
{
	time = time + 0.005;
}


void computeShadows()
{
	//function to compute shadows of the terrain
}

void initSurfaceMesh()
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
			// Step 1: Setting the Vertex-coordinates
			float X = stepSizeX * x;
			float Z = stepSizeZ * z;
			float y = ((-(Z - scope)*(Z - scope) + scope*scope) / (scope*scope)) * (max(0, 3.3 *sin(3* X)+1));

			SurfaceVertices3f[index] = stepSizeX * x;
			SurfaceVertices3f[index + 2] = -stepSizeZ * z;
			SurfaceVertices3f[index + 1] = y;
						
			// Step 3: Setting the Colors - in this case following the coördinates and the 'B' value ('z' value) always 1;
			SurfaceColors3f[index] = y;
			SurfaceColors3f[index + 1] = 1;
			SurfaceColors3f[index + 2] = y;
			
			index += 3;

			// Step 4: Define the texture coördinates, using stepSize I guess -> How 'larger' the coördinate, how 'finer' the texture!
			SurfaceTexCoords2f[indexTexture] = stepSizeX * x;
			SurfaceTexCoords2f[indexTexture + 1] = stepSizeZ * z;
			indexTexture += 2;
		}
	}

	// Step 5: Save the triangle vertices.
	index = 0;
	int row = 0;		// maxRow = NbVertZ - 1!
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

	calcAllNormals();
}

void calcAllNormals(void)
{
	// Step 2, attempt 2.2: First calculate the normals of all triangles and save these. 
	// Then we find which triangles belong to a certain vertex and add these normals, calculate average and normalize!
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

	// For each gridpoint, find out which triangle use that gridpoint and do some magic :)
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

		// Calculate average and normalize the result.
		gpNormal[0] = gpNormal[0] / surfaces;
		gpNormal[1] = gpNormal[1] / surfaces;
		gpNormal[2] = gpNormal[2] / surfaces;
		gpNormal = normalize(gpNormal);

		// Save in SurfaceNormals3f
		SurfaceNormals3f[(g * 3)] = gpNormal[0];
		SurfaceNormals3f[(g * 3) + 1] = gpNormal[1];
		SurfaceNormals3f[(g * 3) + 2] = gpNormal[2];
	}
}

std::vector<float> calcNormal(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z)
{
	std::vector<float> normal;
	normal.resize(3);

	float x = a_y * b_z - a_z * b_y;
	float y = a_z * b_x - a_x * b_z;
	float z = a_x * b_y - a_y * b_x;

	float length = sqrt(x*x + y*y + z*z);

	normal[0] = x / length;
	normal[1] = y / length;
	normal[2] = z / length;

	return normal;
}

std::vector<float> normalize(std::vector<float> toBeNormalized)
{
	std::vector<float> normal;
	normal.resize(3);

	float x = toBeNormalized[0];
	float y = toBeNormalized[1];
	float z = toBeNormalized[2];

	double length = sqrt(x*x + y*y + z*z);

	normal[0] = x / length;
	normal[1] = y / length;
	normal[2] = z / length;

	return normal;
}

void drawSurface()
{
	int timesDrawn = 0;
	float endpos = -time + lengthX;
	float startpos = -time;
	
	glPushMatrix();
	glTranslated(-time, -1, -4);		// start position for drawing, which is moving in the -X direction.

	// Don't draw as long as our end position is not in the screen yet.
	while (endpos < boundaryLeft)
	{
		endpos += lengthX;
		startpos += lengthX;
		glTranslated(lengthX, 0, 0);
	}
	
	// Now, we only draw when our start position is inside the screen boundary.
	while (startpos < boundaryRight)
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

		timesDrawn++;
		endpos += lengthX;
		startpos += lengthX;
		glTranslated(lengthX, 0, 0);
	}
	
	glPopMatrix();
}

//this function loads the textures in the GPU memory
//the function is called once when the program starts
void initTexture()
{
	Texture.resize(3);
	Texture[0]=0;

	PPMImage sand("C:/CGResources/sand.ppm");
	glGenTextures(0, &Texture[0]);

	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sand.sizeX, sand.sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, sand.data);
	glBindTexture(GL_TEXTURE_2D, 0);
}



//take keyboard input into account
void keyboard(unsigned char key, int x, int y)
{
    printf("key %d pressed at %d,%d\n",key,x,y);
    fflush(stdout);

	switch (key)
    {
	case 27:     // ESC
        exit(0);
	case 'L':
		//turn lighting on
		glEnable(GL_LIGHTING);
		break;
	case 'l':
		//turn lighting off
		glDisable(GL_LIGHTING);
		break;
	case 'a':
		//move light
		LightPos[0]-=0.1;
		computeShadows();
		break;
	case 'd':
		//move light
		LightPos[0]+=0.1;
		computeShadows();
		break;
	case 'w':
		//move light
		LightPos[1]+=0.1;
		computeShadows();
		break;
	case 's':
		//move light
		LightPos[1]-=0.1;
		computeShadows();
		break;
    }
}


void display( )
{
	glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
	drawLight();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture[2]);
	drawSurface();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}



//Nothing needed below this point
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 
//STOP READING //STOP READING //STOP READING 





void displayInternal(void);
void reshape(int w, int h);
void init()
{
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

	//int MatSpec [4] = {1,1,1,1};
 //   glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,MatSpec);
 //   glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,10);


    // Enable Depth test
    glEnable( GL_DEPTH_TEST );
	
	//glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
	//Draw frontfacing polygons as filled
    glPolygonMode(GL_FRONT,GL_FILL);
	//draw backfacing polygons as outlined
    glPolygonMode(GL_BACK,GL_LINE);
	glShadeModel(GL_SMOOTH);
	initSurfaceMesh();
	initTexture();
}


/**
 * Programme principal
 */
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    // couches du framebuffer utilisees par l'application
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

    // position et taille de la fenetre
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(W_fen,H_fen);
    glutCreateWindow(argv[0]);

    init( );
	
    // Initialize viewpoint
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-4);
    tbInitTransform();     
    tbHelp();
         
    

	// cablage des callback
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(displayInternal);
    glutMouseFunc(tbMouseFunc);    // traqueboule utilise la souris
    glutMotionFunc(tbMotionFunc);  // traqueboule utilise la souris
    glutIdleFunc(animate);

    // lancement de la boucle principale
    glutMainLoop();
    
    return 0;  // instruction jamais exécutée
}

/**
 * Fonctions de gestion opengl à ne pas toucher
 */
// Actions d'affichage
// Ne pas changer
void displayInternal(void)
{
    // Effacer tout
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT); // la couleur et le z
    

    glLoadIdentity();  // repere camera

    tbVisuTransform(); // origine et orientation de la scene

    display( );    

    glutSwapBuffers();
    glutPostRedisplay();
}
// pour changement de taille ou desiconification
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho (-1.1, 1.1, -1.1,1.1, -1000.0, 1000.0);
    gluPerspective (50, (float)w/h, 1, 10);
    glMatrixMode(GL_MODELVIEW);
}

