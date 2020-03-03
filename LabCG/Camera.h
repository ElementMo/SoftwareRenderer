#pragma once
#include <vector>
#include <limits>
#include <GL/glut.h>
#include <Utils.h>
#include <Model.h>


using namespace std;

class Camera {
private:
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
	float camera[3]{ 0 };
	std::vector<float> cameraVector;

	float d = 5;
	float f = 50;
	float h = 2;

	float* zbuffer;
	float* ibufferR;
	float* ibufferG;
	float* ibufferB;

	vector<Model*> models;

public:
	Shader shaderType;
	vector<float> light_dir = { 1, 1, -1 };

	Camera(float x, float y, float z, vector<Model*> models);
	void setCamera(float Cx, float Cy, float Cz, float Px, float Py, float Pz, float Vx, float Vy, float Vz);
	void setFrustum(float d, float f, float h);
	void drawface(vector<vector<float>> polyverts, float param, vector<vector<float>> normals, float baseColor[3], Shader shaderType);
	vector<float> world2screen(float* point4d);
	void updateCamera();
	void clearAllBuffers();
	void renderImageBuffer();
	void renderZBuffer();
};