#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Model {
public:
	vector<vector<float>> verts;
	vector<vector<int>> faces;
	
	Model(const char* filename) {
		loadFile(filename);
	}

	vector<string>& string_split(const string& str, char delim, vector<string>& elems, bool skip_empty = true) {
		istringstream iss(str);
		for (string item; getline(iss, item, delim); )
			if (skip_empty && item.empty()) continue;
			else elems.push_back(item);
		return elems;
	}

	void loadFile(const char* filename) {
		ifstream in(filename);
		if (in.fail())
			return;
		string line;
		// Read Verts and Faces
		getline(in, line);
		vector<string> verts_faces;
		string_split(line, ' ', verts_faces);
		int numVerts = stoi(verts_faces[1]);
		int numFaces = stoi(verts_faces[2]);


		for (int i = 0; i < numVerts; i++) {
			getline(in, line);
			vector<string> result;
			string_split(line, ' ', result);
			vector<float> oneVert;
			oneVert.push_back(stof(result[0]));
			oneVert.push_back(stof(result[1]));
			oneVert.push_back(stof(result[2]));
			verts.push_back(oneVert);
		}
		for (int i = 0; i < numFaces; i++) {
			getline(in, line);
			vector<string> result;
			string_split(line, ' ', result);
			int faceVertNum = stoi(result[0]);
			vector<int> oneFace;
			for (int j = 0; j < faceVertNum; j++) {
				oneFace.push_back(stoi(result[j + 1]));
			}
			faces.push_back(oneFace);
		}
	}
};