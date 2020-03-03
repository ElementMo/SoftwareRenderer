#pragma once
#include <vector>

float m_rotation[4][4] = {
	{1,0,0,0},
	{0,1,0,0},
	{0,0,1,0},
	{0,0,0,1},
};
float m_trans[4][4] = {
	{1,0,0,0},
	{0,1,0,0},
	{0,0,1,0},
	{0,0,0,1},
};
float m_pers[4][4] = {
	{1,0,0,0},
	{0,1,0,0},
	{0,0,1,0},
	{0,0,1,0}
};
float m_model[4][4] = {
	{1,0,0,0},
	{0,1,0,0},
	{0,0,1,0},
	{0,0,0,1},
};

float camera[3]{ 0 };

class Edge {
public:
	int ymax;
	float xmin;
	float dx;

	float zmin;
	float dz;
	int ymin;
	Edge* nextEdge;
	Edge(){}
	Edge(int ymax, float xmin, float dx, Edge* nextEdge) {
		this->ymax = ymax;
		this->xmin = xmin;
		this->dx = dx;
		this->nextEdge = nextEdge;
	}

};


float* vvAdd(float* v1, float* v2);
float* vvSub(float* v1, float* v2);
float* mvMult(float matrix[4][4], float* vector);
float vvDot(float* v1, float* v2);
float* vvCross(float* v1, float* v2);
float* vNormalize(float* v);
void setCamera(float Cx, float Cy, float Cz, float Px, float Py, float Pz, float Ux, float Uy, float Uz);
void setFrustum(float d, float f, float h);
float map(float val, float a, float b, float c, float d);

void drawface(std::vector<std::vector<float>> polyverts, float color[]);