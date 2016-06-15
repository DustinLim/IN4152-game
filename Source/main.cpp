#include "commonOpenGL.h"

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "trackball.h"
#include "argumentParser.h"
#include <stdio.h>
#include <string.h>
#include <memory>

#include "Entity.h"
#include "landscape.h"
#include "background.h"
#include "boss.h"
#include "mesh.h"
#include "grid.h"
#include "Vertex.h"


//Use the enum values to define different rendering modes 
//The mode is used by the function display and the mode is 
//chosen during execution with the keys 1-9
enum DisplayModeType {GAME=1, MESH=2};
DisplayModeType DisplayMode = GAME;

enum MouseModeType {MOUSE_MODE_SHOOTING=0, MOUSE_MODE_CAMERA=1};
MouseModeType MouseMode = MOUSE_MODE_SHOOTING;

unsigned int W_fen = 800;  // screen width
unsigned int H_fen = 600;  // screen height

Vec3Df topLeft, bottomRight;

float LightPos[4] = {1,1,0.4,1};

////////// Declare your own global variables here:
void calculateWorldSpaceViewportBounds();
void collisionDetection();
Vec3Df mouseToCharacterWorldPlane(int x, int y);

// NOTE: In C++, declaring "Object instance;" will instantly call the Object constructor!
// To do these forward declarations, use smart pointers (unique_ptr) instead.

Character character = Character();
std::vector<Enemy> enemies = {};
std::vector<Projectile> projectiles = {};
std::vector<Mesh >meshes = {};
int glutElapsedTime = 0; //in ms
bool keyPressed[256]; //keyboard buffer

unique_ptr<Background> background; //smart pointer needed
unique_ptr<Groundfloor> groundfloor;
std::vector<Ridge> mountains;
int numberOfRidges = 2;
bool toggleBoss = false;
Boss boss;

// Game timing constants (in ms)
const int firstEnemySpawnDelay = 3000;
const int enemyRespawnDelay = 1500;
const int bossSpawnDelay = 10000;

//TODO remove this again
int meshIndex = 0;


////////// Draw Functions 

//function to draw coordinate axes with a certain length (1 as a default)
void drawCoordSystem(float length=1)
{
	//draw simply colored axes
	
	//remember GPU state
    GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);

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
    if (lightingWasEnabled) {
        glEnable(GL_LIGHTING);
    }
}


void drawLight()
{
	//remember all states of the GPU
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//deactivate the lighting state
	glDisable(GL_LIGHTING);
	//yellow sphere at light position
	glColor3f(1, 1, 0);
	glPushMatrix();
	glTranslatef(LightPos[0], LightPos[1], LightPos[2]);
	glutSolidSphere(0.1, 6, 6);
	glPopMatrix();

	//reset to previous state
	glPopAttrib();
}


void display( )
{
	switch( DisplayMode )
	{
	case GAME:
	{
		glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
		drawLight();
		drawCoordSystem();

		// Note that drawing order has consequences for 'transparancy'
		background->draw();
		for (int i = 0; i < numberOfRidges; i++)
		{
			mountains[i].draw();
		}
		groundfloor->draw();

		for (auto &enemy : enemies) {
			enemy.draw();
		}
		for (auto &projectile : projectiles) {
			projectile.draw();
		}
		
		if (toggleBoss)
			boss.drawBoss();

        character.draw();
		
		break;
	}
    case MESH:
    {
        glEnable(GL_LIGHTING);
        glPushMatrix();
		//David:
        //glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        //glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		//hoofd
		glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        meshes[meshIndex].drawSmooth();
        glPopMatrix();
        glDisable(GL_LIGHTING);
        break;
    }
	default:
		break;
	}
}

bool isHit(Entity ent1, Entity ent2) {
	std::vector<Vec3Df> bb1 = ent1.getBoundingBox();
	std::vector<Vec3Df> bb2 = ent2.getBoundingBox();

	bool xAxis = (bb1[0][0] > bb2[1][0] || bb1[1][0] < bb2[0][0]);
	bool yAxis = (bb1[0][1] > bb2[1][1] || bb1[1][1] < bb2[0][1]);
	bool anyHit = xAxis || yAxis;

	return !anyHit;
}

