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
float legAngle = 0.0f;        // Controls the angle of the leg during stepping
bool spinCannon = false;      // Flag to control cannon spinning
float cannonSpinAngle = 0.0f; // Angle for cannon spinning

// Joint angles for walking
float hipAngleLeft = 0.0f;  // Angle for left hip joint
float kneeAngleLeft = 0.0f; // Angle for left knee joint
float ankleAngleLeft = 0.0f; // Angle for left ankle joint
float hipAngleRight = 0.0f;  // Angle for right hip joint
float kneeAngleRight = 0.0f; // Angle for right knee joint
float ankleAngleRight = 0.0f; // Angle for right ankle joint
float lowerLegAngleLeft = 0.0f;  // Angle for rotating lower part of the left leg
float lowerLegAngleRight = 0.0f; // Angle for rotating lower part of the right leg

// Flag to control walking state
bool walking = false;
bool stepBackwards = false;  // Controls whether the leg is stepping forward or backward
bool walkingForward = true;   // Track whether we're walking forward
int selectedJoint = 0; // 0 for none, 1 for knee, 2 for hip, 3 for body
int cameraView = 0; // 0 = default, 1 = front, 2 = side, 3 = top-down

// Control Robot body rotation on base
float robotAngle = 0.0;
float neckAngle = 0.0f;   // Neck rotation

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

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Change camera position based on selected view
	switch (cameraView) {
	case 0: // Default (isometric view)
		gluLookAt(35.0, 20.0, 35.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	case 1: // Front view
		gluLookAt(0.0, 15.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	case 2: // Side view
		gluLookAt(50.0, 15.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	case 3: // Top-down view
		gluLookAt(0.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
		break;
	}

	// Draw Robot
	drawRobot();

	// Draw ground (lowered further)
	glPushMatrix();
	glTranslatef(0.0, -25.0, 0.0);  // Lowered the ground to -30.0
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}


void drawRobot()
{
	// 1. Draw the lower body (green part and legs) separately with no rotation
	glPushMatrix();  // Save current matrix state
	drawLowerBody();  // Draw the lower body first without any transformations
	glPopMatrix();  // Restore the matrix

	// 2. Draw the upper body with rotation
	glPushMatrix();  // Save current matrix state

	// Rotate only the upper body parts
	glRotatef(robotAngle, 0.0, 1.0, 0.0);  // Apply rotation for the upper body

	// Draw the upper body components: torso, head, arms
	drawBody();      // Beige, black parts (upper torso)
	drawHead();      // Head
	drawLeftArm();   // Left arm
	drawRightArm();  // Right arm

	glPopMatrix();  // End upper body rotation, restore the matrix
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

	// No longer drawing the green bottom part here; it will now be drawn as part of the lower body.
}

void drawHead()
{
	// Set robot material properties for the head (white part)
	GLfloat white_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat white_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat white_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat white_shininess[] = { 50.0f };

	// Darker grey for the grey parts
	GLfloat dark_grey_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat dark_grey_diffuse[] = { 0.2f, 0.2f, 0.2f, 0.1f };
	GLfloat dark_grey_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat dark_grey_shininess[] = { 50.0f };

	GLfloat blue_ambient[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat blue_diffuse[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GLfloat blue_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat blue_shininess[] = { 30.0f };

	glPushMatrix();
	// Apply neck rotation
	glRotatef(neckAngle, 0.0, 1.0, 0.0);  // Rotate neck along Y-axis
	glTranslatef(0, 0.5 * robotBodyLength + 1.0 * headLength, 0); // Move head above the body

	// Draw the head (white part)
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, white_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, white_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, white_shininess);

	glScalef(0.4 * robotBodyWidth, 0.4 * robotBodyWidth, 0.4 * robotBodyWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw the green sides
	GLfloat green_ambient[] = { 0.0, 0.5, 0.0, 1.0 };
	GLfloat green_diffuse[] = { 0.0, 0.6, 0.0, 1.0 };
	GLfloat green_specular[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat green_shininess[] = { 30.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, green_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_shininess);

	// Draw left side of the head
	glPushMatrix();
	glTranslatef(-0.2 * robotBodyWidth, 0, 0);  // Move to left
	glScalef(0.01 * robotBodyWidth, 0.4 * robotBodyWidth, 0.4 * robotBodyWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw right side of the head
	glPushMatrix();
	glTranslatef(0.2 * robotBodyWidth, 0, 0);  // Move to right
	glScalef(0.01 * robotBodyWidth, 0.4 * robotBodyWidth, 0.4 * robotBodyWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Grey stripe positioned just behind the blue and in front of the white, move very slightly down
	glMaterialfv(GL_FRONT, GL_AMBIENT, dark_grey_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dark_grey_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, dark_grey_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, dark_grey_shininess);

	// Move the front grey part very slightly down
	glPushMatrix();
	glTranslatef(0.0, 0.06 * robotBodyWidth, 0.20 * robotBodyWidth);  // Very slight downward adjustment
	glScalef(0.12 * robotBodyWidth, 0.3 * robotBodyWidth, 0.03 * robotBodyWidth);  // Taller and wider
	glutSolidCube(1.0);
	glPopMatrix();

	// Add grey part to the top of the head
	glPushMatrix();
	glTranslatef(0.0, 0.2 * robotBodyWidth, 0.01 * robotBodyWidth);  // Small forward adjustment
	glScalef(0.12 * robotBodyWidth, 0.02 * robotBodyWidth, 0.42 * robotBodyWidth);  // Long and thin grey stripe on top
	glutSolidCube(1.0);
	glPopMatrix();

	// Blue eye (upright visor-like stripe)
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, blue_shininess);

	// Position the blue visor/eye on the front
	glTranslatef(0.0, 0.1 * robotBodyWidth, 0.22 * robotBodyWidth);  // Position it on the front
	glScalef(0.05 * robotBodyWidth, 0.2 * robotBodyWidth, 0.02 * robotBodyWidth);  // Flip dimensions to make it upright
	glutSolidCube(1.0); // Blue eye
	glPopMatrix();

	glPopMatrix();  // End head drawing
}

void drawLowerBody()
{
	// Lower body green section (stationary part)
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	// Position the green section (between the legs) under the upper body but not affected by rotation
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);  // Move to where the green section is positioned
	glScalef(0.8 * robotBodyWidth, robotBodyLength / 3.0, 0.8 * robotBodyDepth);  // Scale to match the body proportions
	glutSolidCube(1.0);  // Draw the green section (stationary)
	glPopMatrix();

	// Left leg
	glPushMatrix();
	// Move the leg lower to connect better to the body
	glTranslatef(0.5 * robotBodyWidth, -0.7 * robotBodyLength, 0.0); // Adjust leg height

	// Hip rotation for walking
	glRotatef(hipAngleLeft, 1.0, 0.0, 0.0); // Rotate hip joint

	// Upper leg segment - beige
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, beige_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, beige_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, beige_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, beige_mat_shininess);

	// Rotate and scale the upper leg
	glRotatef(-15, 1.0, 0.0, 0.0); // Slight rotation for a zig-zag pose
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End upper leg segment

	// Add kneecap before moving down for the lower leg
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	// Adjust kneecap placement (slightly forward on the Z-axis)
	glTranslatef(0.0, -0.25 * robotBodyLength, 0.10 * robotBodyDepth);  // Move kneecap forward slightly
	glScalef(0.25 * robotBodyWidth, 0.1 * robotBodyLength, 0.25 * robotBodyDepth);  // Scale the kneecap
	glutSolidCube(1.0);  // Draw kneecap
	glPopMatrix();

	// Move down for knee joint
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);
	// Knee rotation
	glRotatef(kneeAngleLeft, 1.0, 0.0, 0.0); // Rotate knee

	// Lower leg segment - green
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	glRotatef(15, 1.0, 0.0, 0.0); // Rotate to maintain zig-zag pose
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End lower leg segment

	// New kneecap between the two green parts
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	// Translate to position the kneecap between the two green parts
	glTranslatef(0.0, -0.25 * robotBodyLength, 0.0); // Adjust based on the spacing between the two green parts
	glScalef(0.25 * robotBodyWidth, 0.1 * robotBodyLength, 0.25 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Move down for the second (third part) green leg
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);
	// Lower leg rotation
	glRotatef(lowerLegAngleLeft, 1.0, 0.0, 0.0);

	// Second green part - same size as the previous green part
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	glRotatef(-15, 1.0, 0.0, 0.0); // Continue zig-zag pose
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End second green part

	// Move down for ankle (adjusted to move feet up)
	glTranslatef(0.0, -0.3 * robotBodyLength, 0.0);  // Reduced from -0.5 to -0.3 for closer connection

	// Ankle rotation
	glRotatef(ankleAngleLeft, 1.0, 0.0, 0.0); // Rotate ankle

	// Foot segment - light brown
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	// Foot base
	glPushMatrix();
	glScalef(0.4 * robotBodyDepth, 0.1 * robotBodyLength, 0.6 * robotBodyWidth); // Foot dimensions
	glutSolidCube(1.0);
	glPopMatrix(); // End foot base

	// Add two dents in front of the foot
	// First front dent
	glPushMatrix();
	glTranslatef(-0.15 * robotBodyDepth, 0.0, 0.4 * robotBodyWidth); // Move to the front-left
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	// Second front dent
	glPushMatrix();
	glTranslatef(0.15 * robotBodyDepth, 0.0, 0.4 * robotBodyWidth); // Move to the front-right
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	// Add two dents in back of the foot
	// First back dent
	glPushMatrix();
	glTranslatef(-0.15 * robotBodyDepth, 0.0, -0.4 * robotBodyWidth); // Move to the back-left
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	// Second back dent
	glPushMatrix();
	glTranslatef(0.15 * robotBodyDepth, 0.0, -0.4 * robotBodyWidth); // Move to the back-right
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix(); // End left foot

	glPopMatrix(); // End left leg

	// Right leg (copy of the left leg but mirrored)
	glPushMatrix();
	// Move the leg lower to connect better to the body
	glTranslatef(-0.5 * robotBodyWidth, -0.7 * robotBodyLength, 0.0); // Adjust leg height

	// Hip rotation
	glRotatef(hipAngleRight, 1.0, 0.0, 0.0); // Rotate hip

	// Upper leg segment - beige
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, beige_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, beige_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, beige_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, beige_mat_shininess);

	glRotatef(-15, 1.0, 0.0, 0.0); // Zig-zag pose
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End upper leg segment

	// Add kneecap
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	// Adjust kneecap placement
	glTranslatef(0.0, -0.25 * robotBodyLength, 0.10 * robotBodyDepth);  // Move kneecap forward
	glScalef(0.25 * robotBodyWidth, 0.1 * robotBodyLength, 0.25 * robotBodyDepth);  // Adjust kneecap scale
	glutSolidCube(1.0);
	glPopMatrix();

	// Move down for knee
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);
	// Knee rotation
	glRotatef(kneeAngleRight, 1.0, 0.0, 0.0); // Rotate knee

	// Lower leg segment - green
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	glRotatef(15, 1.0, 0.0, 0.0); // Zig-zag rotation
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End lower leg segment

	// New kneecap between the two green parts (right leg)
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	glTranslatef(0.0, -0.25 * robotBodyLength, 0.0); // Adjust for kneecap position
	glScalef(0.25 * robotBodyWidth, 0.1 * robotBodyLength, 0.25 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Move down for the second green part (right leg)
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);
	// Lower leg rotation
	glRotatef(lowerLegAngleRight, 1.0, 0.0, 0.0);

	// Second green part (right leg)
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	glRotatef(-15, 1.0, 0.0, 0.0); // Continue zig-zag pose
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End second green part

	// Move down for ankle (adjusted to move feet up)
	glTranslatef(0.0, -0.3 * robotBodyLength, 0.0);  // Reduced from -0.5 to -0.3 for closer connection

	// Ankle rotation
	glRotatef(ankleAngleRight, 1.0, 0.0, 0.0); // Rotate ankle

	// Foot segment - light brown
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	// Foot base
	glPushMatrix();
	glScalef(0.4 * robotBodyDepth, 0.1 * robotBodyLength, 0.6 * robotBodyWidth); // Foot dimensions
	glutSolidCube(1.0);
	glPopMatrix(); // End foot base

	// Add two dents in front of the foot
	// First front dent
	glPushMatrix();
	glTranslatef(-0.15 * robotBodyDepth, 0.0, 0.4 * robotBodyWidth); // Move to the front-left
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	// Second front dent
	glPushMatrix();
	glTranslatef(0.15 * robotBodyDepth, 0.0, 0.4 * robotBodyWidth); // Move to the front-right
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	// Add two dents in back of the foot
	// First back dent
	glPushMatrix();
	glTranslatef(-0.15 * robotBodyDepth, 0.0, -0.4 * robotBodyWidth); // Move to the back-left
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	// Second back dent
	glPushMatrix();
	glTranslatef(0.15 * robotBodyDepth, 0.0, -0.4 * robotBodyWidth); // Move to the back-right
	glScalef(0.1 * robotBodyDepth, 0.1 * robotBodyLength, 0.2 * robotBodyWidth); // Small cube for the dent
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix(); // End right foot

	glPopMatrix(); // End right leg
}


void drawLeftArm()
{
	// Set the material for the arm
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();
	// Position arm with respect to parent body
	glTranslatef(0.5 * robotBodyWidth + 0.5 * upperArmWidth, 0, 0.0); // this will be done last

	// Build arm
	glPushMatrix();
	glScalef(upperArmWidth, upperArmLength, upperArmWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Now let's add a hand at the end of the arm
	// Use the gun material properties for the hand color
	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	// Position the hand at the end of the arm
	glPushMatrix();
	glTranslatef(0.0, -0.5 * upperArmLength - 0.2, 0.0);  // Attach to the end of the arm
	glScalef(0.7 * upperArmWidth, 0.5 * upperArmLength, 0.7 * upperArmWidth);  // Increased palm thickness
	glutSolidCube(1.0);  // Hand (palm)

	// Now we add the 5 fingers (thicker and shorter)
	float fingerWidth = 0.06 * upperArmWidth; // Thicker fingers
	float fingerLength = 0.05 * upperArmLength; // Shorter length for fingers

	// Draw 5 fingers
	for (int i = -2; i <= 2; i++) {
		glPushMatrix();
		glTranslatef(i * (0.12 * upperArmWidth), -0.2 * (0.3 * upperArmLength), 0.0);  // Move fingers up
		glScalef(fingerWidth, fingerLength, fingerWidth);  // Scale fingers to thicker and shorter size
		glutSolidCube(1.0);
		glPopMatrix();
	}

	glPopMatrix();  // End of hand
	glPopMatrix();  // End of arm
}

void drawRightArm()
{
	// Set material properties for the arm
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();

	// Adjust translation to mirror the left arm, move it up and forward slightly
	glTranslatef(-(0.5 * robotBodyWidth + 0.5 * upperArmWidth), 0.2 * robotBodyLength, 0.2 * robotBodyDepth); // Moved forward slightly on the Z-axis
	glRotatef(-45.0, 1.0, 0.0, 0.0); // Tilt arm forward

	// Draw the arm (upper arm)
	glPushMatrix();
	glScalef(upperArmWidth, upperArmLength, upperArmWidth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Now handle the cannon attached to the arm
	glMaterialfv(GL_FRONT, GL_AMBIENT, gun_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, gun_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gun_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, gun_mat_shininess);

	// Position the cannon at the end of the arm
	glPushMatrix();
	glTranslatef(0.0, -0.5 * upperArmLength - 0.5 * gunLength, 0.0);  // Attach to the end of the arm

	// Apply cannon spin along its Y-axis (screw-like spin)
	if (spinCannon) {
		glRotatef(cannonSpinAngle, 0.0, 1.0, 0.0);  // Spin along the Y-axis
	}

	// Draw the gun (cannon body)
	glPushMatrix();
	glScalef(gunWidth, gunLength, gunDepth);
	glutSolidCube(1.0);  // Cannon body
	glPopMatrix();

	// Draw the cannon barrel (cylinder)
	glPushMatrix();
	glTranslatef(0.0, -0.5 * gunLength, 0.0);  // Move to the end of the cannon
	glRotatef(90.0, 1.0, 0.0, 0.0);  // Align the cylinder properly
	GLUquadric* quad = gluNewQuadric();
	gluCylinder(quad, 1.5, 1.5, 5.0, 40, 20);  // Barrel
	glPopMatrix();

	// Draw the orange projectile inside the cannon
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, red_orange_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_orange_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red_orange_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, red_orange_shininess);
	glTranslatef(0.0, -2.5 * gunLength, 0.0);  // Inside the cannon
	glScalef(gunWidth * 0.5, gunLength * 0.1, gunDepth * 0.5);
	glutSolidCube(1.0);  // The orange projectile
	glPopMatrix();

	// Draw the magazine under the cannon
	glPushMatrix();
	glTranslatef(0.0, -gunLength - 1.0, 0.0);  // Position magazine below the cannon
	glScalef(gunWidth * 0.8, gunLength * 0.4, gunDepth * 0.8);  // Scale the magazine
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();  // End cannon drawing
	glPopMatrix();  // End arm drawing
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

void stepAnimation(int value)
{
	if (walking) {
		// Move legs in opposite directions
		if (walkingForward) {
			// Move left leg forward, right leg backward
			if (hipAngleLeft < 50.0f) {
				hipAngleLeft += 2.0f;   // Raise left hip
				kneeAngleLeft -= 1.5f;  // Bend left knee
				ankleAngleLeft += 1.0f; // Raise left ankle
				lowerLegAngleLeft += 2.0f; // Rotate the lower leg at the new joint
			}

			if (hipAngleRight > -50.0f) {
				hipAngleRight -= 2.0f;   // Lower right hip
				kneeAngleRight += 1.5f;  // Straighten right knee
				ankleAngleRight -= 1.0f; // Lower right ankle
				lowerLegAngleRight -= 2.0f; // Rotate the lower leg at the new joint
			}

			// If both legs have reached their maximum angles, switch direction
			if (hipAngleLeft >= 50.0f && hipAngleRight <= -50.0f) {
				walkingForward = false;  // Switch to moving backward
			}
		}
		else { // Move legs in reverse direction (reset position)
			// Move left leg backward, right leg forward
			if (hipAngleLeft > 0.0f) {
				hipAngleLeft -= 2.0f;   // Lower left hip
				kneeAngleLeft += 1.5f;  // Straighten left knee
				ankleAngleLeft -= 1.0f; // Lower left ankle
				lowerLegAngleLeft -= 2.0f; // Reset the lower leg joint angle
			}

			if (hipAngleRight < 0.0f) {
				hipAngleRight += 2.0f;   // Raise right hip
				kneeAngleRight -= 1.5f;  // Bend right knee
				ankleAngleRight += 1.0f; // Raise right ankle
				lowerLegAngleRight += 2.0f; // Rotate the lower leg at the new joint
			}

			// If both legs have returned to their starting angles, switch direction
			if (hipAngleLeft <= 0.0f && hipAngleRight >= 0.0f) {
				walkingForward = true;   // Switch to moving forward
			}
		}

		glutPostRedisplay();  // Trigger redraw

		// Continue the walking animation if walking is still active
		if (walking) {
			glutTimerFunc(10, stepAnimation, 0);  // Continue walking
		}
	}
}

void cannonAnimation(int value)
{
	if (spinCannon)
	{
		cannonSpinAngle += 5.0f;  // Increment the cannon spin angle to rotate the cannon
		if (cannonSpinAngle > 360.0f) {
			cannonSpinAngle -= 360.0f;  // Reset the angle after a full rotation
		}
		glutPostRedisplay();  // Redraw to show the updated cannon position
		glutTimerFunc(10, cannonAnimation, 0);  // Continue spinning the cannon
	}
}

void resetJointAngles() {
	hipAngleLeft = 0.0f;
	kneeAngleLeft = 0.0f;
	ankleAngleLeft = 0.0f;
	lowerLegAngleLeft = 0.0f;

	hipAngleRight = 0.0f;
	kneeAngleRight = 0.0f;
	ankleAngleRight = 0.0f;
	lowerLegAngleRight = 0.0f;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'k':  // Control left knee
		selectedJoint = 1;  // Select knee joint
		break;
	case 'h':  // Control left hip
		selectedJoint = 2;  // Select hip joint
		break;
	case 'n':  // Control neck (head rotation)
		selectedJoint = 3;
		break;
	case 'b':  // Select body rotation
		selectedJoint = 4;
		break;
	case 'l':  // Control lower part of the left leg (between middle and lower sections)
		selectedJoint = 5;
		break;
	case 'a': // Control ankle
		selectedJoint = 6;
		break;
	case '1':  // Default view (isometric)
		cameraView = 0;
		break;
	case '2':  // Front view
		cameraView = 1;
		break;
	case '3':  // Side view
		cameraView = 2;
		break;
	case '4':  // Top-down view
		cameraView = 3;
		break;
	case 'w':  // Start/Stop walking
		walking = !walking;
		if (walking) {
			glutTimerFunc(10, stepAnimation, 0);  // Start walking animation
		}
		else {
			resetJointAngles();  // Reset joint angles when walking stops
			glutPostRedisplay(); // Trigger a redraw to reflect the reset angles
		}
		break;
	case 'c':  // Toggle cannon spinning
		spinCannon = !spinCannon;
		if (spinCannon) {
			glutTimerFunc(10, cannonAnimation, 0);
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
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

void functionKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		// Control right knee when 'k' is pressed
		if (selectedJoint == 1) {
			kneeAngleRight += 2.0f;  // Rotate right knee
		}
		// Control right hip when 'h' is pressed
		else if (selectedJoint == 2) {
			hipAngleRight += 2.0f;   // Rotate right hip
		}
		// Control neck
		else if (selectedJoint == 3) {
			neckAngle += 2.0f;     // Rotate neck (left turn)
		}
		// Control body rotation
		else if (selectedJoint == 4) {
			robotAngle += 2.0f;    // Rotate body (left)
			if (robotAngle > 360.0f) {
				robotAngle -= 360.0f;
			}
		}
		// Rotate lower left leg
		else if (selectedJoint == 5) {
			lowerLegAngleLeft += 2.0f;  // Rotate lower left leg
		}
		// Rotate ankle
		else if (selectedJoint == 6) {
			ankleAngleLeft += 2.0f;
		}
		break;

	case GLUT_KEY_RIGHT:
		// Control right knee when 'k' is pressed
		if (selectedJoint == 1) {
			kneeAngleRight -= 2.0f;  // Rotate right knee in the opposite direction
		}
		// Control right hip when 'h' is pressed
		else if (selectedJoint == 2) {
			hipAngleRight -= 2.0f;   // Rotate right hip in the opposite direction
		}
		// Control neck
		else if (selectedJoint == 3) {
			neckAngle -= 2.0f;     // Rotate neck (right turn)
		}
		// Control body rotation
		else if (selectedJoint == 4) {
			robotAngle -= 2.0f;    // Rotate body (right)
			if (robotAngle < -360.0f) {
				robotAngle += 360.0f;
			}
		}
		// Rotate lower left leg in opposite direction
		else if (selectedJoint == 5) {
			lowerLegAngleLeft -= 2.0f;  // Rotate lower left leg in the opposite direction
		}
		// Rotate ankle
		else if (selectedJoint == 6) {
			ankleAngleLeft -= 2.0f;
		}
		break;

	case GLUT_KEY_UP:
		// Control left knee when 'k' is pressed
		if (selectedJoint == 1) {
			kneeAngleLeft += 2.0f;  // Rotate left knee
		}
		// Control left hip when 'h' is pressed
		else if (selectedJoint == 2) {
			hipAngleLeft += 2.0f;   // Rotate left hip
		}
		// Control neck
		else if (selectedJoint == 3) {
			neckAngle += 2.0f;     // Rotate neck (upward turn - simulating look up)
		}
		else if (selectedJoint == 4) {
			robotAngle += 2.0f;    // Optional: You can add more functionality for the body here
		}
		// Rotate lower left leg in opposite direction
		else if (selectedJoint == 5) {
			lowerLegAngleLeft += 2.0f;  // Rotate lower left leg in the opposite direction
		}
		// Rotate ankle
		else if (selectedJoint == 6) {
			ankleAngleLeft += 2.0f;
		}
		break;

	case GLUT_KEY_DOWN:
		// Control left knee when 'k' is pressed
		if (selectedJoint == 1) {
			kneeAngleLeft -= 2.0f;  // Rotate left knee in the opposite direction
		}
		// Control left hip when 'h' is pressed
		else if (selectedJoint == 2) {
			hipAngleLeft -= 2.0f;   // Rotate left hip in the opposite direction
		}
		// Control neck
		else if (selectedJoint == 3) {
			neckAngle -= 2.0f;     // Rotate neck (downward turn - simulating look down)
		}
		else if (selectedJoint == 4) {
			robotAngle -= 2.0f;    // Optional: You can add more functionality for the body here
		}
		// Rotate lower left leg
		else if (selectedJoint == 5) {
			lowerLegAngleLeft -= 2.0f;  // Rotate lower left leg
		}
		// Rotate ankle
		else if (selectedJoint == 6) {
			ankleAngleLeft -= 2.0f;
		}
		break;
	}
	glutPostRedisplay();   // Trigger redraw to apply changes
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