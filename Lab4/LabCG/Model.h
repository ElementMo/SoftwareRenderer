#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Utils.h>

using namespace std;

class Model {
private:
	float m_model_pos[4][4] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1},
	};
	float m_model_rot[4][4] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1},
	};

public:
	int texture_size = 0;
	int* texureR;
	int* texureG;
	int* texureB;
	int* specular;
	int* cloud;
	int* lights;

	vector<vector<float>> verts;
	vector<vector<int>> faces;
	vector<vector<float>> normals;
	float baseColor[3] = { 0 };

	Model(const char* filename, float color[3], bool doCleanning);
	void translate(float x, float y, float z);
	void scale(float s);
	void rotate(float x, float y, float z);
	vector<float> getVerts(int point);
	vector<string>& string_split(const string& str, char delim, vector<string>& elems);
	void loadFile(const char* filename);
	void calcNormals();
	vector<int> inWhichFaces(int index_vert);
	vector<float> getNormal(int iface, int nvert);
	void cleanData();
	void loadTexture(const char* diffuseTexture, const char* specularTexture, const char* cloudTexture, const char* lightsTexture);
	float texture2D(float u, float v, int* colorChannel);
};