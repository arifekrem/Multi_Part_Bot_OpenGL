/*******************************************************************
           Hierarchical Multi-Part Model - Bastion Bot
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gl/glut.h>
#include <GL/glu.h>  // For gluCylinder

// Dimensions of the robot parts
float robotBodyWidth = 6.0, robotBodyHeight = 8.0, robotBodyDepth = 4.0;
float headWidth = 2.0, headHeight = 2.0, headDepth = 2.0;
float upperArmLength = 4.0, upperArmWidth = 1.0;
float lowerArmLength = 3.0, lowerArmWidth = 0.8;
float upperLegLength = 5.0, upperLegWidth = 1.2;
float lowerLegLength = 4.0, lowerLegWidth = 1.0;
float handLength = 1.0, handWidth = 0.8;
float footLength = 1.5, footWidth = 1.0;
float gunRadius = 0.7, gunLength = 3.0;

// Joint angles
float shoulderAngle = 0.0, elbowAngle = 0.0;
float hipAngleLeft = 0.0, kneeAngleLeft = 0.0;
float hipAngleRight = 0.0, kneeAngleRight = 0.0;
float cannonSpin = 0.0;
float robotAngle = 0.0;  // For rotating the robot body
bool cannonSpinning = false;
bool isLeftLegMoving = true;

// Helper functions for drawing basic shapes
void drawCube(float width, float height, float depth) {
    glPushMatrix();
    glScalef(width, height, depth);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawCylinder(float radius, float height) {
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, 20, 20);
    gluDeleteQuadric(quad);
}

// Draw the robot body
void drawBody() {
    glPushMatrix();
    drawCube(robotBodyWidth, robotBodyHeight, robotBodyDepth);
    glPopMatrix();
}

// Draw the robot head
void drawHead() {
    glPushMatrix();
    glTranslatef(0.0, robotBodyHeight / 2 + headHeight / 2, 0.0);
    drawCube(headWidth, headHeight, headDepth);
    glPopMatrix();
}

// Draw the cannon (as a cylinder)
void drawCannon() {
    glPushMatrix();  // Save transformation for cannon
    glTranslatef(0.0, robotBodyHeight / 2 + headHeight, 0.0);
    if (cannonSpinning) {
        cannonSpin += 5.0;  // Spin the cannon only if cannonSpinning is true
    }
    glRotatef(cannonSpin, 0.0, 1.0, 0.0);  // Only spin based on the `cannonSpin` value
    glTranslatef(0.0, 0.5 * headHeight, 0.0);
    drawCylinder(gunRadius, gunLength);
    glPopMatrix();  // Restore transformation for body
}

// Draw an arm with hands
void drawArm(float shoulderAng, float elbowAng) {
    // Upper arm
    glPushMatrix();
    glRotatef(shoulderAng, 1.0, 0.0, 0.0);  // Shoulder joint
    drawCube(upperArmWidth, upperArmLength, upperArmWidth);

    // Lower arm
    glTranslatef(0.0, -upperArmLength / 2, 0.0);
    glRotatef(elbowAng, 1.0, 0.0, 0.0);  // Elbow joint
    glTranslatef(0.0, -lowerArmLength / 2, 0.0);
    drawCube(lowerArmWidth, lowerArmLength, lowerArmWidth);

    // Hand
    glTranslatef(0.0, -lowerArmLength / 2 - handLength / 2, 0.0);
    drawCube(handWidth, handLength, handWidth);  // Add distinct hands

    glPopMatrix();
}

// Draw a leg with feet
void drawLeg(float hipAng, float kneeAng) {
    // Upper leg
    glPushMatrix();
    glTranslatef(0.0, -upperLegLength / 2, 0.0);  // Adjust leg position
    glRotatef(hipAng, 1.0, 0.0, 0.0);  // Hip joint
    drawCube(upperLegWidth, upperLegLength, upperLegWidth);

    // Lower leg
    glTranslatef(0.0, -upperLegLength / 2, 0.0);
    glRotatef(kneeAng, 1.0, 0.0, 0.0);  // Knee joint
    glTranslatef(0.0, -lowerLegLength / 2, 0.0);
    drawCube(lowerLegWidth, lowerLegLength, lowerLegWidth);

    // Foot
    glTranslatef(0.0, -lowerLegLength / 2 - footLength / 2, 0.0);
    drawCube(footWidth, footLength, footWidth);  // Add distinct feet

    glPopMatrix();
}

// Draw the full robot
void drawRobot() {
    glPushMatrix();
    glRotatef(robotAngle, 0.0, 1.0, 0.0);  // Rotate the whole body with 'R'
    drawBody();
    drawHead();
    drawCannon();  // Isolate the cannon from other transformations

    // Left arm
    glPushMatrix();
    glTranslatef(robotBodyWidth / 2 + upperArmWidth / 2, robotBodyHeight / 4, 0.0);
    drawArm(shoulderAngle, elbowAngle);
    glPopMatrix();

    // Right arm
    glPushMatrix();
    glTranslatef(-(robotBodyWidth / 2 + upperArmWidth / 2), robotBodyHeight / 4, 0.0);
    drawArm(shoulderAngle, elbowAngle);
    glPopMatrix();

    // Left leg
    glPushMatrix();
    glTranslatef(robotBodyWidth / 4, -(robotBodyHeight / 2), 0.0);
    drawLeg(hipAngleLeft, kneeAngleLeft);
    glPopMatrix();

    // Right leg
    glPushMatrix();
    glTranslatef(-robotBodyWidth / 4, -(robotBodyHeight / 2), 0.0);
    drawLeg(hipAngleRight, kneeAngleRight);
    glPopMatrix();

    glPopMatrix();
}

// Step animation for alternating leg movement without affecting the cannon
void stepAnimation() {
    if (isLeftLegMoving) {
        hipAngleLeft = fmin(hipAngleLeft + 5.0, 30.0);  // move left leg forward
        kneeAngleLeft = fmax(kneeAngleLeft - 2.5, -15.0);
        hipAngleRight = fmax(hipAngleRight - 5.0, -30.0);  // move right leg back
        kneeAngleRight = fmin(kneeAngleRight + 2.5, 15.0);
    }
    else {
        hipAngleRight = fmin(hipAngleRight + 5.0, 30.0);  // move right leg forward
        kneeAngleRight = fmax(kneeAngleRight - 2.5, -15.0);
        hipAngleLeft = fmax(hipAngleLeft - 5.0, -30.0);  // move left leg back
        kneeAngleLeft = fmin(kneeAngleLeft + 2.5, 15.0);
    }

    // Toggle leg for the next step
    if ((hipAngleLeft == 30.0 || hipAngleRight == 30.0) ||
        (hipAngleLeft == -30.0 || hipAngleRight == -30.0)) {
        isLeftLegMoving = !isLeftLegMoving;
    }

    glutPostRedisplay();
}

// Display function
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    drawRobot();
    glutSwapBuffers();
}

// Handle keyboard inputs
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'c':  // Start cannon spin
        cannonSpinning = true;
        glutPostRedisplay();
        break;
    case 'C':  // Stop cannon spin
        cannonSpinning = false;
        glutPostRedisplay();
        break;
    case 'w':  // Animate leg step without affecting cannon
        stepAnimation();
        glutPostRedisplay();
        break;
    case 'W':  // Reset leg step
        hipAngleLeft = 0.0;
        kneeAngleLeft = 0.0;
        hipAngleRight = 0.0;
        kneeAngleRight = 0.0;
        glutPostRedisplay();
        break;
    case 'r':  // Rotate the body
        robotAngle += 5.0;
        glutPostRedisplay();
        break;
    case 'R':  // Rotate body backwards
        robotAngle -= 5.0;
        glutPostRedisplay();
        break;
    case 's':  // Adjust shoulder angle
        shoulderAngle += 5.0;
        glutPostRedisplay();
        break;
    case 'e':  // Adjust elbow angle
        elbowAngle += 5.0;
        glutPostRedisplay();
        break;
    }
}

// Initialization function
void initOpenGL(int w, int h) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);

    GLfloat lightPos0[] = { -4.0f, 8.0f, 8.0f, 1.0f };
    GLfloat lightPos1[] = { 4.0f, 8.0f, 8.0f, 1.0f };
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Reshape callback
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    initOpenGL(w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Bastion-Like Robot");

    initOpenGL(800, 600);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