/**
 * Animation function, only put animation code here!
 * That is to say, code that updates positioning of drawables.
 */
void animate( )
{
	for (int i = 0; i < numberOfRidges; i++)
	{
		mountains[i].move();
	}
	background->move();
	groundfloor->move();

	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	int deltaTime = currentTime - glutElapsedTime;
	glutElapsedTime = currentTime;

	for (auto &enemy : enemies) {
		enemy.animate(deltaTime);
	}
	for (auto &projectile : projectiles) {
		projectile.animate(deltaTime);
	}

	character.animate(deltaTime);
	character.position[0] = std::fmax(character.position[0], topLeft[0] + (character.width / 2.0f) * character.scale);
	character.position[0] = std::fmin(character.position[0], bottomRight[0] - (character.width / 2.0f) * character.scale);

	character.position[1] = std::fmin(character.position[1], topLeft[1] - (character.height / 2.0f) * character.scale);
	character.position[1] = std::fmax(character.position[1], bottomRight[1] + (character.height / 2.0f) * character.scale);

	if (toggleBoss)
		boss.animate(deltaTime);

	collisionDetection();
}

void collisionDetection() {
	//Check if any bullets hit
	for (std::vector<Projectile>::iterator projectile = projectiles.begin(); projectile != projectiles.end();) {
		bool broken = false;
		for (std::vector<Enemy>::iterator enemy = enemies.begin(); enemy != enemies.end();) {
			if (isHit((*projectile), (*enemy))) {
				enemy = enemies.erase(enemy);
				projectile = projectiles.erase(projectile);
				broken = true;
				break;
			}
			else {
				++enemy;
			}
		}
		if (!broken) {
			projectile++;
		}
	}

	//Check if anything went outside the viewport
	for (std::vector<Enemy>::iterator enemy = enemies.begin(); enemy != enemies.end();) {
		if ((*enemy).getBoundingBox()[1][0] < topLeft[0]) {
			enemy = enemies.erase(enemy);
		}
		else {
			++enemy;
		}
	}
	Entity viewport = Entity();
	viewport.height = H_fen;
	viewport.width = W_fen;
	for (std::vector<Projectile>::iterator projectile = projectiles.begin(); projectile != projectiles.end();) {
		if (!isHit((*projectile), viewport)) {
			projectile = projectiles.erase(projectile);
		}
		else {
			++projectile;
		}
	}

	//Check if the player didn't hit an enemy
	for (std::vector<Enemy>::iterator enemy = enemies.begin(); enemy != enemies.end();) {
		if (isHit(character, (*enemy))) {
			enemy = enemies.erase(enemy);
			// Do some logic here
		}
		else {
			++enemy;
		}
	}
}

// Method parameter is required to be registered by glutTimerFunc()
void spawnEnemy(int unusedValue)
{
	if (!toggleBoss)
	{
		Enemy enemy = Enemy();
		enemy.position = Vec3Df(3, (rand() % 3 - 1), 0);
		enemy.movementDirection = Vec3Df(-1, 0.2, 0);
		enemies.push_back(enemy);

		// Repeat this
		glutTimerFunc(enemyRespawnDelay, spawnEnemy, 0);
	}
}

void spawnBoss(int unusedValue)
{
	boss = Boss(Vec3Df(6, -1, -2), -1, 0.5);
	boss.setTarget(&character.position);
	toggleBoss = true;
}

Projectile spawnProjectile(Vec3Df direction)
{
	Vec3Df spawnPos = character.getAngleRefPos();
	direction.normalize();
	spawnPos += direction * character.getArmRadius();

    Projectile projectile = Projectile(spawnPos, direction);
	projectile.movementSpeed = 3.0;
	projectile.width = 0.125;
	projectile.height = 0.125;

	projectiles.push_back(projectile);
	return projectile;
}

void updateCharacterMovementDirection()
{
    // Update character movement
    Vec3Df direction = Vec3Df(0, 0, 0);
    if (keyPressed['w']) { direction += Vec3Df(0, 1, 0); }
    if (keyPressed['a']) { direction += Vec3Df(-1, 0, 0); }
    if (keyPressed['s']) { direction += Vec3Df(0, -1, 0); }
    if (keyPressed['d']) { direction += Vec3Df(1, 0, 0); }
    character.movementDirection = direction;
}

