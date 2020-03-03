#pragma once
#include <cmath>
#include <vector>
#include <algorithm>

#define PI 3.14159265358979323846
#define TAU 6.28318530717958647692
#define EPSILON 1.0E-4

const int width = 800;
const int height = 800;

class Edge {
public:
	int ymax;
	int ymin;
	float xmin;
	float dx;

	float zmin;
	std::vector<float> nNear;
	float dz;
	std::vector<float> dn;
	Edge* nextEdge;

	Edge() {}
	Edge(int ymax, float xmin, float dx, Edge* nextEdge) {
		this->ymax = ymax;
		this->xmin = xmin;
		this->dx = dx;
		this->nextEdge = nextEdge;
	}
};

enum Shader {
	Phong,
	Gouraud,
	Flat
};

float* vvAdd(float* v1, float* v2);
float* vvSub(float* v1, float* v2);
float* mvMult(float matrix[4][4], float* vector);
float vvDot(float* v1, float* v2);
float* vvCross(float* v1, float* v2);
float* vNormalize(float* v);
float map(float val, float a, float b, float c, float d);
float lerp(float start, float stop, float amt);

std::vector<float> vvAddVector(std::vector<float> v1, std::vector<float> v2);
std::vector<float> vvSubVector(std::vector<float> v1, std::vector<float> v2);
std::vector<float> vvDivVector(std::vector<float> v1, float scalar);
std::vector<float> vvMultVector(std::vector<float> v1, float scalar);
float vvDotVector(std::vector<float> v1, std::vector<float> v2);
std::vector<float> vvCrossVector(std::vector<float> v1, std::vector<float> v2);
std::vector<float> vNormalizeVector(std::vector<float> v);
float vMagVector(std::vector<float> v);
bool vvEquals(std::vector<float> v1, std::vector<float> v2);