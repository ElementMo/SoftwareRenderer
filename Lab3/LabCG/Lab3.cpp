/*
CS6554 Lab3
Mobai Wang
*/
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <Model.h>
#include <Camera.h>


using namespace std;

// ----------- Program Parameters -----
int mouseX, mouseY;
int pmouseX, pmouseY;
int frameCount;

// ----------- Camera Parameters -------
float rotationX, rotationY;
float d = 5;
float f = 50;
float h = 2;
float cam2pref = 20;
float cam2prefLerp = 20;

// ----------- World ------------
vector<Model*> models;
Camera* cam;


void setup() {

	// Best Camera Angle
	//rotationX = 6.81;
	rotationY = 10;

	// Load Model
	int model_num = 8;
	for (int i = 0; i < model_num; i++) {
		float color[3] = { 1.6 - 0.2 * i, i * 0.15,  0.2 };
		Model* model = new Model("DFiles/donut.d.txt", color, false);
		model->scale(1.5);
		model->translate(cos(TAU / model_num * i) * 4, -0.5, sin(TAU / model_num * i) * 4);
		model->rotate(i * 0.1, -TAU / model_num * i, 0);
		models.push_back(model);
	}
	float color[3] = { 0.1, 0.6, 0.9 };
	Model* model = new Model("DFiles/teapot.d.txt", color, true);
	models.push_back(model);

	// Create Camera
	cam = new Camera(0, 0, -20, models);
	cam->shaderType = Flat;
}

void draw() {
	frameCount++;

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.1, 0.1, 0.1, 1);

	// Clear ZBuffer
	cam->clearAllBuffers();

	//---------- Mouse Control -----------------
	cam2pref = lerp(cam2pref, cam2prefLerp, 0.2);
	rotationX += (mouseX - pmouseX) * 0.005;
	rotationY += (mouseY - pmouseY) * 0.05;

	//---------- Update Camera -----------------
	cam->setCamera(cos(rotationX - PI / 2) * cam2pref, rotationY, sin(rotationX - PI / 2) * cam2pref, 0, 0, 0, 0, 1, 0);

	//---------- Update Model ------------------
	for (int i = 0; i < models.size(); i++) {
		if (i < models.size() - 1)
			models[i]->rotate(frameCount / 20.0, -TAU / models.size() * i, 0);
		else
			models[i]->rotate(0, frameCount / 20.0, 0);
	}

	//---------- Draw Camera Buffers -----------
	cam->updateCamera();
	cam->renderImageBuffer();
	//cam->renderZBuffer();

	glFlush();
	glutPostRedisplay();
	pmouseX = mouseX;
	pmouseY = mouseY;
}

void mouse_move(int x, int y) {
	mouseX = x;
	mouseY = y;
}

void mouse_func(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		mouseX = x;
		mouseY = y;
		pmouseX = x;
		pmouseY = y;
	}
	if ((button == 3) || (button == 4)) {
		if (state == GLUT_UP) return;
		cam2prefLerp = (button == 3) ? cam2prefLerp -= 1 : cam2prefLerp += 1;
	}
}
void keyPressed(unsigned char key, int x, int y) {
	if (cam->shaderType == Phong)
		cam->shaderType = Gouraud;
	else if (cam->shaderType == Gouraud)
		cam->shaderType = Flat;
	else
		cam->shaderType = Phong;
}

int main(int argc, char** argv)
{
	// Setup Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lab2");

	// Setup Graphics
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, width, 0, height);

	// My Own Setup
	setup();

	// Call Back Functions
	glutDisplayFunc(draw);
	glutMotionFunc(mouse_move);
	glutMouseFunc(mouse_func);
	glutKeyboardFunc(keyPressed);

	glutMainLoop();
	return 0;
}
