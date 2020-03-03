#include <Model.h>


Model::Model(const char* filename, float color[3], bool doCleanning) {
	loadFile(filename);
	if (doCleanning)
		cleanData();
	calcNormals();
	baseColor[0] = color[0];
	baseColor[1] = color[1];
	baseColor[2] = color[2];
}


void Model::translate(float x, float y, float z) {
	m_model_pos[0][3] = -x;
	m_model_pos[1][3] = -y;
	m_model_pos[2][3] = -z;
}
void Model::scale(float s) {
	m_model_pos[0][0] = s;
	m_model_pos[1][1] = s;
	m_model_pos[2][2] = s;
}

void Model::rotate(float alpha, float beta, float gama) {
	float cos_alpha = cos(alpha);
	float sin_alpha = sin(alpha);
	float cos_beta = cos(beta);
	float sin_beta = sin(beta);
	float cos_gama = cos(gama);
	float sin_gama = sin(gama);

	m_model_rot[0][0] = cos_gama * cos_beta;
	m_model_rot[0][1] = -sin_gama * cos_alpha + cos_gama * sin_beta * sin_alpha;
	m_model_rot[0][2] = sin_gama * sin_alpha + cos_gama * sin_beta * cos_alpha;
	m_model_rot[1][0] = sin_gama * cos_beta;
	m_model_rot[1][1] = cos_gama * cos_alpha + sin_gama * sin_beta * sin_alpha;
	m_model_rot[1][2] = -cos_gama * sin_alpha + sin_gama * sin_beta * cos_alpha;
	m_model_rot[2][0] = -sin_beta;
	m_model_rot[2][1] = cos_beta * sin_alpha;
	m_model_rot[2][2] = cos_beta * cos_alpha;


}

vector<float> Model::getNormal(int iface, int nvert) {
	float* vector4d = new float[4]{ normals[faces[iface][nvert] - 1][0], normals[faces[iface][nvert] - 1][1], normals[faces[iface][nvert] - 1][2], 0 };
	float* pos_rot = mvMult(m_model_rot, vector4d);
	vector<float> result = { pos_rot[0], pos_rot[1], pos_rot[2] };
	delete[] vector4d;
	delete[] pos_rot;
	return result;
}

vector<float> Model::getVerts(int point) {
	float* point4d = new float[4]{ verts[point][0], verts[point][1], verts[point][2], 1 };
	float* pos_rot = mvMult(m_model_rot, point4d);
	float* pos_trans = mvMult(m_model_pos, pos_rot);
	vector<float> result = { pos_trans[0], pos_trans[1], pos_trans[2] };
	delete[] point4d;
	delete[] pos_trans;
	delete[] pos_rot;
	return result;
}

vector<string>& Model::string_split(const string& str, char delim, vector<string>& elems) {
	istringstream iss(str);
	for (string item; getline(iss, item, delim); )
		if (item.empty()) continue;
		else elems.push_back(item);
	return elems;
}

void Model::loadFile(const char* filename) {
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

void Model::calcNormals() {
	//cout << "Calculate Normal Started !" << endl;
	for (int i = 0; i < verts.size(); i++) {
		vector<int> inWhichFaces;
		for (int j = 0; j < faces.size(); j++) {
			for (int k = 0; k < faces[j].size(); k++) {
				if (faces[j][k] - 1 == i)
					inWhichFaces.push_back(j);
			}
		}
		vector<vector<float>> facesNormals;
		vector<float> N;
		for (int m = 0; m < inWhichFaces.size(); m++) {
			vector<int> thisFace = faces[inWhichFaces[m]];
			vector<float> v1 = verts[thisFace[0] - 1];
			vector<float> v2 = verts[thisFace[1] - 1];
			vector<float> v3 = verts[thisFace[2] - 1];
			N = vvCrossVector(vvSubVector(v3, v1), vvSubVector(v2, v1));
			N = vNormalizeVector(N);
			facesNormals.push_back(N);
		}
		vector<float> vAvg(3);
		for (int m = 0; m < facesNormals.size(); m++) {
			vAvg = vvAddVector(vAvg, facesNormals[m]);
		}
		vAvg = vvDivVector(vAvg, facesNormals.size());
		normals.push_back(vAvg);
	}
	cout << "Calculate Normal Complete !" << endl;
}

vector<int> Model::inWhichFaces(int index_vert) {
	vector<int> inWhichFaces;
	for (int j = 0; j < faces.size(); j++) {
		for (int k = 0; k < faces[j].size(); k++) {
			if (faces[j][k] - 1 == index_vert) {
				inWhichFaces.push_back(j);
			}
		}
	}
	return inWhichFaces;
}

void Model::cleanData() {
	cout << "Clean Data..." << endl;
	for (int i = 0; i < verts.size(); i++) {
		vector<float> p = verts[i];
		for (int j = 0; j < verts.size(); j++) {
			if (i != j) {
				vector<float> p_compare = verts[j];
				if (vvEquals(p_compare, p)) {
					vector<int> whichFaces = inWhichFaces(j);
					for (int k = 0; k < whichFaces.size(); k++) {
						vector<int> thisFace = faces[whichFaces[k]];
						for (int m = 0; m < thisFace.size(); m++) {
							vector<float> thisVert = verts[thisFace[m] - 1];
							if (vvEquals(thisVert, p)) {
								faces[whichFaces[k]][m] = i + 1;
							}
						}
					}
				}
			}
		}
	}
}