//take keyboard input into account
void keyboard(unsigned char key, int x, int y)
{
    //printf("Keydown %d, cursor pos (%d,%d)\n",key,x,y);
    //fflush(stdout);
    keyPressed[key] = true;

	if ((key>='1')&&(key<='9'))
	{
		DisplayMode= (DisplayModeType) (key-'0');
		return;
	}
    
    updateCharacterMovementDirection();
    
	switch (key)
    {
    case 'm': {
        if (MouseMode == MOUSE_MODE_SHOOTING) {
            MouseMode = MOUSE_MODE_CAMERA;
        }
        else if (MouseMode == MOUSE_MODE_CAMERA) {
            MouseMode = MOUSE_MODE_SHOOTING;
        }
        break;
    }
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
	// MOVING THE LIGHT IN THE X,Y,Z DIRECTION -> We should do something with shadows in a later stadium.
	case 'f':
		LightPos[0] -= 0.1f;
		break;
	case 'h':
		LightPos[0] += 0.1f;
		break;
	case 't':
		LightPos[1] += 0.1f;
		break;
	case 'g':
		LightPos[1] -= 0.1f;
		break;
	case 'r':
		LightPos[2] += 0.1f;
		break;
	case 'y':
		LightPos[2] -= 0.1f;
		break;	
	case 'b':
		spawnBoss(0);
		break;
	case 'j':
		if (boss.position[0] <= 0)
			boss.setDestination(Vec3Df(-2, -1, -0.5), 1);
		else
			boss.setDestination(Vec3Df(0, -1, -1), 1);
		break;
	case 'k':
		if (boss.position[0] >= 0)
			boss.setDestination(Vec3Df(2, -1, 0), 1);
		else
			boss.setDestination(Vec3Df(0, -1, 0.5), 1);
		break;
	case '+':
		meshIndex = ++meshIndex % meshes.size();
		break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    //printf("Keyup %d, cursor pos (%d,%d)\n",key,x,y);
    //fflush(stdout);
    keyPressed[key] = false;

    updateCharacterMovementDirection();
}

// Returns for a given cursor coordinate, the intersection points it has
// when projected to the near and far clipping planes of the view frustum.
// NOTE: inspired by http://stackoverflow.com/a/18247608
void calculateMouseRay(int x, int y, Vec3Df *nearPoint, Vec3Df *farPoint)
{
    // Fetch transformation matrices
    GLdouble modelview[16]; //world coordinates -> eye coordinates
    GLdouble projection[16]; //eye coordinates -> clip coordinates
    GLint viewport[4]; //normalized device coordinates -> window coordinates
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    // OpenGL uses a bottom-left origin,
    // however the click coordinates we receive use top-left origin..
    GLdouble winX = (GLdouble)x;
    GLdouble winY = viewport[3] - (GLdouble)y;

    // Get intersection points for the 'near' and 'far' clipping planes
    GLdouble nearX, nearY, nearZ, farX, farY, farZ;
    gluUnProject(winX, winY, 0.0,
                 modelview, projection, viewport,
                 &nearX, &nearY, &nearZ);
    gluUnProject(winX, winY, 1.0,
                 modelview, projection, viewport,
                 &farX, &farY, &farZ);

    *nearPoint = Vec3Df(nearX, nearY, nearZ);
    *farPoint = Vec3Df(farX, farY, farZ);
}

Vec3Df mouseToCharacterWorldPlane(int x, int y) {
	float xD = topLeft[0] + (((float)x) / W_fen) * (bottomRight[0] - topLeft[0]);
	float yD = topLeft[1] + (((float)y) / H_fen) * (bottomRight[1] - topLeft[1]);
	return Vec3Df(xD, yD, character.position[2]);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN &&
        MouseMode == MOUSE_MODE_SHOOTING)
    {
		Vec3Df shootingDirection = mouseToCharacterWorldPlane(x, y) - character.getAngleRefPos();
		spawnProjectile(shootingDirection);
		character.updateArmAngle(mouseToCharacterWorldPlane(x, y));
    }
    else if (MouseMode == MOUSE_MODE_CAMERA)
    {
        // Pass this event to trackball.h
        tbMouseFunc(button, state, x, y);
		calculateWorldSpaceViewportBounds();
    }
}

