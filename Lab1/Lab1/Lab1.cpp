/*
CS6554 Lab1
Mobai Wang
*/
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "Lab1.h"
#include "Model.cpp"

using namespace std;

// -------------- parameters -------------- 
const int width = 800;
const int height = 800;

int mouseX, mouseY;
int pmouseX, pmouseY;
float rotationX, rotationY;

float camRotation = 0;

Model* model;

vector<float> world2screen(float* point4d) {
	float* temp1 = mvMult(m_trans, point4d);
	float* temp2 = mvMult(m_rotation, temp1);
	float* v_final = mvMult(m_pers, temp2);
	vector<float> screencoord;
	screencoord.push_back(map(v_final[0] / v_final[3], -1, 1, 0, width));
	screencoord.push_back(map(v_final[1] / v_final[3], -1, 1, 0, height));

	delete[] temp1;
	delete[] temp2;
	delete[] v_final;
	return screencoord;
}

void setup() {
	// Best Camera Angle
	rotationX = 5.24;
	rotationY = 5.7;
	// Load Model
	model = new Model("camaro.d.txt");
	// Setup Camera and Frustum
	setCamera(15, 10, -15, 0, 0, 0, 0, 1, 0);
	setFrustum(5, 0, 2);
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.1, 0.1, 0.1, 1);

	//---------- Auto Rotation -----------------
	//setCamera(sin(camRotation) * 20, 10, cos(camRotation) * 20, 0, 0, 0, 0, 1, 0);

	//---------- Mouse Control -----------------
	rotationX += (mouseX - pmouseX) * 0.005;
	rotationY += (mouseY - pmouseY) * 0.05;
	setCamera(cos(rotationX) * 20, rotationY, sin(rotationX) * 20, 0, 0, 0, 0, 1, 0);

	////---------- Vertex Mode ---------------
	//camRotation += 0.001;
	//glBegin(GL_POINTS);
	//glColor3f(1, 1, 1);
	//for (int i = 0; i < model->verts.size(); i++) {
	//	float* point4d = new float[4]{ model->verts[i][0], model->verts[i][1], model->verts[i][2], 1 };
	//	vector<float> p1 = world2screen(point4d);
	//	glVertex2f(p1[0], p1[1]);
	//	delete[] point4d;
	//}
	//glEnd();

	//----------- Wireframe Mode --------------
	camRotation += 0.01;
	glBegin(GL_LINES);
	for (int i = 0; i < model->faces.size(); i++) {
		vector<int> pointsPerFace = model->faces[i];
		//// Check Face Normal Direction
		int p1_index = pointsPerFace[0] - 1;
		int p2_index = pointsPerFace[1] - 1;
		int p3_index = pointsPerFace[2] - 1;
		float* p1 = new float[3]{ model->verts[p1_index][0], model->verts[p1_index][1], model->verts[p1_index][2] };
		float* p2 = new float[3]{ model->verts[p2_index][0], model->verts[p2_index][1], model->verts[p2_index][2] };
		float* p3 = new float[3]{ model->verts[p3_index][0], model->verts[p3_index][1], model->verts[p3_index][2] };
		float* edge1 = vvSub(p1, p2);
		float* edge2 = vvSub(p3, p2);
		float* Np = vvCross(edge1, edge2);
		float* Npc = vvSub(camera, p2);
		float dotVal = vvDot(Np, Npc);
		delete[] p1;
		delete[] p2;
		delete[] p3;
		delete[] edge1;
		delete[] edge2;
		delete[] Np;
		delete[] Npc;
		if (dotVal <= 0) {
			continue;
		}

		// Draw Wire Frame
		for (int j = 0; j < pointsPerFace.size(); j++) {
			int point1 = pointsPerFace[j] - 1;
			int point2 = pointsPerFace[(j + 1) % pointsPerFace.size()] - 1;
			float* point4d1 = new float[4]{ model->verts[point1][0], model->verts[point1][1], model->verts[point1][2], 1 };
			float* point4d2 = new float[4]{ model->verts[point2][0], model->verts[point2][1], model->verts[point2][2], 1 };
			vector<float> p1 = world2screen(point4d1);
			vector<float> p2 = world2screen(point4d2);
			glVertex2f(p1[0], p1[1]);
			glVertex2f(p2[0], p2[1]);
			delete[] point4d1;
			delete[] point4d2;
		}
	}
	glEnd();

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
}

int main(int argc, char** argv)
{
	// Setup Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lab1");

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

	glutMainLoop();
	return 0;
}

float* vvAdd(float* v1, float* v2) {
	float* result = new float[3]{ v1[0] + v2[0], v1[1] + v2[1],v1[2] + v2[2] };
	return result;
}
float* vvSub(float* v1, float* v2) {
	float* result = new float[3]{ v1[0] - v2[0], v1[1] - v2[1],v1[2] - v2[2] };
	return result;
}
float* mvMult(float matrix[4][4], float* vector) {
	float* result = new float[4]{ 0 };
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i] += matrix[i][j] * vector[j];
		}
	}
	return result;
}
float vvDot(float* v1, float* v2) {
	float result = 0;
	for (int i = 0; i < 3; i++) {
		result += v1[i] * v2[i];
	}
	return result;
}
float* vvCross(float* v1, float* v2) {
	float* result = new float[3]{ 0 };
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return result;
}
float* vNormalize(float* v) {
	float mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	float* n = new float[3]{ 0 };
	n[0] = v[0] / mag;
	n[1] = v[1] / mag;
	n[2] = v[2] / mag;
	return n;
}
void setCamera(float Cx, float Cy, float Cz, float Px, float Py, float Pz, float Vx, float Vy, float Vz) {
	m_trans[0][3] = -Cx;
	m_trans[1][3] = -Cy;
	m_trans[2][3] = -Cz;
	camera[0] = Cx;
	camera[1] = Cy;
	camera[2] = Cz;

	float* N = new float[3]{ Px - Cx, Py - Cy, Pz - Cz };
	N = vNormalize(N);
	float* Vcomma = new float[3]{ Vx, Vy, Vz };
	float* U = vvCross(N, Vcomma);
	U = vNormalize(U);
	float* V = vvCross(U, N);
	V = vNormalize(V);

	for (int i = 0; i < 3; i++) {
		m_rotation[0][i] = U[i];
		m_rotation[1][i] = V[i];
		m_rotation[2][i] = N[i];
	}

	delete[] N;
	delete[] U;
	delete[] V;
}
void setFrustum(float d, float f, float h) {
	m_pers[0][0] = d / h;
	m_pers[1][1] = d / h;
	m_pers[2][2] = f / (f - d);
	m_pers[2][3] = -(d * f) / (f - d);
	m_pers[3][2] = 1;
}
float map(float val, float a, float b, float c, float d) {
	return c + (d - c) * ((val - a) / (b - a));;
}