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



//                  .-"      "-.
//                 /            \
//                |              |
//                |,  .-.  .-.  ,|
//                | )(__/  \__)( |
//                |/     /\     \|
//      (@_       (_     ^^     _)
//  _    ) \_______\__|IIIIII|__/__________________________
//(_)@8@8{}<________|-\IIIIII/-|___________________________>
//       )_/        \          /
//      (@           `--------` jgs

//Once upon a time there was a new practical assignment.
//In the following you can find your tasks described directly in the code.
//The mission today is threefold:
//1) Learn about texture mapping and texture coordinates
//	This part is crucial, as textures are one of the most important elements that make real-time graphics look rich
//2) Setting up of a procedural mesh including connectivity and normals (in this case a wave/terrain)
//	It becomes increasingly common to produce meshes on the fly - especially, since the dawn of the geometry shader.
//  Furthermore, the way meshes should be stored on the GPU is in form of so-called vertex buffer objects.
//  We will see how to setup the arrays (vectors) in the right way - although we will not yet apply the transfer to the GPU.
//3) Optional - if time is sufficient - Apply shadows to a terrain
//	By now you have seen in the course that calculating shadows is not simple and costly. Here, we will present another alternative:
//	compute shadows for each vertex of the terrain and interpolate the result.
//	Nowadays, such approaches are often used in the context of global illumination and (more precisely) ambient occlusion, which we will soon learn about in class.
//
//
// The structure of the code is the same as last time and should look familiar!
//
// Please start by looking at the global variables and then jump to the drawQuad() function - your journey will start with this magic scroll.
// Good Luck young adventurer, your quest awaits you!



//INTERESTING CODE STARTS HERE!!!


//////Predefined global variables

//Use the enum values to define different rendering modes 
//The mode is used by the function display and the mode is 
//chosen during execution with the keys 1-9
enum DisplayModeType {TEXTURED_QUAD=1, SURFACE=2,};

DisplayModeType DisplayMode = TEXTURED_QUAD;

unsigned int W_fen = 800;  // screen width
unsigned int H_fen = 800;  // screen height


//an array of texture (indices)
std::vector<GLuint> Texture;

//light position in the scene
float LightPos[4] = {0,0,1,1};

//Vertices and texture coordinates for the terrain/water
//later you can increase NbVert* to produce a more detailed mesh
//you should know the index face set (triangles defined by vertex indices) from the last practical assignment
int NbVertX=15, NbVertY=15; 
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


// Declare your own global variables here:
float variable = 0;

// new function;
std::vector<float> calcNormal(float a_x, float a_y, float a_z, float b_x, float b_y, float b_z);
std::vector<float> normalize(std::vector<float> toBeNormalized);


////////// Draw Functions 

/**
 * Move on young padawan, may the force be with you... 
*/

void drawQuad()
{
	//Legends tell us that this scroll was once written by an old magician - who was a big CLARK GABLE fan!
        //It is supposed to produce a flying checker board, but no one could figure out how the spell works... Maybe you can?
	//1) try to change the texture coordinates, what do you observe?
	// a) try making them much smaller than 1 (e.g., 0.1)
	// you should see pixels of the texture, try to change the Mag Filter from GL_NEAREST to GL_LINEAR
	// what do you observe?
	// b) try making them much larger than 1 (e.g., 10)
	// you should see aliasing for grazing angles, try to change the Min Filter from GL_NEAREST to GL_LINEAR_MIPMAP_LINEAR
	// what do you observe?
	// c) try using negative coordinates
	// try to change GL_WRAP_S from GL_CLAMP to GL_REPEAT
	// what do you observe?
	// try to change GL_WRAP_T from GL_CLAMP to GL_REPEAT
	// what do you observe?
	//2) Keep the wrapping modes on GL_REPEAT, make the texture coordinate depend on a global variable. 
	// go to the function animate() and increment the value of this variable continously
	// (similar to animating the vertex in the last practical session), see the texture move on the object.
	// you can also make it controllable using some keys and the keyboard function.
	//2) add a color to each vertex (glColor3f), what do you observe?	
	//3) try activating the light by pressing 'L' (to deactivate 'l'). Move the light with a,d,s,w, what do you observe?
	//4) go to initTexture and follow the instructions there - the treasure hunt continues... ;)

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3f(1,1,1);
	glNormal3f(0,0,1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0,1+variable);
		glVertex2f(0,0);
		
		glTexCoord2f(1,1+variable);
		glVertex2f(1,0);

		glTexCoord2f(1,0+variable);
		glVertex2f(1,1);

		glTexCoord2f(0,0+variable);
		glVertex2f(0,1);
	glEnd();

	glPopAttrib();
}


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
	variable = (variable >= 1) ? 0 : variable + 0.005;
}


