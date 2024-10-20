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
// Flag to control walking state
bool walking = false;
bool stepBackwards = false;  // Controls whether the leg is stepping forward or backward
bool walkingForward = true;   // Track whether we're walking forward
int selectedJoint = 0; // 0 for none, 1 for knee, 2 for hip, 3 for body


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
	// Move the leg lower to connect better to the body
	glTranslatef(0.5 * robotBodyWidth, -0.7 * robotBodyLength, 0.0); // Adjust leg height

	// Hip rotation
	glRotatef(hipAngleLeft, 1.0, 0.0, 0.0); // Rotate hip

	// Upper leg segment - beige
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, beige_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, beige_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, beige_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, beige_mat_shininess);

	// Zig-zag rotation for upper leg
	glRotatef(15, 1.0, 0.0, 0.0);
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End upper leg segment

	// Move down for knee
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);
	// Knee rotation
	glRotatef(kneeAngleLeft, 1.0, 0.0, 0.0); // Rotate knee

	// Lower leg segment - green
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, green_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, green_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, green_mat_shininess);

	glRotatef(-15, 1.0, 0.0, 0.0); // Rotate to maintain zig-zag
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End lower leg segment

	// Move down for ankle
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);
	// Ankle rotation
	glRotatef(ankleAngleLeft, 1.0, 0.0, 0.0); // Rotate ankle

	// Foot segment - light brown
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	glScalef(0.4 * robotBodyDepth, 0.1 * robotBodyLength, 0.6 * robotBodyWidth); // Foot dimensions
	glutSolidCube(1.0);
	glPopMatrix(); // End foot

	glPopMatrix(); // End left leg

	// Right leg (copy of the left leg but positioned on the other side)
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

	// Zig-zag rotation for upper leg
	glRotatef(15, 1.0, 0.0, 0.0);
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End upper leg segment

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

	glRotatef(-15, 1.0, 0.0, 0.0); // Rotate to maintain zig-zag
	glScalef(0.2 * robotBodyWidth, 0.5 * robotBodyLength, 0.2 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix(); // End lower leg segment

	// Move down for ankle
	glTranslatef(0.0, -0.5 * robotBodyLength, 0.0);
	// Ankle rotation
	glRotatef(ankleAngleRight, 1.0, 0.0, 0.0); // Rotate ankle

	// Foot segment - light brown
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, light_brown_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_brown_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, light_brown_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, light_brown_mat_shininess);

	glScalef(0.4 * robotBodyDepth, 0.1 * robotBodyLength, 0.6 * robotBodyWidth); // Foot dimensions
	glutSolidCube(1.0);
	glPopMatrix(); // End foot

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

	// Correct translation to the right side of the body
	glTranslatef(-(0.5 * robotBodyWidth + 0.5 * upperArmWidth), 0.5 * robotBodyLength, 0.0);

	// Rotate around the shoulder
	glRotatef(shoulderAngle, 1.0, 0.0, 0.0);  // Hinge at shoulder

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

	// Translate for the cannon's position relative to the arm
	glPushMatrix();
	glTranslatef(0.0, -0.5 * upperArmLength - 0.5 * gunLength, 0.0);

	// Apply cannon spin (if activated by pressing 'C')
	if (spinCannon) {
		glRotatef(cannonSpinAngle, 0.0, 1.0, 0.0);  // Spin along the vertical axis (Y-axis)
	}

	// Draw the gun (cannon body)
	glPushMatrix();
	glScalef(gunWidth, gunLength, gunDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	// Draw the cannon barrel (cylinder)
	glPushMatrix();
	glTranslatef(0.0, -0.5 * gunLength, 0.0);  // Position at the end of the cannon
	glRotatef(90.0, 1.0, 0.0, 0.0);  // Align the barrel properly
	GLUquadric* quad = gluNewQuadric();
	gluCylinder(quad, 1.5, 1.5, 5.0, 40, 20);  // Barrel
	glPopMatrix();

	// Add the orange projectile inside the cannon
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT, red_orange_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_orange_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red_orange_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, red_orange_shininess);
	glTranslatef(0.0, -2.5 * gunLength, 0.0);  // Inside the cannon
	glScalef(gunWidth * 0.5, gunLength * 0.1, gunDepth * 0.5);
	glutSolidCube(1.0);  // The orange projectile
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
		// Move left leg forward, right leg backward
		if (walkingForward) {
			// Move left leg forward
			if (hipAngleLeft < 30.0f) {
				hipAngleLeft += 2.0f;   // Raise left hip
				kneeAngleLeft -= 2.0f;  // Bend left knee
				ankleAngleLeft += 1.0f; // Raise left ankle
			}
			else {
				walkingForward = false;  // Switch to moving backward
			}

			// Move right leg backward
			if (hipAngleRight > -30.0f) {
				hipAngleRight -= 2.0f;   // Lower right hip
				kneeAngleRight += 2.0f;  // Straighten right knee
				ankleAngleRight -= 1.0f; // Lower right ankle
			}
		}
		else { // Move legs in reverse direction (reset position)
			// Move left leg backward
			if (hipAngleLeft > 0.0f) {
				hipAngleLeft -= 2.0f;   // Lower left hip
				kneeAngleLeft += 2.0f;  // Straighten left knee
				ankleAngleLeft -= 1.0f; // Lower left ankle
			}
			else {
				walkingForward = true;   // Switch to moving forward
			}

			// Move right leg forward
			if (hipAngleRight < 0.0f) {
				hipAngleRight += 2.0f;   // Raise right hip
				kneeAngleRight -= 2.0f;  // Bend right knee
				ankleAngleRight += 1.0f; // Raise right ankle
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
		shoulderAngle += 5.0f;  // Increment the shoulder angle to rotate the arm and cannon

		if (shoulderAngle > 360.0f)
		{
			shoulderAngle -= 360.0f;  // Reset angle after full rotation
		}

		glutPostRedisplay();  // Redraw to show the updated arm and cannon position

		// Continue the animation
		glutTimerFunc(10, cannonAnimation, 0);
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'k': // Adjust knee angle
		selectedJoint = 1;
		break;
	case 'h': // Adjust hip angle
		selectedJoint = 2;
		break;
	case 'b': // Adjust body rotation or another joint
		selectedJoint = 3;
		break;
	case 'r': // Rotate the robot�s body
		robotAngle += 2.0;
		break;
	case 'R': // Rotate the robot�s body in the opposite direction
		robotAngle -= 2.0;
		break;
	case 'w': // Start walking animation
		walking = !walking;
		if (walking) {
			glutTimerFunc(10, stepAnimation, 0);  // Start the walking animation
		}
		break;

		// Start rotating the arm and cannon
	case 'c':
		spinCannon = !spinCannon;  // Toggle the spinning state
		if (spinCannon) {
			shoulderAngle = 0.0;  // Ensure the shoulder rotation starts at 0
			glutTimerFunc(10, cannonAnimation, 0);  // Start cannon animation
		}
		break;

		// Stop cannon rotation and reset the angle
	case 'C':
		spinCannon = false;  // Disable spinning
		shoulderAngle = 0.0f;  // Reset the shoulder rotation
		glutPostRedisplay();  // Redraw
		break;

	default:
		break;
	}

	glutPostRedisplay();  // Trigger window redisplay after key press
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
		if (selectedJoint == 1) {
			kneeAngleLeft += 2.0f;  // Increment left knee angle
		}
		else if (selectedJoint == 2) {
			hipAngleLeft += 2.0f;   // Increment left hip angle
		}
		else if (selectedJoint == 3) {
			robotAngle += 2.0f;     // Rotate the entire body
		}
		break;

	case GLUT_KEY_RIGHT:
		if (selectedJoint == 1) {
			kneeAngleLeft -= 2.0f;  // Decrement left knee angle
		}
		else if (selectedJoint == 2) {
			hipAngleLeft -= 2.0f;   // Decrement left hip angle
		}
		else if (selectedJoint == 3) {
			robotAngle -= 2.0f;     // Rotate the entire body in the opposite direction
		}
		break;

	case GLUT_KEY_UP:
		if (selectedJoint == 1) {
			kneeAngleRight += 2.0f;  // Increment right knee angle
		}
		else if (selectedJoint == 2) {
			hipAngleRight += 2.0f;   // Increment right hip angle
		}
		break;

	case GLUT_KEY_DOWN:
		if (selectedJoint == 1) {
			kneeAngleRight -= 2.0f;  // Decrement right knee angle
		}
		else if (selectedJoint == 2) {
			hipAngleRight -= 2.0f;   // Decrement right hip angle
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
