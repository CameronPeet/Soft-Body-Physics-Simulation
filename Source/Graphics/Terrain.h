#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "Utils.h"
#include "Camera.h"
#include "Light.h"

class Terrain
{
public:
	Terrain() {}
	Terrain(std::wstring HeightmapFilename, std::string texFileName, std::string texFileName1, std::string texFileName2, GLuint program, Camera* camera, Light* light);
	~Terrain();
	void Render();


	Light* light;
	Camera* camera;
	std::string texFileName;
	std::string texFileName1;
	std::string texFileName2;

//Public Data
public:

	//Scale, Position and Rotation Components
	glm::quat m_Rotation;
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	glm::vec3 m_RotationPoint;

	//Attributes
	float Specular;
	float Ambient;
	glm::vec3 ObjectColor;


private:
	GLuint mNumVertices;
	GLuint mNumFaces;
	GLuint numRows = 513, numCols = 513;
	GLfloat heightScale = 0.35f, heightOffset = 0.0f, cellSpacing = 1.0f;
	std::vector<float> heightmap;
	std::vector<VertexFormat>vertices;
	std::vector<GLuint>indices;
	std::wstring heightmapFilename;

	GLuint vao, vbo, ebo, program, texture, texture1, texture2;

	void loadHeightMap();
	void smooth();
	float average(UINT i, UINT j);
	void createVerticesAndIndices();
	void buildBuffers();
	GLuint setTexture(std::string  texFileName);
};