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

#include "Entity.h"

//START READING HERE!!!


//////Predefined global variables

//Use the enum values to define different rendering modes 
//The mode is used by the function display and the mode is 
//chosen during execution with the keys 1-9
enum DisplayModeType {GAME=6};

DisplayModeType DisplayMode = GAME;

unsigned int W_fen = 800;  // screen width
unsigned int H_fen = 600;  // screen height

float LightPos[4] = {1,1,0.4,1};
std::vector<float> MeshVertices;
std::vector<unsigned int> MeshTriangles;

//Declare your own global variables here:
Entity character = Entity();
std::vector<Entity> enemies = {};
int glutElapsedTime = 0; //in ms


////////// Draw Functions 

//function to draw coordinate axes with a certain length (1 as a default)
void drawCoordSystem(float length=1)
{
	//draw simply colored axes
	
	//remember all states of the GPU
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//deactivate the lighting state
	glDisable(GL_LIGHTING);
	//draw axes
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(length,0,0);

		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,length,0);

		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,length);
	glEnd();
	
	//reset to previous state
	glPopAttrib();
}


void drawLight()
{
	// TODO LIGHT
}


void display( )
{
	//set the light to the right position
    glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
	drawLight();

	switch( DisplayMode )
	{
	case GAME:
		drawCoordSystem();
		character.draw();
		for (auto &enemy : enemies) {
			enemy.draw();
		}
	break;
	default:
		
		break;
	}
}


/**
 * Animation function, only put animation code here!
 * That is to say, code that updates positioning of drawables.
 */
void animate( )
{
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = currentTime - glutElapsedTime;
	glutElapsedTime = currentTime;

	character.animate(deltaTime);
	for (auto &enemy : enemies)
	{
		enemy.animate(deltaTime);
	}
}

void spawnEnemy(int value)
{
	Entity enemy = Entity();
	enemy.position = Vec3Df(3, (rand()%3-1), 0);
	enemy.movementDirection = Vec3Df(-1, 0, 0);
	enemy.color = Vec3Df(0, 0, 1);
	enemies.push_back(enemy);

	// Repeat this
	glutTimerFunc(1000, spawnEnemy, 0);
}

//take keyboard input into account
void keyboard(unsigned char key, int x, int y)
{
    printf("Keydown %d, cursor pos (%d,%d)\n",key,x,y);
    fflush(stdout);

	if ((key>='1')&&(key<='9'))
	{
		DisplayMode= (DisplayModeType) (key-'0');
		return;
	}
	
	switch (key)
    {
	case 'w':
		character.movementDirection = Vec3Df(0, 1, 0);
		break;
	case 'a':
		character.movementDirection = Vec3Df(-1, 0, 0);
		break;
	case 's':
		character.movementDirection = Vec3Df(0, -1, 0);
		break;
	case 'd':
		character.movementDirection = Vec3Df(1, 0, 0);
		break;
	case 27:     // touche ESC
        exit(0);
	case 'L':
		//turn lighting on
		glEnable(GL_LIGHTING);
		break;
	case 'l':
		//turn lighting off
		glDisable(GL_LIGHTING);
		break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    printf("Keyup %d, cursor pos (%d,%d)\n",key,x,y);
    fflush(stdout);

	if (key == 'w' || key == 'a' || key == 's' || key == 'd')
	{
		character.movementDirection = Vec3Df(0, 0, 0);
	}
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
bool loadMesh(const char * filename);
void init()
{
    glDisable( GL_LIGHTING );
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
	//loadMesh("David.obj");
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
    glutInitWindowPosition(20, 80);
    glutInitWindowSize(W_fen,H_fen);
    glutCreateWindow(argv[0]);

    init( );
	
    // Initialize viewpoint
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-4);
    tbInitTransform();     
    tbHelp();
         
	character.color = Vec3Df(1, 0, 0);
	character.position = Vec3Df(-2, 0, 0);

	// cablage des callback
    glutReshapeFunc(reshape);
	glutIgnoreKeyRepeat(1); 
    glutKeyboardFunc(keyboard); //call *once* on keydown
	glutKeyboardUpFunc(keyboardUp);
    glutDisplayFunc(displayInternal); 
	glutMouseFunc(tbMouseFunc);    // traqueboule utilise la souris
    glutMotionFunc(tbMotionFunc);  // traqueboule utilise la souris
    glutIdleFunc(animate);
	glutTimerFunc(1000, spawnEnemy, 0);

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

