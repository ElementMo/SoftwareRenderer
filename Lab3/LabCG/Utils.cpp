#include <Utils.h>

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

std::vector<float> vvAddVector(std::vector<float> v1, std::vector<float> v2) {
	std::vector<float> result;
	result.push_back(v1[0] + v2[0]);
	result.push_back(v1[1] + v2[1]);
	result.push_back(v1[2] + v2[2]);
	return result;
}

std::vector<float> vvSubVector(std::vector<float> v1, std::vector<float> v2) {
	std::vector<float> result;
	result.push_back(v1[0] - v2[0]);
	result.push_back(v1[1] - v2[1]);
	result.push_back(v1[2] - v2[2]);
	return result;
}
std::vector<float> vvDivVector(std::vector<float> v1, float scalar) {
	std::vector<float> result;
	result.push_back(v1[0] / scalar);
	result.push_back(v1[1] / scalar);
	result.push_back(v1[2] / scalar);
	return result;
}
std::vector<float> vvMultVector(std::vector<float> v1, float scalar) {
	std::vector<float> result;
	result.push_back(v1[0] * scalar);
	result.push_back(v1[1] * scalar);
	result.push_back(v1[2] * scalar);
	return result;
}
float vvDotVector(std::vector<float> v1, std::vector<float> v2) {
	float result = 0;
	for (int i = 0; i < 3; i++) {
		result += v1[i] * v2[i];
	}
	return result;
}
std::vector<float> vvCrossVector(std::vector<float> v1, std::vector<float> v2) {
	std::vector<float> result(3);
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return result;
}

std::vector<float> vNormalizeVector(std::vector<float> v) {
	float mag = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	std::vector<float> n;
	n.push_back(v[0] / mag);
	n.push_back(v[1] / mag);
	n.push_back(v[2] / mag);
	return n;
}
float vMagVector(std::vector<float> v) {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

bool vvEquals(std::vector<float> v1, std::vector<float> v2) {
	return (abs(v1[0] - v2[0]) < EPSILON && abs(v1[1] - v2[1]) < EPSILON && abs(v1[2] - v2[2]) < EPSILON);
}


float map(float val, float a, float b, float c, float d) {
	float result = c + (d - c) * ((val - a) / (b - a));
	return result;
}

float lerp(float start, float stop, float amt) {
	return start + (stop - start) * amt;
}