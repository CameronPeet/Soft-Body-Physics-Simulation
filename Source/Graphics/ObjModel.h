#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

// GL Includes
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <soil\SOIL.h>

// GL Math Includes
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

//Assimp Model Loader Includes
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

//Local Includes
#include "Mesh.h"
#include "Camera.h"


class ObjModel
{
//Public functions
public:

	Camera* camera;
	GLuint program;

	// Constructor, expects a filepath to a 3D ObjModel.
	ObjModel() {}
	ObjModel(GLchar* path, Camera* camera, GLuint program);
	GLint TextureFromFile(const char* path, string directory);

	//Render all of the Objects Meshes
	void Render();	

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

//Private methods
private:

	void loadObjModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

//Private Data
private:
	/*  ObjModel Data  */
	vector<Mesh> meshes;
	string directory;
	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Texture> textures_loaded;



};
