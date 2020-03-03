#include <Camera.h>

Camera::Camera(float x, float y, float z, vector<Model*> models) {
	cameraVector = { 0, 0, 0 };
	// Setup Camera and Frustum
	setCamera(x, y, z, 0, 0, 0, 0, 1, 0);
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
	// Load World
	this->models = models;
}

void Camera::setCamera(float Cx, float Cy, float Cz, float Px, float Py, float Pz, float Vx, float Vy, float Vz) {
	m_trans[0][3] = -Cx;
	m_trans[1][3] = -Cy;
	m_trans[2][3] = -Cz;

	camera[0] = Cx;
	camera[1] = Cy;
	camera[2] = Cz;

	cameraVector[0] = Cx;
	cameraVector[1] = Cy;
	cameraVector[2] = Cz;

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

void Camera::setFrustum(float d, float f, float h) {
	m_pers[0][0] = d / h;
	m_pers[1][1] = d / h;
	m_pers[2][2] = f / (f - d);
	m_pers[2][3] = -(d * f) / (f - d);
	m_pers[3][2] = 1;
}

void Camera::drawface(vector<vector<float>> polyverts, float param, vector<vector<float>> normals, float baseColor[3], Shader shaderType)
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
		vector<float> n1 = normals[i];
		float x2 = polyverts[(i + 1) % polyverts.size()][0];
		int y2 = polyverts[(i + 1) % polyverts.size()][1];
		float z2 = polyverts[(i + 1) % polyverts.size()][2];
		vector<float> n2 = normals[(i + 1) % normals.size()];
		int y3 = polyverts[(i + 2) % polyverts.size()][1];
		//if (y1 == y2)
		//	continue;
		int ymin = y1 > y2 ? y2 : y1;
		int ymax = y1 > y2 ? y1 : y2;

		float zmin = y1 > y2 ? z2 : z1;
		float xmin = y1 > y2 ? x2 : x1;
		vector<float> nNear = y1 > y2 ? n2 : n1;
		float dx = float(x1 - x2) / (y1 - y2);
		float dz = float(z1 - z2) / (y1 - y2);
		vector<float> dn = vvDivVector(vvSubVector(n1, n2), (y1 - y2));
		if (((y1 < y2) && (y1 > y0)) || ((y2 < y1) && (y2 > y3))) {
			ymin++;
			xmin += dx;
			zmin += dz;
			nNear = vvAddVector(nNear, dn);
		}
		Edge* e = new Edge();
		e->ymax = ymax;
		e->xmin = xmin;
		e->dx = dx;
		e->nextEdge = ET[ymin]->nextEdge;
		e->zmin = zmin;
		e->ymin = ymin;
		e->nNear = nNear;
		e->dz = dz;
		e->dn = dn;
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

				if (zbuffer[y * width + int(x)] > zp&& x > 0 && x < width && zp > 0 && zp < 1) {
					zbuffer[y * width + int(x)] = zp;

					vector<float> na = e->nextEdge->nNear;
					vector<float> nb = e->nextEdge->nextEdge->nNear;
					vector<float> N = vvSubVector(nb, vvMultVector(vvSubVector(nb, na), ((xb - x) / (xb - xa))));

					vector<float> pixelColor(3);

					if (shaderType == Phong || shaderType == Flat) {
						N = vNormalizeVector(N);
						//vector<float> R = vNormalizeVector(vvSubVector(vvMultVector(N, 2 * vvDotVector(N, light_dir)), light_dir));
						//float specular = pow(max(vvDotVector(vNormalizeVector(cameraVector), R), 0.0f), 64);	
						vector<float> H = vNormalizeVector(vvAddVector(light_dir, vNormalizeVector(cameraVector)));
						float specular = pow(max(vvDotVector(N, H), 0.0f), 128);
						float diffuse = vvDotVector(N, light_dir);
						pixelColor = {
							baseColor[0] * diffuse + specular * 2,
							baseColor[1] * diffuse + specular * 2,
							baseColor[2] * diffuse + specular * 2 };
					}

					if (shaderType == Gouraud) {
						pixelColor = N;
					}

					ibufferR[y * width + int(x)] = pixelColor[0];
					ibufferG[y * width + int(x)] = pixelColor[1];
					ibufferB[y * width + int(x)] = pixelColor[2];
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
			e->nextEdge->nNear = vvAddVector(e->nextEdge->nNear, e->nextEdge->dn);
			e = e->nextEdge;
		}
	}
	delete AET;
	for (int i = tableYmin; i <= tableYmax; i++) {
		delete ET[i];
	}
}