void mouseMotion(int x, int y) {
	if (MouseMode == MOUSE_MODE_SHOOTING)
	{
		character.updateArmAngle(mouseToCharacterWorldPlane(x, y));
	}
	else if (MouseMode == MOUSE_MODE_CAMERA)
	{
		// Pass this event to trackball.h
		tbMotionFunc(x, y);
		calculateWorldSpaceViewportBounds();
	}
}

void mousePassiveMotion(int x, int y) {
	if (MouseMode == MOUSE_MODE_SHOOTING)
	{
		character.updateArmAngle(mouseToCharacterWorldPlane(x, y));
	}
	else if (MouseMode == MOUSE_MODE_CAMERA)
	{

	}
}

void calculateWorldSpaceViewportBounds() {
	Vec3Df nearPoint, farPoint;
	calculateMouseRay(0, 0, &nearPoint, &farPoint);
	Vec3Df ray = nearPoint - farPoint;
	float fraction = (character.position[2] - farPoint[2]) / ray[2];
	topLeft = farPoint +(ray * fraction);

	calculateMouseRay(W_fen, H_fen, &nearPoint, &farPoint);
	ray = nearPoint - farPoint;
	fraction = (character.position[2] - farPoint[2]) / ray[2];
	bottomRight = farPoint +(ray * fraction);
}


void displayInternal(void);
void reshape(int w, int h);
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

	background.reset(new Background());
	groundfloor.reset(new Groundfloor());
	mountains.resize(numberOfRidges);
	mountains[0] = Ridge(1, 50, 10, -3, 0.005f, -4, "./Textures/sand.ppm");
	mountains[1] = Ridge(2, 50, 10, -3, 0.0075f, -3, "./Textures/sand.ppm");

	//TODO change mesh to correct object.
	printf("Loading Mesh\n");
	Mesh mesh = Mesh();
	mesh.loadMesh("./Models/hoofd.obj");
	meshes.push_back(mesh);
	printf("Creating Grid, 16\n");
	meshes.push_back(Grid::getReduxMesh(mesh, 16));
	printf("Creating Grid, 8\n");
	meshes.push_back(Grid::getReduxMesh(mesh, 8));
	printf("Creating Grid, 4\n");
	meshes.push_back(Grid::getReduxMesh(mesh, 4));

	character.initTexture();
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

	// Windows only exposes OpenGL 1.1 functions.
	// To call more modern functions, we need to load GLEW.
	#if defined(_WIN32)
		GLenum err = glewInit();
		(GLEW_OK != err) ? printf("GLEW init failed!\n") : printf("GLEW init complete\n");
	#endif

    init( );
	
    // Initialize viewpoint
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-4);
    tbInitTransform();     
    tbHelp();

	calculateWorldSpaceViewportBounds();

	// cablage des callback
    glutReshapeFunc(reshape);
	glutIgnoreKeyRepeat(1); 
    glutKeyboardFunc(keyboard); //call *once* on keydown
	glutKeyboardUpFunc(keyboardUp);
    glutDisplayFunc(displayInternal); 
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);  // traqueboule utilise la souris
	glutPassiveMotionFunc(mousePassiveMotion);
    glutIdleFunc(animate);
	glutTimerFunc(firstEnemySpawnDelay, spawnEnemy, 0);
	glutTimerFunc(bossSpawnDelay, spawnBoss, 0);

    // lancement de la boucle principale
    glutMainLoop();
    
    return 0;  // instruction jamais exécutée
}

/**
 * This is the display callback function.
 * It is called when GLUT determines that the normal plane for the window needs to be redisplayed.
 * The entire normal plane region should be redisplayed in response to the callback.
 */
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
	W_fen = w;
	H_fen = h;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho (-1.1, 1.1, -1.1,1.1, -1000.0, 1000.0);
    gluPerspective (50, (float)w/h, 1, 10);
    glMatrixMode(GL_MODELVIEW);

	calculateWorldSpaceViewportBounds();
}

