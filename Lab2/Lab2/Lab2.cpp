/*
CS6554 Lab2
Mobai Wang
*/
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "Lab2.h"
#include "Model.cpp"

using namespace std;

// ----------- Program Parameters -----
const int width = 800;
const int height = 800;

int mouseX, mouseY;
int pmouseX, pmouseY;

// ----------- Camera Parameters -------
float rotationX, rotationY;
float d = 5;
float f = 50;
float h = 2;
float cam2pref = 20;

// ----------- World Parameters --------
float colorlerp1 = 0;
float colorlerp2 = 0;

float* zbuffer;
float* ibufferR;
float* ibufferG;
float* ibufferB;

vector<Model*> models;

vector<float> world2screen(float* point4d) {
	float* temp1 = mvMult(m_trans, point4d);
	float* temp2 = mvMult(m_rotation, temp1);
	float* v_final = mvMult(m_pers, temp2);
	vector<float> screencoord;
	screencoord.push_back(map(v_final[0] / v_final[3], -1, 1, 0, width));
	screencoord.push_back(map(v_final[1] / v_final[3], -1, 1, 0, height));
	screencoord.push_back(v_final[2] / v_final[3]);
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
	Model* model = new Model("camaro.d.txt");
	models.push_back(model);

	// Setup Camera and Frustum
	setCamera(15, 10, -15, 0, 0, 0, 0, 1, 0);
	setFrustum(d, f, h);

	// Initialize ZBuffer & IBuffers
	zbuffer = new float[width * height];
	ibufferR = new float[width * height];
	ibufferG = new float[width * height];
	ibufferB = new float[width * height];
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = numeric_limits<float>::infinity();
		ibufferR[i] = 0;
		ibufferG[i] = 0;
		ibufferB[i] = 0;
	}
}

void draw() {
	// Clear ZBuffer
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = numeric_limits<float>::infinity();
		ibufferR[i] = 0.1;
		ibufferG[i] = 0.1;
		ibufferB[i] = 0.1;
	}

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.1, 0.1, 0.1, 1);

	//---------- Mouse Control -----------------
	rotationX += (mouseX - pmouseX) * 0.005;
	rotationY += (mouseY - pmouseY) * 0.05;
	setCamera(cos(rotationX) * cam2pref, rotationY, sin(rotationX) * cam2pref, 0, 0, 0, 0, 1, 0);

	//---------- Color Shifting ----------------
	colorlerp1 += 0.05;
	colorlerp2 += 0.08;

	//------------ Scan Line Mode -------------
	for (int m = 0; m < models.size(); m++) {
		for (int i = 0; i < models[m]->faces.size(); i++) {
			vector<int> pointsPerFace = models[m]->faces[i];
			//// Check Face Normal Direction
			int p1_index = pointsPerFace[0] - 1;
			int p2_index = pointsPerFace[1] - 1;
			int p3_index = pointsPerFace[2] - 1;
			float* p1 = new float[3]{ models[m]->verts[p1_index][0], models[m]->verts[p1_index][1], models[m]->verts[p1_index][2] };
			float* p2 = new float[3]{ models[m]->verts[p2_index][0], models[m]->verts[p2_index][1], models[m]->verts[p2_index][2] };
			float* p3 = new float[3]{ models[m]->verts[p3_index][0], models[m]->verts[p3_index][1], models[m]->verts[p3_index][2] };
			float* edge1 = vvSub(p1, p2);
			float* edge2 = vvSub(p3, p2);
			float* N = vvCross(edge1, edge2);
			float* face2cam = vvSub(camera, p2);
			float dotVal = vvDot(N, face2cam);
			delete[] p1;
			delete[] p2;
			delete[] p3;
			delete[] edge1;
			delete[] edge2;
			delete[] N;
			delete[] face2cam;
			if (dotVal <= 0) {
				continue;
			}

			// Draw Scan Line Face
			vector<vector<float>> screenPointsPerFace;
			float color[3];
			for (int j = 0; j < pointsPerFace.size(); j++) {
				int point = pointsPerFace[j] - 1;
				float* point4d = new float[4]{ models[m]->verts[point][0], models[m]->verts[point][1], models[m]->verts[point][2], 1 };
				//glColor3f(1 - dotVal * 0.3, 1 - dotVal * 0.25, 1 - dotVal * 0.2);	
				//glColor3f(dotVal * 0.1, 1-dotVal * 0.9, 1-dotVal * 0.8);
				color[0] = 1 - dotVal * 0.3;
				color[1] = 1 - dotVal * 0.25;	//	Edge Glowing Effect! :)
				color[2] = 1 - dotVal * 0.2;
				screenPointsPerFace.push_back(world2screen(point4d));
				delete[] point4d;
			}
			drawface(screenPointsPerFace, color);
		}
	}

	// ---------------- Display the frameBuffers -------------------
	glBegin(GL_POINTS);
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			glColor3f(ibufferR[y * width + x], ibufferG[y * width + x], ibufferB[y * width + x]);
			//glColor3f(zbuffer[y * width + x], zbuffer[y * width + x], zbuffer[y * width + x]);
			glVertex2f(x, y);
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
	float result = c + (d - c) * ((val - a) / (b - a));
	return result;
}