vector<float> Camera::world2screen(float* point4d) {
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



void Camera::updateCamera() {
	light_dir = vNormalizeVector(light_dir);
	//------------ Scan Line Mode -------------
	for (int m = 0; m < models.size(); m++) {
		for (int i = 0; i < models[m]->faces.size(); i++) {
			vector<int> pointsPerFace = models[m]->faces[i];
			//// Check Face Normal Direction
			int p1_index = pointsPerFace[0] - 1;
			int p2_index = pointsPerFace[1] - 1;
			int p3_index = pointsPerFace[2] - 1;
			float* p1 = new float[3]{ models[m]->getVerts(p1_index)[0], models[m]->getVerts(p1_index)[1], models[m]->getVerts(p1_index)[2] };
			float* p2 = new float[3]{ models[m]->getVerts(p2_index)[0], models[m]->getVerts(p2_index)[1], models[m]->getVerts(p2_index)[2] };
			float* p3 = new float[3]{ models[m]->getVerts(p3_index)[0], models[m]->getVerts(p3_index)[1], models[m]->getVerts(p3_index)[2] };
			float* edge1 = vvSub(p1, p2);
			float* edge2 = vvSub(p3, p2);
			float* N = vvCross(edge1, edge2);
			vector<float> N_face = { N[0], N[1], N[2] };
			float* face2cam = vvSub(camera, p2);
			float dotVal = vvDot(N, face2cam);
			delete[] p1;
			delete[] p2;
			delete[] p3;
			delete[] edge1;
			delete[] edge2;
			delete[] N;
			delete[] face2cam;

			if (dotVal <= -1) { // Make sure most surfaces are visible ( Modeling issue )  
				continue;
			}

			// Draw Scan Line Face
			vector<vector<float>> normalsPerFace;
			vector<vector<float>> colorsPerFace;
			vector<vector<float>> screenPointsPerFace;
			for (int j = 0; j < pointsPerFace.size(); j++) {
				int point = pointsPerFace[j] - 1;
				float* point4d = new float[4]{ models[m]->getVerts(point)[0], models[m]->getVerts(point)[1], models[m]->getVerts(point)[2], 1 };
				screenPointsPerFace.push_back(world2screen(point4d));
				delete[] point4d;

				if (shaderType == Phong) {
					normalsPerFace.push_back(models[m]->getNormal(i, j));
				}
				if (shaderType == Gouraud) {
					float vertex_intensity = vvDotVector(light_dir, models[m]->getNormal(i, j));
					vector<float> H = vNormalizeVector(vvAddVector(light_dir, vNormalizeVector(cameraVector)));
					float vertex_specular = pow(max(vvDotVector(models[m]->getNormal(i, j), H), 0.0f), 128);
					vector<float> vertex_color = {
						models[m]->baseColor[0] * vertex_intensity + vertex_specular * 2,
						models[m]->baseColor[1] * vertex_intensity + vertex_specular * 2,
						models[m]->baseColor[2] * vertex_intensity + vertex_specular * 2 };
					colorsPerFace.push_back(vertex_color);
				}
				if (shaderType == Flat) {
					normalsPerFace.push_back(N_face);
				}
			}
			if (shaderType == Phong)
				drawface(screenPointsPerFace, dotVal, normalsPerFace, models[m]->baseColor, Phong);		// Phong
			if (shaderType == Gouraud)
				drawface(screenPointsPerFace, dotVal, colorsPerFace, models[m]->baseColor, Gouraud);	// Gourand
			if (shaderType == Flat) {
				drawface(screenPointsPerFace, 1, normalsPerFace, models[m]->baseColor, Flat);	// Flat
			}

		}
	}

}

void Camera::clearAllBuffers() {
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = numeric_limits<float>::infinity();
		ibufferR[i] = 0.1;
		ibufferG[i] = 0.1;
		ibufferB[i] = 0.1;
	}
}

void Camera::renderImageBuffer() {
	glBegin(GL_POINTS);
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			glColor3f(ibufferR[y * width + x], ibufferG[y * width + x], ibufferB[y * width + x]);
			glVertex2f(x, y);
		}
	}
	glEnd();
}

void Camera::renderZBuffer() {
	glBegin(GL_POINTS);
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			glColor3f(zbuffer[y * width + x], zbuffer[y * width + x], zbuffer[y * width + x]);
			glVertex2f(x, y);
		}
	}
	glEnd();
}