void computeShadows()
{
	//function to compute shadows of the terrain
}

void initSurfaceMesh()
{
	//The villagers are all counting on you!  
	//You need to prepare the right ingredients to make your spell work and transform the simple quad into a mountainscape!
	//This spell is only cast ONCE when the program is launched... so take your time...
	
	//Use this function to fill in the right values in the Surface*** arrays, which will represent the terrain.
	//Below you see the current version, which is still a simple quad made out of 2x2 vertices (the default values for NbVertX, NbVertY).
	
	//1) Extend the function to support any values for NbVertX, NbVertY. 
	//USE FOR LOOPS!!! The code below is only for illustration purposes!
	// transform the quad into a grid of NbVertX, NbVertY that connect up to produce triangles.
	// you can verify if it worked when you lowok at the scene FROM BELOW...

	//2) try modulating the z coordinate, use a cos function along the x axis to define z
	// the resulting surface will look like a wave. 
	
	//4) After all this work, you are exhausted... take a break... you can use it to wonder why there is no number 3... 

	//5) compute the correct normal values (you can do this analytically if you work with a cos function)
	// activate the light by pressing 
	
	//6) Associate a color value based on z - cover the mountain tips in snow (white) and fill in meadows in the valleys (green).
	
	
	//7) Play with more complex patterns e.g., cos(x)*sin(y) and compute the normals of this mesh 
	//	After all the orcs will not be stopped with a simple wave...
	
	//8) The orcs still broke though, but they are afraid of light! 
	// So start separating light and darkness: Compute a shadow value per vertex. 
	// For this, test a ray from the vertex to the light source. 
	// If any triangle of the mesh is intersected by this ray, set the vertex color to 0,0,0, else to 1,1,1
	// do not choose too big values for NbVertX, NbVertY, testing all intersections can be costly
	// Please add this computation in the function compute shadows, so that it is called when the light is moved (see function keyboard).
	//9) The orcs are coming, go to combat()...
	
	int numberOfGridpoints = NbVertX * NbVertY;
	int numberOfTriangles = (NbVertX - 1) * (NbVertY - 1) * 2;
	SurfaceVertices3f.resize(3 * numberOfGridpoints);
	SurfaceNormals3f.resize(3 * numberOfGridpoints);
	SurfaceTexCoords2f.resize(2 * numberOfGridpoints);
	SurfaceTriangles3ui.resize(3 * numberOfTriangles);
	SurfaceColors3f.resize(3 * numberOfGridpoints);
	
	double quadMaxX = 5;
	double quadMaxY = 5;
	double stepSizeX = quadMaxX / (NbVertX - 1);
	double stepSizeY = quadMaxY / (NbVertY - 1);

	int index = 0;				// index used in arrays where we take a 'three'-step
	int indexTexture = 0;		// index used in the texture array where we take a 'two'-step (we don't have a z-value)
	
	// Creating the gridpoint, the order is (0,0) - (1,0) - (0,1) - (1,1) when NbVertX, NbVertY = 2
	// And (0,0) - (0.5,0) - (1,0) - (0,0.5) - (0.5,0.5) - (0.5,1) - (0,1) - (0.5,1) - (1,1)  <- each new value for y is treated as a new row in gridpoint land.
	for (int y = 0; y < NbVertY; y++)
	{
		for (int x = 0; x < NbVertX; x++)
		{
			// Step 1: Setting the Vertex-coordinates
			SurfaceVertices3f[index] = stepSizeX * x;
			SurfaceVertices3f[index + 1] = stepSizeY * y;
			SurfaceVertices3f[index + 2] = 0.3 * cos(0.25 * x) * sin(y);				// all z = 0.
						
			// Step 3: Setting the Colors - in this case following the coördinates and the 'B' value ('z' value) always 1;
			SurfaceColors3f[index] = cos(0.25 * x) * sin(y);
			SurfaceColors3f[index + 1] = 1;
			SurfaceColors3f[index + 2] = cos(0.25 * x) * sin(y);
			
			index += 3;

			// Step 4: Define the texture coördinates, using stepSize I guess -> How 'larger' the coördinate, how 'finer' the texture!
			SurfaceTexCoords2f[indexTexture] = stepSizeX * x;
			SurfaceTexCoords2f[indexTexture + 1] = stepSizeY * y;
			indexTexture += 2;
		}
	}

	// Step 5: Save the triangle vertices.
	index = 0;
	int row = 0;		// maxRow = NbVertY - 1!
	for (int g = 0; g < numberOfGridpoints; g++) {
		// Note: We do nothing when our gridpoint is the last in a row (NbVertX + (NbVertX + 1) * row)
		if (g != (NbVertX - 1) + (NbVertX) * row)
		{
			if (row < (NbVertY - 1))		// Add an upstanding triangle
			{
				SurfaceTriangles3ui[index] = g;
				SurfaceTriangles3ui[index + 1] = g + 1;
				SurfaceTriangles3ui[index + 2] = g + NbVertX;
				index += 3;
			}
			if (row > 0)			// Add a downward triangle
			{
				SurfaceTriangles3ui[index] = g;
				SurfaceTriangles3ui[index + 1] = g + 1 - NbVertX;
				SurfaceTriangles3ui[index + 2] = g + 1;
				index += 3;
			}
		}
		else		// We reach the end of the row
			row++;
	}

	// Step 2, attempt 2.2: First calculate the normals of all triangles and save these. 
	// Then we find which triangles belong to a certain vertex and add these normals, calculate average and normalize!
	std::vector<float> TriangleNormals3f;
	TriangleNormals3f.resize(numberOfTriangles * 3);

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
		TriangleNormals3f[i+1] = normal[1];
		TriangleNormals3f[i+2] = normal[2];
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
				gpNormal[2] += TriangleNormals3f[(triangle * 3)+ 2];
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

	/* EXAMPLE
	//vertices with 3 coordinates
	SurfaceVertices3f.resize(3*2*2);
	//normals with 3 coordinates
	SurfaceNormals3f.resize(3*2*2);
	//texture coords per vertex
	SurfaceTexCoords2f.resize(2*2*2);
	//triangles (2 per default)
	SurfaceTriangles3ui.resize(3*2);
	//per vertex colors
	SurfaceColors3f.resize(3*2*2);

	//define coords
	SurfaceVertices3f[0]=0;
	SurfaceVertices3f[1]=0;
	SurfaceVertices3f[2]=0;

	SurfaceVertices3f[3]=1;
	SurfaceVertices3f[4]=0;
	SurfaceVertices3f[5]=0;

	SurfaceVertices3f[6]=1;
	SurfaceVertices3f[7]=1;
	SurfaceVertices3f[8]=0;

	SurfaceVertices3f[9]=0;
	SurfaceVertices3f[10]=1;
	SurfaceVertices3f[11]=0;

	//define normals
	SurfaceNormals3f[0]=0;
	SurfaceNormals3f[1]=0;
	SurfaceNormals3f[2]=1;

	SurfaceNormals3f[3]=0;
	SurfaceNormals3f[4]=0;
	SurfaceNormals3f[5]=1;

	SurfaceNormals3f[6]=0;
	SurfaceNormals3f[7]=0;
	SurfaceNormals3f[8]=1;

	SurfaceNormals3f[9]=0;
	SurfaceNormals3f[10]=0;
	SurfaceNormals3f[11]=1;

	//define colors
	SurfaceColors3f[0]=0;
	SurfaceColors3f[1]=0;
	SurfaceColors3f[2]=1;

	SurfaceColors3f[3]=1;
	SurfaceColors3f[4]=0;
	SurfaceColors3f[5]=1;

	SurfaceColors3f[6]=1;
	SurfaceColors3f[7]=1;
	SurfaceColors3f[8]=1;

	SurfaceColors3f[9]=0;
	SurfaceColors3f[10]=1;
	SurfaceColors3f[11]=1;

	//define tri indices
	SurfaceTriangles3ui[0]=0;
	SurfaceTriangles3ui[1]=1;
	SurfaceTriangles3ui[2]=2;

	SurfaceTriangles3ui[3]=2;
	SurfaceTriangles3ui[4]=3;
	SurfaceTriangles3ui[5]=0;

	//define texcoords
	SurfaceTexCoords2f[0]=0;
	SurfaceTexCoords2f[1]=0;

	SurfaceTexCoords2f[2]=1;
	SurfaceTexCoords2f[3]=0;
	
	SurfaceTexCoords2f[4]=1;
	SurfaceTexCoords2f[5]=1;

	SurfaceTexCoords2f[6]=0;
	SurfaceTexCoords2f[7]=1;
	*/
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
//This function is complete (!) and will draw the data in the Surface**** arrays.
//You do not need to modify this one.

	for (int t=0; t<SurfaceTriangles3ui.size();t+=3)
	{
		glBegin(GL_TRIANGLES);
		
		for (int triVertex=0; triVertex<3;++triVertex)
		{
			int vIndex=SurfaceTriangles3ui[t+triVertex];

			glTexCoord2fv(&(SurfaceTexCoords2f[2*vIndex]));
			glNormal3fv(&(SurfaceNormals3f[3*vIndex]));
			glColor3fv(&(SurfaceColors3f[3*vIndex]));
			glVertex3fv(&(SurfaceVertices3f[3*vIndex]));
		}
		glEnd();
	}
}

