/*******************************************************************
		   Hierarchical Multi-Part Model Example
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "QuadMesh.h"

const int vWidth = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

// Note how everything depends on robot body dimensions so that can scale entire robot proportionately
// just by changing robot body scale
float robotBodyWidth = 12.0;
float robotBodyLength = 10.0;
float robotBodyDepth = 8.0;
float headWidth = 0.3 * robotBodyWidth;
float headLength = headWidth;
float headDepth = headWidth;
float upperArmLength = robotBodyLength;
float upperArmWidth = 0.2 * robotBodyWidth;
float gunWidth = upperArmWidth;
float gunLength = upperArmLength / 2.0;
float gunDepth = upperArmWidth;
float stanchionLength = robotBodyLength;
float stanchionRadius = 0.1 * robotBodyDepth;
float baseWidth = 1.5 * robotBodyWidth;
float baseLength = 0.4 * stanchionLength;

// Control Robot body rotation on base
float robotAngle = 0.0;

// Control arm rotation
float shoulderAngle = -40.0;
float gunAngle = -25.0;

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat robotBody_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat robotBody_mat_specular[] = { 0.45f,0.55f,0.45f,1.0f };
GLfloat robotBody_mat_diffuse[] = { 0.8f, 0.7f, 0.5f, 1.0f };
GLfloat robotBody_mat_shininess[] = { 32.0F };


GLfloat robotArm_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat robotArm_mat_diffuse[] = { 0.4f, 0.5f, 0.2f, 1.0f };
GLfloat robotArm_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat robotArm_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };   
GLfloat gun_mat_diffuse[] = { 0.1f, 0.1f, 0.1f, 0.01f };   
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };  
GLfloat gun_mat_shininess[] = { 100.0F };

GLfloat robotLowerBody_mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat robotLowerBody_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat robotLowerBody_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat robotLowerBody_mat_shininess[] = { 76.8F };

GLfloat green_mat_ambient[] = { 0.0, 0.5, 0.0, 1.0 };
GLfloat green_mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
GLfloat green_mat_diffuse[] = { 0.0, 0.6, 0.0, 1.0 };
GLfloat green_mat_shininess[] = { 30.0 };

GLfloat beige_mat_ambient[] = { 0.8, 0.7, 0.5, 1.0 };
GLfloat beige_mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
GLfloat beige_mat_diffuse[] = { 0.9, 0.8, 0.6, 1.0 };
GLfloat beige_mat_shininess[] = { 30.0 };

GLfloat light_brown_mat_ambient[] = { 0.6, 0.3, 0.1, 1.0 };
GLfloat light_brown_mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
GLfloat light_brown_mat_diffuse[] = { 0.7, 0.4, 0.2, 1.0 };
GLfloat light_brown_mat_shininess[] = { 30.0 };

GLfloat red_orange_ambient[] = { 0.8f, 0.2f, 0.0f, 1.0f };
GLfloat red_orange_diffuse[] = { 0.9f, 0.3f, 0.1f, 1.0f };
GLfloat red_orange_specular[] = { 0.8f, 0.2f, 0.1f, 1.0f };
GLfloat red_orange_shininess[] = { 32.0F };

// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


// Mouse button
int currentButton;

// A flat open mesh
QuadMesh* groundMesh = NULL;

// Structure defining a bounding box, currently unused
typedef struct BoundingBox {
	VECTOR3D min;
	VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void drawRobot();
void drawBody();
void drawHead();
void drawLowerBody();
void drawLeftArm();
void drawRightArm();

int main(int argc, char** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("3D Hierarchical Example");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// Other initializatuion
	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-16.0f, 0.0f, 16.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 32.0);
	groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(35.0, 20.0, 35.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Draw Robot

	// Apply modelling transformations M to move robot
	// Current transformation matrix is set to IV, where I is identity matrix
	// CTM = IV
	drawRobot();

	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -20.0, 0.0);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}

void drawRobot()
{
	glPushMatrix();
	// spin robot on base. 
	glRotatef(robotAngle, 0.0, 1.0, 0.0);

	drawBody();
	drawHead();
	drawLeftArm();
	drawRightArm();
	glPopMatrix();

	// don't want to spin fixed base in this example
	drawLowerBody();

	glPopMatrix();
}

void drawBody()
{
	// Top Part (Beige, wide)
	glPushMatrix();
	// Set material properties for the beige top part
	glMaterialfv(GL_FRONT, GL_AMBIENT, beige_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, beige_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, beige_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, beige_mat_shininess);

	// Position and scale the top part
	glTranslatef(0.0, 0.5 * robotBodyLength, 0.0);  // Top part is at the top
	glScalef(robotBodyWidth, robotBodyLength / 3.0, robotBodyDepth);  // Wide part, 1/3 of total height
	glutSolidCube(1.0);
	glPopMatrix();

	// Middle Part (Black, thin and long)
	glPushMatrix();
	// Set material properties for the black middle part
	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	// Position and scale the middle part
	glTranslatef(0.0, 0.0, 0.0);  // Middle part stays in the center
	glScalef(0.4 * robotBodyWidth, robotBodyLength / 2.0, 0.4 * robotBodyDepth);  // Thin and long part
	glutSolidCube(1.0);
	glPopMatrix();

	// Bottom Part (Green, thinner and between the legs)
	glPushMatrix();
	// Set material properties for the green bottom part
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	// Position and scale the bottom part
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);  // Bottom part is at the bottom
	glScalef(0.8 * robotBodyWidth, robotBodyLength / 3.0, 0.8 * robotBodyDepth);  // Thinner and smaller to fit between the legs
	glutSolidCube(1.0);
	glPopMatrix();
}


void drawHead()
{
	// Set robot material properties for the head
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	// Position head with respect to parent (body)
	glTranslatef(0, 0.5 * robotBodyLength + 1.0 * headLength, 0); // Move head even higher

	// Build the head
	glPushMatrix();
	glScalef(0.4 * robotBodyWidth, 0.4 * robotBodyWidth, 0.4 * robotBodyWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawLowerBody()
{
	// Left leg
	glPushMatrix();
	// Move the leg lower to connect better to the body but slightly higher to avoid going into the ground
	glTranslatef(0.5 * robotBodyWidth, -0.7 * robotBodyLength, 0.0); // Move the leg up slightly to avoid clipping with the ground

	// First (upper) part of the leg - beige segment
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, beige_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, beige_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, beige_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, beige_mat_shininess);

	// Rotate forward to create the first angle in the zig-zag (front-to-back)
	glRotatef(15, 1.0, 0.0, 0.0); // Rotate around the X-axis for front-to-back zig-zag effect
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth); // Make the beige part longer and move lower
	glutSolidCube(1.0);
	glPopMatrix();

	// First (upper) green part of the leg
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0); // Move down for the next segment
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	// Rotate backward for the next zig-zag angle
	glRotatef(-15, 1.0, 0.0, 0.0); // Rotate in the opposite direction around the X-axis
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth); // Increase the green part length to match beige
	glutSolidCube(1.0);
	glPopMatrix();

	// Second (lower) green part of the leg
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0); // Move down for the second green segment
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	// Rotate forward slightly to maintain the zig-zag pattern
	glRotatef(15, 1.0, 0.0, 0.0); // Small rotation for final zig-zag
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth); // Increase the second green part length
	glutSolidCube(1.0);
	glPopMatrix();

	// Foot (light brown)
	glTranslatef(0.0, -0.25 * robotBodyLength, 0.0); // Move down for the foot, ensuring it's above the ground
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	// Foot should now be longer front-to-back
	glScalef(0.4 * robotBodyDepth, 0.1 * robotBodyLength, 0.6 * robotBodyWidth); // Adjust foot size: longer Z (front-to-back)
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix(); // End left leg

	// Right leg (copy of the left leg but positioned on the other side)
	glPushMatrix();
	// Move the leg lower to connect better to the body but slightly higher to avoid going into the ground
	glTranslatef(-0.5 * robotBodyWidth, -0.7 * robotBodyLength, 0.0); // Move the leg up slightly to avoid clipping with the ground

	// First (upper) part of the leg - beige segment
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, beige_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, beige_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, beige_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, beige_mat_shininess);

	// Rotate forward to create the first angle in the zig-zag (front-to-back)
	glRotatef(15, 1.0, 0.0, 0.0); // Rotate around the X-axis for front-to-back zig-zag effect
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth); // Make the beige part longer and move lower
	glutSolidCube(1.0);
	glPopMatrix();

	// First (upper) green part of the leg
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0); // Move down for the next segment
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	// Rotate backward for the next zig-zag angle
	glRotatef(-15, 1.0, 0.0, 0.0); // Rotate in the opposite direction around the X-axis
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth); // Increase the green part length to match beige
	glutSolidCube(1.0);
	glPopMatrix();

	// Second (lower) green part of the leg
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0); // Move down for the second green segment
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	// Rotate forward slightly to maintain the zig-zag pattern
	glRotatef(15, 1.0, 0.0, 0.0); // Small rotation for final zig-zag
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth); // Increase the second green part length
	glutSolidCube(1.0);
	glPopMatrix();

	// Foot (light brown)
	glTranslatef(0.0, -0.25 * robotBodyLength, 0.0); // Move down for the foot, ensuring it's above the ground
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	// Foot should now be longer front-to-back
	glScalef(0.4 * robotBodyDepth, 0.1 * robotBodyLength, 0.6 * robotBodyWidth); // Adjust foot size: longer Z (front-to-back)
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix(); // End right leg
}


void drawLeftArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(0.5 * robotBodyWidth + 0.5 * upperArmWidth, 0, 0.0); // this will be done last

	// build arm
	glPushMatrix();
	glScalef(upperArmWidth, upperArmLength, upperArmWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawRightArm()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();

	// Rotate arm at shoulder
	glTranslatef(-(0.5 * robotBodyWidth + 0.5 * upperArmWidth), 0.5 * upperArmLength, 0.0);
	glRotatef(shoulderAngle, 1.0, 0.0, 0.0);
	glTranslatef((0.5 * robotBodyWidth + 0.5 * upperArmWidth), -0.5 * upperArmLength, 0.0);

	// Position arm and gun with respect to parent body
	glTranslatef(-(0.5 * robotBodyWidth + 0.5 * upperArmWidth), 0, 0.0);

	// build arm
	glPushMatrix();
	glScalef(upperArmWidth, upperArmLength, upperArmWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Gun
	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	glPushMatrix();
	// rotate gun
	glTranslatef(-(0.5 * robotBodyWidth + 0.5 * upperArmWidth), -(0.5 * upperArmLength), 0.0);
	glRotatef(gunAngle, 1.0, 0.0, 0.0);
	glTranslatef((0.5 * robotBodyWidth + 0.5 * upperArmWidth), (0.5 * upperArmLength), 0.0);

	// Position gun with respect to parent arm 
	glTranslatef(0, -(0.5 * upperArmLength + 0.5 * gunLength), 0.0);

	// build simplified gun body (cube)
	glPushMatrix();
	glScalef(gunWidth, gunLength, gunDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	// build simplified gun barrel (cylinder)
	glPushMatrix();
	// Position the cylinder in front of the cube
	glTranslatef(0.0, -0.5 * gunLength + 0.5, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0); // Align the cylinder with the correct axis
	GLUquadric* quad = gluNewQuadric();
	gluCylinder(quad, 1.5, 1.5, 5.0, 40, 20); // A long, thin cylinder for the barrel
	glPopMatrix();

	// Add the red/orange rectangle inside the cylinder
	glPushMatrix();
	// Set the color for the red/orange rectangle
	GLfloat red_orange_ambient[] = { 0.8f, 0.2f, 0.0f, 1.0f };
	GLfloat red_orange_diffuse[] = { 0.9f, 0.3f, 0.1f, 1.0f };
	GLfloat red_orange_specular[] = { 0.8f, 0.2f, 0.1f, 1.0f };
	GLfloat red_orange_shininess[] = { 32.0F };

	glMaterialfv(GL_FRONT, GL_AMBIENT, red_orange_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red_orange_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_orange_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, red_orange_shininess);

	// Position and size the red/orange rectangle inside the cylinder
	glTranslatef(0.0, -2.5 * gunLength, 0.0); // Adjust position inside cylinder
	glScalef(gunWidth * 0.5, gunLength * 0.1, gunDepth * 0.5); // Size the rectangle
	glutSolidCube(1.0); // Draw the red/orange rectangle
	glPopMatrix();

	// Add a magazine to the gun
	glPushMatrix();
	// Position the magazine below the gun body
	glTranslatef(0.0, -gunLength - 1.0, 0.0); // Move below the gun
	glScalef(gunWidth * 0.8, gunLength * 0.4, gunDepth * 0.8); // Adjust size of the magazine
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
	glPopMatrix();
}


void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 100.0); // Increased far clipping plane from 40.0 to 100.0

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 35) looking at the origin
	gluLookAt(0.0, 6.0, 35.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}


bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 't':

		break;
	case 'r':
		robotAngle += 2.0;
		break;
	case 'R':
		robotAngle -= 2.0;
		break;
	case 'a':
		shoulderAngle += 2.0;
		break;
	case 'A':
		shoulderAngle -= 2.0;
		break;
	case 'g':
		gunAngle += 2.0;
		break;
	case 'G':
		gunAngle -= 2.0;
		break;
	case 's':
		glutTimerFunc(10, animationHandler, 0);
		break;
	case 'S':
		stop = true;
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


void animationHandler(int param)
{
	if (!stop)
	{
		shoulderAngle += 1.0;
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}



// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{

	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