void drawface(vector<vector<float>> polyverts, float color[])
{
	for (int i = 0; i < polyverts.size(); i++) {
		if (polyverts[i][1] > height || polyverts[i][1] < 0) {
			return;
		}
	}
	//	Get Min Max Y Value to reduce render time
	int tableYmax = 0;
	int tableYmin = height;
	for (int i = 0; i < polyverts.size(); i++) {
		if (polyverts[i][1] > tableYmax) {
			tableYmax = polyverts[i][1];
		}
		if (polyverts[i][1] < tableYmin) {
			tableYmin = polyverts[i][1];
		}
	}

	Edge* ET[height];
	for (int i = tableYmin; i <= tableYmax; i++) {
		ET[i] = new Edge();
		ET[i]->nextEdge = nullptr;
	}
	Edge* AET = new Edge();
	AET->nextEdge = nullptr;

	// Construct ET
	for (int i = 0; i < polyverts.size(); i++)
	{
		int y0 = polyverts[(i - 1 + polyverts.size()) % polyverts.size()][1];
		float x1 = polyverts[i][0];
		int y1 = polyverts[i][1];
		float z1 = polyverts[i][2];
		float x2 = polyverts[(i + 1) % polyverts.size()][0];
		int y2 = polyverts[(i + 1) % polyverts.size()][1];
		float z2 = polyverts[(i + 1) % polyverts.size()][2];
		int y3 = polyverts[(i + 2) % polyverts.size()][1];
		if (y1 == y2)
			continue;
		int ymin = y1 > y2 ? y2 : y1;
		int ymax = y1 > y2 ? y1 : y2;
		float zmin = y1 > y2 ? z2 : z1;
		float xmin = y1 > y2 ? x2 : x1;
		float dx = float(x1 - x2) / (y1 - y2);
		float dz = float(z1 - z2) / (y1 - y2);
		if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3))) {
			ymin++;
			xmin += dx;
			zmin += dz;
		}
		Edge* e = new Edge(ymax, xmin, dx, ET[ymin]->nextEdge);
		e->zmin = zmin;
		e->ymin = ymin;
		e->dz = dz;
		ET[ymin]->nextEdge = e;
	}

	//	Scanning
	for (int y = tableYmin; y <= tableYmax; y++) {
		// Put Edges from ET to AET
		while (ET[y]->nextEdge != nullptr) {
			Edge* eInsert = ET[y]->nextEdge;
			Edge* e = AET;
			while (e->nextEdge != nullptr) {
				if (eInsert->xmin > e->nextEdge->xmin) {
					e = e->nextEdge;
					continue;
				}
				if (eInsert->xmin == e->nextEdge->xmin && eInsert->dx > e->nextEdge->dx) {
					e = e->nextEdge;
					continue;
				}
				break;
			}
			ET[y]->nextEdge = eInsert->nextEdge;
			eInsert->nextEdge = e->nextEdge;
			e->nextEdge = eInsert;
		}

		//	Fill pixel with AET
		Edge* e = AET;
		while (e->nextEdge != nullptr && e->nextEdge->nextEdge != nullptr) {
			// ZBuffer Data
			float za = e->nextEdge->zmin;
			float zb = e->nextEdge->nextEdge->zmin;
			float xa = e->nextEdge->xmin;
			float xb = e->nextEdge->nextEdge->xmin;
			for (float x = xa; x <= xb; x++) {
				float zp = zb - (zb - za) * ((xb - x) / (xb - xa));
				if (zbuffer[y * width + int(x)] > zp && x > 0 && x < width && zp > 0 && zp < 1) {
					zbuffer[y * width + int(x)] = zp;
					ibufferR[y * width + int(x)] = color[0];
					ibufferG[y * width + int(x)] = color[1];
					ibufferB[y * width + int(x)] = color[2];
				}
			}
			e = e->nextEdge->nextEdge;
		}


		// Delete y=ymax edge
		e = AET;
		while (e->nextEdge != nullptr) {
			if (e->nextEdge->ymax == y) {
				Edge* eDelete = e->nextEdge;
				e->nextEdge = eDelete->nextEdge;
				eDelete->nextEdge = nullptr;
				delete eDelete;
			}
			else {
				e = e->nextEdge;
			}
		}

		//	Update x in AET
		e = AET;
		while (e->nextEdge != nullptr) {
			e->nextEdge->xmin += e->nextEdge->dx;
			e->nextEdge->zmin += e->nextEdge->dz;
			e = e->nextEdge;
		}
	}
	delete AET;
	for (int i = tableYmin; i <= tableYmax; i++) {
		delete ET[i];
	}
}
