#pragma once


#include "Model.h"

GLfloat skyboxVertices[] = {
	// Positions          
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};

// Cubemap (Skybox)
std::vector<std::string> faces;

GLuint loadCubemap(std::vector<std::string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}


class SkyBox : public Model
{
public:
	SkyBox() {}
	SkyBox(ModelType Shape, char* Right, char* Left, char* Top, char* Bottom, char* Back, char* Front);
	void Create(ModelType Shape, char* Right, char* Left, char* Top, char* Bottom, char* Back, char* Front);
	void Initialise();
	void Render(GLuint program, Camera& camera);


	char* right, *left, *top, *bottom, *back, *front;

	char* SkyBoxTextures[6];
	GLuint SkyBoxTextureID[6];

	GLuint skyboxVAO, skyboxVBO;
	GLuint skyboxTexture;


};


void SkyBox::Render(GLuint program, Camera& camera)
{
	glUseProgram(program);

	GLint orignal;
	glGetIntegerv(GL_DEPTH_FUNC, &orignal);
	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(orignal); // Set depth function back to default
}

void SkyBox::Initialise()
{

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	faces.push_back(right);
	faces.push_back(left);
	faces.push_back(top);
	faces.push_back(bottom);
	faces.push_back(back);
	faces.push_back(front);
	 skyboxTexture = loadCubemap(faces);



}



SkyBox::SkyBox(ModelType Shape, char* Right, char* Left, char* Top, char* Bottom, char* Back, char* Front)
{
	m_Scale = glm::vec3(1, 1, 1);
	right = Right;
	left = Left;
	top = Top;
	bottom = Bottom;
	back = Back;
	front = Front;

	SkyBoxTextures[0] = right;
	SkyBoxTextures[1] = left;
	SkyBoxTextures[2] = top;
	SkyBoxTextures[3] = bottom;
	SkyBoxTextures[4] = back;
	SkyBoxTextures[5] = front;
}


void SkyBox::Create(ModelType Shape, char* Right, char* Left, char* Top, char* Bottom, char* Back, char* Front)
{
	m_Scale = glm::vec3(1, 1, 1);
	right = Right;
	left = Left;
	top = Top;
	bottom = Bottom;
	back = Back;
	front = Front;

	SkyBoxTextures[0] = right;
	SkyBoxTextures[1] = left;
	SkyBoxTextures[2] = top;
	SkyBoxTextures[3] = bottom;
	SkyBoxTextures[4] = back;
	SkyBoxTextures[5] = front;
}