#pragma once

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


float* vvAdd(float* v1, float* v2);
float* vvSub(float* v1, float* v2);
float* mvMult(float matrix[4][4], float* vector);
float vvDot(float* v1, float* v2);
float* vvCross(float* v1, float* v2);
float* vNormalize(float* v);
void setCamera(float Cx, float Cy, float Cz, float Px, float Py, float Pz, float Ux, float Uy, float Uz);
void setFrustum(float d, float f, float h);
float map(float val, float a, float b, float c, float d);