//this function loads the textures in the GPU memory
//the function is called once when the program starts
void initTexture()
{
	//Mmm... you understood the mechanism of moving the checkerboard, but can you produce something else?
	//1) load more textures using the file "brick.ppm" and put it in Texture[1], then "sand.ppm" and put it in Texture[2]
	//	btw. the wise Imp named Gerold, or short "Gimp" (it is actually a software), knows how to write PPM files (P6 - colored images)
	//  Hence, you can convert any image into a file that you can then use as a texture, but first stick to the given ones...
	//3) To make use of these textures, you first need a magic wand from the village (function) display().
	//	But be careful my young apprentice, you will need to traverse a desert...

	Texture.resize(3);
	Texture[0]=0;
	Texture[1]=0;
	Texture[2]=0;

	PPMImage image("C:/CGResources/checker.ppm");
	glGenTextures(1, &Texture[0]);

	glBindTexture(GL_TEXTURE_2D, Texture[0]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY, 
		GL_RGB, GL_UNSIGNED_BYTE, image.data);
	glBindTexture(GL_TEXTURE_2D, 0);


	PPMImage bricks("C:/CGResources/brick.ppm");
	glGenTextures(1, &Texture[1]);

	glBindTexture(GL_TEXTURE_2D, Texture[1]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bricks.sizeX, bricks.sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, bricks.data);
	glBindTexture(GL_TEXTURE_2D, 1);

	PPMImage sand("C:/CGResources/sand.ppm");
	glGenTextures(1, &Texture[2]);

	glBindTexture(GL_TEXTURE_2D, Texture[2]);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, sand.sizeX, sand.sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, sand.data);
	glBindTexture(GL_TEXTURE_2D, 2);
}













//take keyboard input into account
void keyboard(unsigned char key, int x, int y)
{
    printf("key %d pressed at %d,%d\n",key,x,y);
    fflush(stdout);

	if ((key>='1')&&(key<='9'))
	{
		DisplayMode= (DisplayModeType) (key-'0');
		return;
	}
	
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




















//DESERT//
//			     .    _    +     .  ______   .          .     '      .            '+
//  (      /|\      _   _|      \___   .   +    '    .         *
//    /\  ||||| .  | | |   | |      |       .    '                    .    '
// __||||_|||||____| |_|_____________\________________________________________
// . |||| |||||  /\   _____      _____  .   .       .             .       .
//  . \|`-'|||| ||||    __________            .
//     \__ |||| ||||      .          .     .     .        -            .   .
//  __    ||||`-'|||  .       .    __________
// .    . |||| ___/  ___________             .
// _   ___|||||__  _           .          _
//      _ `---'    .   .    .   _   .   .    .
// _  ^      .  -    .    -    .       -    .    .  .      -   .     .    -
//made by Bob Allison                                                   _   __




void display( )
{
	//The desert was extremely hot, if you have not collected a bottle of water on your journey, you die... and need to go back to drawQuad()
	//and work double shifts for the graphics course...

	//Otherwise you made it!!! Welcome to the display village.
	//The inhabitants of display village agree to give you their magic wand, but they want something in exchange...
	//They have struggled for a long time to fight against a group of orcs.
	//These attacked the poor villagers already several times.
	//In order to protect themselves they would like to build a city wall out of bricks.

	//Take a look at the case for TEXTURED_QUAD
	//1) try activating Texture[1] instead of Texture[0] to produce brick stones - a first test for your magic tool!
	//2) What happens if you bind the value 0?
		
	//It turns out that bricks alone are not enough. The villagers are too lazy to build the wall!!! 
	//This is devestating!
	//Do you remember the old spell on the drawQuad() scroll?... it created a checker board out of nothing...
	// Maybe it is possible to even build a mountain terrain to protect the city? 
	// After all, everybody knows: orcs are bad mountaineers... ;)

	//3) To start building the mountains, continue with initSurfaceMesh()
	// In that function you will have to fill the Surface*** arrays which will be used by drawSurface() below.
	// In order to see your results, do not forget to change to the SECOND display mode (press 2)
	// or change the initialization of the DisplayMode variable on the top of this file.
	// now hurry... the orcs are coming...

	//These two lines setup the light
	glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
	drawLight();

	switch( DisplayMode )
	{
	case TEXTURED_QUAD:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Texture[1]);
		drawQuad();		
		glBindTexture(GL_TEXTURE_2D,0);
		glDisable(GL_TEXTURE_2D);
		break;
	default:
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, Texture[2]);
		drawSurface();
		glBindTexture(GL_TEXTURE_2D,0);
		glDisable(GL_TEXTURE_2D);
		break;
	}
}


void combat()
{
	//Fantastic, you made it!!! The orcs cannot find their way to the village anymore!
	//Participate in the celebration at the bottom of trackball.h.
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

